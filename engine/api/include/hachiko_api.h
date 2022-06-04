#pragma once
#include <windows.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl.h>

#include "D3D12MemAlloc.h"
#include "input_controller_api.h"

// <<---------------------------------- RENDERER         ---------------------------------->> 

class renderer_i {
public:
	virtual HRESULT create_pipeline(UINT width, UINT height, D3D_FEATURE_LEVEL feature_level, HWND wnd_handle) = 0;
	virtual HRESULT wait_for_prev_frame() = 0;
	virtual HRESULT begin_command_list(ID3D12GraphicsCommandList** command_list) = 0;
	virtual HRESULT end_command_list(ID3D12GraphicsCommandList* command_list) = 0;
	virtual HRESULT clear_render_target(ID3D12GraphicsCommandList* command_list, float clear_color[4]) = 0;
	virtual HRESULT on_render() = 0;

	virtual ID3D12Device* get_d3d_device() = 0;
	virtual D3D12MA::Allocator* get_gpu_allocator() = 0;

	virtual void destroy_pipeline() = 0;
};

void create_render_device(renderer_i*& renderer);
void destroy_render_device(renderer_i* renderer);