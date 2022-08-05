#pragma once
#include "pch.h"
#include "renderer.h"
#include "resource_manager.h"
#include "constant_buffers_manager.h"

class engine_c
{
public:
    static engine_c& get_instance()
    {
        static engine_c instance;
    	return instance;
    }

    struct {
        XMFLOAT4X4A view_proj_mat = XMFLOAT4X4A(0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
        XMFLOAT4X4A world_mat = XMFLOAT4X4A(0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
    } common_engine_cb;
    CONSTANT_BUFFER_HANDLE common_engine_cb_handle = CONSTANT_BUFFER_INVALID_HANDLE;

    HRESULT init_sub_systems(const UINT width, const UINT height, const D3D_FEATURE_LEVEL feature_level, HWND wnd_handle);
    HRESULT prepare_frame(ID3D12GraphicsCommandList* command_list);
    void shut_down();

    renderer_c*                 get_renderer()                 { return &d3d_renderer; }
    resource_manager_c*         get_resource_manager()         { return &resource_manager; }
    constant_buffers_manager_c* get_constant_buffers_manager() { return &constant_buffers_manager; }
private:
    engine_c() = default;
    ~engine_c() = default;

    engine_c(engine_c const&) = delete;
    void operator=(engine_c const&) = delete;


protected:
    renderer_c                  d3d_renderer;
    resource_manager_c          resource_manager;
    constant_buffers_manager_c  constant_buffers_manager;
};