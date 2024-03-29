#pragma once
#include "pch.h"
#include "renderer.h"
#include "resource_manager.h"
#include "gpu_heaps_manager.h"
#include "constant_buffers_manager.h"

//#define ENABLE_IMGUI

#define INSTANTIATE_GAME_ENGINE \
	auto& engine = engine_c::get_instance(); \
	d3d_renderer = engine.get_renderer(); \
	resource_manager = engine.get_resource_manager(); \
    gpu_heaps_manager = engine.get_heaps_manager(); \
	constant_buffers_manager = engine.get_constant_buffers_manager();

#define INSTANTIATE_GAME_ENGINE_LOWLEVEL \
    gpu_allocator = d3d_renderer->get_gpu_allocator(); \
    d3d_device = d3d_renderer->get_d3d_device();

#define GAME_ENGINE_INSTANCES \
	renderer_c* d3d_renderer = nullptr; \
	resource_manager_c* resource_manager = nullptr; \
    gpu_heaps_manager_c* gpu_heaps_manager = nullptr; \
	constant_buffers_manager_c* constant_buffers_manager = nullptr;

#define LOWLEVEL_GAME_ENGINE_INSTANCES \
    D3D12MA::Allocator* gpu_allocator = nullptr; \
    ID3D12Device* d3d_device = nullptr;

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
    int common_engine_cb_handle = -1;

    HRESULT init_sub_systems(const UINT width, const UINT height, UINT num_back_buffer_frames, const D3D_FEATURE_LEVEL feature_level, HWND wnd_handle);
    HRESULT prepare_frame(ID3D12GraphicsCommandList* command_list);
    void shut_down();

    renderer_c*                 get_renderer()                 { return &d3d_renderer; }
    resource_manager_c*         get_resource_manager()         { return &resource_manager; }
    gpu_heaps_manager_c*        get_heaps_manager()            { return &gpu_heaps_manager; }
    constant_buffers_manager_c* get_constant_buffers_manager() { return &constant_buffers_manager; }
private:
    engine_c() = default;
    ~engine_c() = default;

    engine_c(engine_c const&) = delete;
    void operator=(engine_c const&) = delete;


protected:
    HRESULT init_imgui(HWND wnd_handle, UINT num_back_buffer_frames);

protected:
    LOWLEVEL_GAME_ENGINE_INSTANCES

    renderer_c                  d3d_renderer;
    resource_manager_c          resource_manager;
    gpu_heaps_manager_c         gpu_heaps_manager;
    constant_buffers_manager_c  constant_buffers_manager;
};

#ifdef ENABLE_IMGUI
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif