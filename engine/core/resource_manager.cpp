#include "resource_manager.h"
#include "utils.h"
#include <regex>

static const std::regex REGEX_LOD_KEYWORD     ("_lod([0-9]+)");
static const std::regex REGEX_LAYER_KEYWORD   ("_layer([0-9]+)");
static const std::regex REGEX_CUBEMAP_KEYWORD ("_cubemap([0 - 9]+)");
static const std::regex REGEX_ALL_KEYWORD     ("((_lod)[0-9]+)*((_layer)[0-9]+)*((_cubemap)[0-9]+)*");

void resource_manager_c::init()
{
    init(fs::current_path());
}

resource_manager_c::RESOURCE_DESC_FROM_NAME resource_manager_c::get_desc_from_resource_name(std::string resource_name) {
    RESOURCE_DESC_FROM_NAME desc{};

    std::smatch matches_lod, matches_layer, matches_cubemap;
    std::regex_search(resource_name, matches_lod, REGEX_LOD_KEYWORD);
    std::regex_search(resource_name, matches_cubemap, REGEX_CUBEMAP_KEYWORD);
    std::regex_search(resource_name, matches_layer, REGEX_LAYER_KEYWORD);

    desc.layer_id = matches_cubemap.empty() ? desc.layer_id : stoi(matches_cubemap[1].str());
    desc.layer_id = matches_layer.empty() ? desc.layer_id : stoi(matches_layer[1].str());
    desc.lod_id = matches_lod.empty() ? desc.lod_id : stoi(matches_lod[1].str());

    return desc;
}

void resource_manager_c::init(const fs::path& root) {
    RESOURCE_ID current_resource_id = 0;
    SUBRESOURCE_ID current_subresource_id = 0;
    for (const fs::directory_entry& dir_entry : fs::recursive_directory_iterator(root)) {
        if (dir_entry.is_regular_file()) {

            auto relative_path = fs::relative(dir_entry.path(), root);

            std::string filename = relative_path.generic_string();
            std::string resource_name = std::regex_replace(filename, REGEX_ALL_KEYWORD, "");
            auto resource_desc_from_name = get_desc_from_resource_name(filename);

            if (!is_available(resource_name)) {
                GPU_RESOURCE resource{};
                resource_registry[resource_name] = current_resource_id;
                subresource_registry[filename] = current_subresource_id;

                resource.name = resource_name;
                resource.state = RESOURCE_NOT_LOADED;
                resource.subresources.push_back(current_subresource_id);
                resource.max_num_layers = std::max(resource.max_num_layers, resource_desc_from_name.layer_id + 1);
                resource.max_num_mip_maps = std::max(resource.max_num_mip_maps, resource_desc_from_name.lod_id + 1);

                gpu_resources.emplace_back(resource);
                current_resource_id++;
            } else {
                RESOURCE_ID resource_id = resource_registry[resource_name];
                GPU_RESOURCE& resource = gpu_resources[resource_id];

                resource.subresources.push_back(current_subresource_id);
                resource.max_num_layers = std::max(resource.max_num_layers, resource_desc_from_name.layer_id + 1);
                resource.max_num_mip_maps = std::max(resource.max_num_mip_maps, resource_desc_from_name.lod_id + 1);
            }

            GPU_SUBRESOURCE subresource;
            subresource.file_path = dir_entry.path();
            subresource.layer_id = resource_desc_from_name.layer_id;
            subresource.mip_id = resource_desc_from_name.lod_id;
            gpu_subresources.emplace_back(subresource);

            current_subresource_id++;
        }
    }
}


void resource_manager_c::start() {
    if (!scheduler) {
        scheduler = new std::thread(static_resource_manager_scheduler, this);
    }
}


void resource_manager_c::terminate() {
	if (scheduler) {
        load_tasks.push(TERMINATE_QUERY);
        scheduler->join();
        delete scheduler;
        scheduler = nullptr;
	}
}


bool resource_manager_c::is_available(const std::string& resource_name)
{
    std::lock_guard guard(resource_lock);
    return resource_registry.find(resource_name) != resource_registry.end();
}


resource_manager_c::QUERY_RESOURCE_STATE resource_manager_c::query_resource(RESOURCE_ID resource_id) {
    std::lock_guard	guard(resource_lock);
    auto &resource = gpu_resources[resource_id];

    if (resource.state == RESOURCE_NOT_LOADED) {
        const auto task_num = in_progress_tasks_num++;

        if (task_num < RESOURCE_LOADING_CONCURRENT_TASKS_MAX_NUM) {
            auto& task = load_tasks_pool[task_num];

            task.type = TEXTURE;
            task.resource_id = resource_id;

            load_tasks.push(&task);

            resource.state = RESOURCE_REQUESTED;
        } else {
            --in_progress_tasks_num;
            return BUSY;
        }
    }

    if (resource.state == RESOURCE_NOT_EXIST) {
        return NOT_EXIST;
    }

    if (resource.state == RESOURCE_AVAILABLE) {
        return AVAILABLE;
    }

    return LOADING;
}


resource_manager_c::QUERY_RESOURCE_STATE resource_manager_c::query_resource(const std::string& resource_id) {
    if (is_available(resource_id))
    {
        return query_resource(resource_registry[resource_id]);
    }
    return NOT_EXIST;
}


resource_manager_c::GPU_RESOURCE* resource_manager_c::get_resource(RESOURCE_ID resource_id) {
    if (gpu_resources[resource_id].state == RESOURCE_AVAILABLE) {
        return &gpu_resources[resource_id];
    }

    return nullptr;
}

resource_manager_c::GPU_RESOURCE* resource_manager_c::get_resource(const std::string& resource_id) {
    if (!is_available(resource_id)) {
        return nullptr;
    }

    return get_resource(resource_registry[resource_id]);
}


HRESULT resource_manager_c::create_resource_factory(renderer_c* renderer) {
    d3d_renderer = renderer;
    d3d_allocator = d3d_renderer->get_gpu_allocator();
    upload_queue = d3d_renderer->get_upload_command_queue();

    HRESULT hres;
    CK(wic_image_loader.create_resource_factory(d3d_allocator));

    CK(d3d_renderer->get_d3d_device()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&resource_fance)));
    resource_fence_value = 1;
    resource_fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (resource_fence_event == nullptr)
        return HRESULT_FROM_WIN32(GetLastError());

    return S_OK;
}


void resource_manager_c::destroy_resource_factory()
{
    if (scheduler) {
        terminate();
        scheduler = nullptr;
    }

    for (auto gpu_res : gpu_resources) {
        if (gpu_res.resource) {
            gpu_res.resource->Release();
        }
    }
    wic_image_loader.destroy_resource_factory();
}


HRESULT resource_manager_c::wait_for_resources_uploaded()
{
    HRESULT hres;
    const UINT64 fence = resource_fence_value;
    CK(upload_queue->Signal(resource_fance.Get(), fence));
    resource_fence_value++;

    if (resource_fance->GetCompletedValue() < fence)
    {
        CK(resource_fance->SetEventOnCompletion(fence, resource_fence_event));
        WaitForSingleObject(resource_fence_event, INFINITE);
    }
    return S_OK;
}


void resource_manager_c::release_upload_list(std::list<UPLOAD_TASK>& upload_list) {
    if (!upload_list.empty()) {
        wait_for_resources_uploaded();

        for (auto& upload : upload_list) {
            upload.upload_buffer->Release();
            gpu_resources[upload.resource_id].state = RESOURCE_AVAILABLE;
        }

        upload_list.clear();
    }
}


void resource_manager_c::resource_manager_scheduler()
{
    ID3D12GraphicsCommandList* upload_command_list = nullptr;
    std::list<UPLOAD_TASK> upload_list;

    bool running = true;
    while (running) {

        release_upload_list(upload_list);

        for (int i = 0; i < RESOURCE_UPLOAD_CHUNK_SIZE; i++) {
            LOAD_QUERY* load_query = nullptr;
            if (load_tasks.try_pop(load_query)) {
                if (load_query) {
                    if (!upload_command_list) {
                        d3d_renderer->begin_upload_command_list(&upload_command_list);
                    }

                    auto& resource = gpu_resources[load_query->resource_id];

                    for (SUBRESOURCE_ID subresource_id : resource.subresources) {
                        auto& subresource = gpu_subresources[subresource_id];
                        wic_image_c image(wic_image_loader, subresource.file_path);

                        D3D12MA::Allocation* upload_resource;
                        wic_image_loader.upload_texture_image(
                            d3d_renderer->get_d3d_device(),
                            upload_command_list,
                            image,
                            subresource.mip_id,
                            subresource.layer_id,
                            resource.name,
                            resource.max_num_layers,
                            resource.max_num_mip_maps,
                            resource.resource,
                            resource.desc,
                            upload_resource);

                        upload_list.push_back(UPLOAD_TASK{ upload_resource, load_query->resource_id });
                    }
                    --in_progress_tasks_num;
                } else {
                    running = false;
                    break;
                }
            } else {
                break;
            }
        }

        if (upload_command_list) {
            d3d_renderer->end_command_list(upload_command_list);

            ID3D12CommandList* ppCommandLists[] = { upload_command_list };
            upload_queue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
        }
    }

    release_upload_list(upload_list);
}