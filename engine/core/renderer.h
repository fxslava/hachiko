#pragma once
#include <assert.h>
#include <vector>
#include "hachiko_api.h"
#include "d3dx12.h"
#include "DirectXMath.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

class renderer_c : public renderer_i {
public:
	HRESULT create_pipeline(UINT width, UINT height, UINT num_back_buffer_frames, D3D_FEATURE_LEVEL feature_level, HWND wnd_handle);
	HRESULT wait_for_prev_frame();
	HRESULT begin_render(ID3D12GraphicsCommandList** command_list);
	HRESULT end_render(ID3D12GraphicsCommandList* command_list);
	HRESULT clear_render_target(float clear_color[4]);
	HRESULT begin_upload_command_list(ID3D12GraphicsCommandList** command_list);
	HRESULT end_command_list(ID3D12GraphicsCommandList* command_list);

	ID3D12Device*       get_d3d_device()           { return d3d_device.Get(); };
	ID3D12CommandQueue* get_upload_command_queue() { return d3d_upload_queue.Get(); }
	D3D12MA::Allocator* get_gpu_allocator()        { return gpu_allocator; };

	~renderer_c() {
		destroy_pipeline();
	}

private:
	std::vector <IDXGIAdapter1*> enum_adapters(D3D_FEATURE_LEVEL feature_level);
	HRESULT create_dxgi_factory();
	HRESULT create_command_queues();
	HRESULT create_direct_command_lists();
	HRESULT create_upload_command_list();
	HRESULT create_swap_chain(UINT frame_num, UINT& frame_idx);
	HRESULT create_depth_buffer();
	HRESULT create_descriptor_heaps(UINT num_descriptors);
	HRESULT create_render_target_view(UINT frame_num);
	void    destroy_pipeline();

	ComPtr<IDXGIFactory4>               dxgi_factory;
	ComPtr<ID3D12Device>                d3d_device;
	ComPtr<IDXGISwapChain3>             d3d_swap_chain;
	ComPtr<ID3D12DescriptorHeap>        d3d_rtv_heap;
	ComPtr<ID3D12DescriptorHeap>        d3d_dsv_heap;
	ComPtr<ID3D12CommandQueue>          d3d_direct_queue;
	ComPtr<ID3D12CommandQueue>          d3d_upload_queue;
	ComPtr<ID3D12CommandAllocator>      d3d_command_allocator;
	ComPtr<ID3D12CommandAllocator>      render_command_allocator;
	ComPtr<ID3D12CommandAllocator>      copy_command_allocator;
	ComPtr<ID3D12GraphicsCommandList>   d3d_render_command_list;
	ComPtr<ID3D12GraphicsCommandList>   d3d_upload_command_list;
	std::vector<ComPtr<ID3D12Resource>> d3d_render_targets{};
	D3D12MA::Allocation*                d3d_depth_buffer = nullptr;

	CD3DX12_VIEWPORT d3d_viewport;
	CD3DX12_RECT d3d_scissor_rect;
	UINT rtv_desc_size = 0;
	UINT swap_chain_frame_num = 2;
	UINT current_render_target_idx = 0;

	XMINT2 screen_resolution = XMINT2(0,0);

	// Synchronization objects.
	UINT   frame_idx = 0;
	HANDLE frame_fence_event = 0;
	UINT64 frame_fence_value = 0;
	ComPtr<ID3D12Fence> frame_fance;

	// Memory managment
	D3D12MA::Allocator*	gpu_allocator = nullptr;

	// hWnd
	HWND wnd_handle = 0;
};