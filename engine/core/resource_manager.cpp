#include "resource_manager.h"
#include "utils.h"


void resource_manager_c::init()
{
    init(fs::current_path());
}

void resource_manager_c::init(const fs::path& root) {
    RESOURCE_ID resource_id = 0;
    for (const fs::directory_entry& dir_entry : fs::recursive_directory_iterator(root)) {
        if (dir_entry.is_regular_file()) {

            auto relative_path = fs::relative(dir_entry.path(), root);

            std::wstring filename = relative_path.generic_wstring();
            recource_registry[filename] = resource_id;
            resource_names.push_back(filename);

            RESOURCE_ENTITY resource_entity;
            resource_entity.state = RESOURCE_NOT_LOADED;
            resource_entity.resource_path = dir_entry.path();

            resources.emplace_back(resource_entity);
            resource_id++;
        }
    }

    gpu_resources.resize(resource_id);
    std::fill(gpu_resources.begin(), gpu_resources.end(), nullptr);
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


resource_manager_c::~resource_manager_c() {
    if (scheduler) {
        terminate();
    }
}


bool resource_manager_c::is_available(const std::wstring& resource_id)
{
    std::lock_guard guard(resource_lock);
    return recource_registry.find(resource_id) != recource_registry.end();
}


resource_manager_c::QUERY_RESOURCE_STATE resource_manager_c::query_resource(RESOURCE_ID resource_id) {
    std::lock_guard	guard(resource_lock);
    auto &resource = resources[resource_id];

    if (resource.state == RESOURCE_NOT_LOADED) 
    {
        const auto task_num = in_progress_tasks_num++;
        if (task_num < RESOURCE_LOADING_CONCURRENT_TASKS_MAX_NUM) {
            auto& task = load_tasks_pool[task_num];

            task.type = TEXTURE;
            task.resource_path = resource.resource_path;
            task.resource_id = resource_id;

            load_tasks.push(&task);

            resource.state = RESOURCE_REQUESTED;
        } else {
            --in_progress_tasks_num;
            return BUSY;
        }
    }

    if (resource.state == RESOURCE_NOT_EXIST)
    {
        return NOT_EXIST;
    }

    if (resource.state == RESOURCE_AVAILABLE)
    {
        return AVAILABLE;
    }

    return LOADING;
}


resource_manager_c::QUERY_RESOURCE_STATE resource_manager_c::query_resource(const std::wstring& resource_id)
{
    if (is_available(resource_id))
    {
        return query_resource(recource_registry[resource_id]);
    }
    return NOT_EXIST;
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


void resource_manager_c::resource_manager_scheduler()
{
    std::list<resource_payload_t> payload;

    bool running = true;
    while (running) {

        if (!payload.empty()) {
            wait_for_resources_uploaded();

            for (const auto& payload_element : payload) {
                wic_image_loader.pay(payload_element.texture_payload);
                resources[payload_element.resource_id].state = RESOURCE_AVAILABLE;
            }

            payload.clear();
        }

        ID3D12GraphicsCommandList* upload_list = nullptr;

        for (int i = 0; i < RESOURCE_UPLOAD_CHUNK_SIZE; i++) {
            LOAD_QUERY* load_query = nullptr;
            if (load_tasks.try_pop(load_query))
            {
                if (load_query) {
                    --in_progress_tasks_num;

                    if (!upload_list) {
                        d3d_renderer->begin_upload_command_list(&upload_list);
                    }

                	wic_image_loader_c::payload_t payload_element;
                    wic_image_loader.load_texture(
                        d3d_renderer->get_d3d_device(), 
                        upload_list, 
                        load_query->resource_path, 
                        resource_names[load_query->resource_id], 
                        payload_element, 
                        gpu_resources[load_query->resource_id]);

                    payload.push_back(resource_payload_t{ payload_element, load_query->resource_id });
                }
                else
                {
                    running = false;
                    break;
                }
            } else
            {
                break;
            }
        }

        if (upload_list) {
            d3d_renderer->end_command_list(upload_list);

            ID3D12CommandList* ppCommandLists[] = { upload_list };
            upload_queue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
        }
    }

    if (!payload.empty()) {
        wait_for_resources_uploaded();

        for (const auto& payload_element : payload) {
            wic_image_loader.pay(payload_element);
        }

        payload.clear();
    }
}


HRESULT resource_manager_c::update()
{
    ComPtr<ID3D12GraphicsCommandList> d3d_upload_command_list;
    d3d_renderer->begin_upload_command_list(&d3d_upload_command_list);
    return S_OK;
}