#include "engine.h"
#include "utils.h"

HRESULT engine_c::init_sub_systems(const UINT width, const UINT height, const D3D_FEATURE_LEVEL feature_level, HWND wnd_handle)
{
    HRESULT hres;
    CK(d3d_renderer.create_pipeline(width, height, feature_level, wnd_handle));
    CK(resource_manager.create_resource_factory(&d3d_renderer));

    resource_manager.init(fs::current_path() / fs::path("resources"));
    resource_manager.start();

    CK(constant_buffers_manager.allocate_resources(&d3d_renderer));

    return S_OK;
}


void engine_c::shut_down()
{
    constant_buffers_manager.destroy_resources();
    resource_manager.destroy_resource_factory();
}