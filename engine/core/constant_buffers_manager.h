#pragma once
#include "pch.h"
#include "utils.h"
#include "renderer.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

#define ALIGN_CB(x) ((x + 255) & ~255)

class constant_buffer_c
{
public:
	constant_buffer_c(renderer_c* renderer) : d3d_renderer(renderer)
	{
		gpu_allocator = renderer->get_gpu_allocator();
		d3d_device    = renderer->get_d3d_device();
	};

	HRESULT allocate(std::wstring name, size_t size);
	HRESULT update_constant_buffer(BYTE* buffer, size_t size, size_t offset) const;
	void copy_buffer_on_gpu(ID3D12GraphicsCommandList* command_list);

	size_t get_size() {	return buffer_size;	}
	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() { return gpu_buffer->GetResource()->GetGPUVirtualAddress(); }

private:
	ComPtr<D3D12MA::Allocation> gpu_upload_buffer;
	ComPtr<D3D12MA::Allocation> gpu_buffer;
	ComPtr<D3D12MA::Allocator>  gpu_allocator;
	ComPtr<ID3D12Device>        d3d_device;

	const renderer_c* d3d_renderer;
	size_t buffer_size = 0;
};

constexpr uint8_t CONSTANT_BUFFER_INVALID_HANDLE = 255;
using CONSTANT_BUFFER_HANDLE = uint8_t;

class constant_buffers_manager_c
{
public:
	void init(renderer_c* renderer)
	{
		d3d_renderer = renderer;
		gpu_allocator = renderer->get_gpu_allocator();
		d3d_device = renderer->get_d3d_device();
	}

	HRESULT create_constant_buffer(CONSTANT_BUFFER_HANDLE& cb_handle, std::wstring name, size_t size);
	HRESULT update_constant_buffer(CONSTANT_BUFFER_HANDLE& cb_handle, BYTE* buffer, size_t size, size_t offset = 0);
	HRESULT get_cbv_heap(std::vector<CONSTANT_BUFFER_HANDLE>& cb_handle_array, ID3D12DescriptorHeap*& cbv_heap);
	void copy_buffers_on_gpu(ID3D12GraphicsCommandList* command_list);

private:
	struct CBV_HEAP_ID
	{
		union {
			uint64_t raw;
			CONSTANT_BUFFER_HANDLE handles[8];
		} ID_ARRAY;

		CBV_HEAP_ID(std::vector<CONSTANT_BUFFER_HANDLE>& cb_handle_array)
		{
			int idx = 0;
			assert(cb_handle_array.size() < _countof(ID_ARRAY.handles));
			for (int idx = 0; idx < _countof(ID_ARRAY.handles); idx++) {
				if (idx < cb_handle_array.size()) {
					ID_ARRAY.handles[idx] = cb_handle_array[idx];
				} else {
					ID_ARRAY.handles[idx] = CONSTANT_BUFFER_INVALID_HANDLE;
				}
			}
		}
	};

	bool is_cbv_heap_available_in_cache(const CBV_HEAP_ID& heap_id) const
	{
		return cbv_heap_cache.find(heap_id.ID_ARRAY.raw) != cbv_heap_cache.end();
	}

	HRESULT create_cbv_heap(const CBV_HEAP_ID& heap_id, std::vector<CONSTANT_BUFFER_HANDLE>& cb_handle_array, ID3D12DescriptorHeap*& cbv_heap);

private:
	std::vector<constant_buffer_c> constant_buffers_array;
	std::map<uint64_t, ID3D12DescriptorHeap*> cbv_heap_cache;

	int current_constant_buffer_handle_idx = 0;

	ComPtr<ID3D12Device>         d3d_device;
	ComPtr<D3D12MA::Allocator>   gpu_allocator;

	renderer_c* d3d_renderer = nullptr;
};