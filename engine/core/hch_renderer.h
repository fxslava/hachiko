#pragma once
#include <assert.h>
#include <vector>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

class hch_render_device_c {
public:
	HRESULT create_pipeline(D3D_FEATURE_LEVEL feature_level, HWND wnd_handle);
	HRESULT wait_for_prev_frame();
	HRESULT prerecord_render();
	HRESULT on_render();
	void destroy_pipeline();

private:
	std::vector <IDXGIAdapter1*> enum_adapters(D3D_FEATURE_LEVEL feature_level);
	HRESULT create_dxgi_factory();
	HRESULT create_command_queues();
	HRESULT create_direct_command_list();
	HRESULT create_swap_chain(UINT frame_num, UINT& frame_idx);
	HRESULT create_rtv_desc_heap(UINT num_descriptors);
	HRESULT create_render_target_view(UINT frame_num);

	ComPtr<IDXGIFactory4>               dxgi_factory;
	ComPtr<ID3D12Device>                d3d_device;
	ComPtr<IDXGISwapChain3>             d3d_swap_chain;
	ComPtr<ID3D12DescriptorHeap>        d3d_rtv_heap;
	ComPtr<ID3D12CommandQueue>          d3d_direct_queue;
	ComPtr<ID3D12CommandAllocator>      d3d_command_allocator;
	ComPtr<ID3D12GraphicsCommandList>   d3d_direct_command_list;
	std::vector<ComPtr<ID3D12Resource>> d3d_render_targets{};
	ComPtr<ID3D12PipelineState>         d3d_pipeline_state;

	INT64 rtv_desc_size = 0;
	UINT swap_chain_frame_num = 3;
	UINT current_render_target_idx = 0;

	// Synchronization objects.
	UINT   frame_idx = 0;
	HANDLE frame_fence_event = 0;
	UINT64 frame_fence_value = 0;
	ComPtr<ID3D12Fence> frame_fance;

	// hWnd
	HWND wnd_handle = 0;
};