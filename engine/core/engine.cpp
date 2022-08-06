#include "engine.h"
#include "utils.h"

#define INSTANTIATE_ENGINE_LOWLEVEL \
    gpu_allocator = d3d_renderer.get_gpu_allocator(); \
    d3d_device = d3d_renderer.get_d3d_device();

HRESULT engine_c::init_sub_systems(const UINT width, const UINT height, UINT num_back_buffer_frames, const D3D_FEATURE_LEVEL feature_level, HWND wnd_handle)
{
    HRESULT hres;
    CK(d3d_renderer.create_pipeline(width, height, num_back_buffer_frames, feature_level, wnd_handle));

    INSTANTIATE_ENGINE_LOWLEVEL

    CK(resource_manager.create_resource_factory(&d3d_renderer));

    resource_manager.init(fs::current_path() / fs::path("resources"));
    resource_manager.start();

    CK(constant_buffers_manager.create_constant_buffer(common_engine_cb_handle, L"COMMON_ENGINE", sizeof(common_engine_cb)));

    gpu_heaps_manager.create(num_back_buffer_frames);

    CK(init_imgui(wnd_handle, num_back_buffer_frames));

    return S_OK;
}   


HRESULT engine_c::init_imgui(HWND wnd_handle, UINT num_back_buffer_frames) 
{
    auto imgui_srv_desc_handle = gpu_heaps_manager.static_alloc(1);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(wnd_handle);
    ImGui_ImplDX12_Init(d3d_renderer.get_d3d_device(), num_back_buffer_frames,
        DXGI_FORMAT_R8G8B8A8_UNORM, imgui_srv_desc_handle.p_heap,
        imgui_srv_desc_handle.cpu_handle,
        imgui_srv_desc_handle.gpu_handle);

    return S_OK;
}


void engine_c::shut_down()
{
    resource_manager.destroy_resource_factory();
}


HRESULT engine_c::prepare_frame(ID3D12GraphicsCommandList* command_list)
{
    HRESULT hres = constant_buffers_manager.update_constant_buffer(common_engine_cb_handle, reinterpret_cast<BYTE*>(&common_engine_cb), sizeof(common_engine_cb));
    constant_buffers_manager.copy_buffers_on_gpu(command_list);

    ID3D12DescriptorHeap* current_heap = gpu_heaps_manager.get_descriptor_heap();
    gpu_heaps_manager.next_frame();

    command_list->SetDescriptorHeaps(1, &current_heap);

    return hres;
}