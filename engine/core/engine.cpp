#include "engine.h"
#include "utils.h"

HRESULT engine_c::init_sub_systems(const UINT width, const UINT height, const D3D_FEATURE_LEVEL feature_level, HWND wnd_handle)
{
    HRESULT hres;
    CK(d3d_renderer.create_pipeline(width, height, feature_level, wnd_handle));
    CK(resource_manager.create_resource_factory(&d3d_renderer));

    resource_manager.init(fs::current_path() / fs::path("resources"));
    resource_manager.start();

    constant_buffers_manager.init(&d3d_renderer);
    constant_buffers_manager.create_constant_buffer(common_engine_cb_handle, L"COMMON_ENGINE", sizeof(common_engine_cb));

    return S_OK;
}


void engine_c::shut_down()
{
    resource_manager.destroy_resource_factory();
}


HRESULT engine_c::update(ID3D12GraphicsCommandList* command_list)
{
    HRESULT hres = constant_buffers_manager.update_constant_buffer(common_engine_cb_handle, reinterpret_cast<BYTE*>(&common_engine_cb), sizeof(common_engine_cb));
    constant_buffers_manager.copy_buffers_on_gpu(command_list);
    return hres;
}