#pragma once
#include "pch.h"
#include "utils.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

#define ALIGN_CB(x) ((x + 255) & ~255)

class constant_buffer_c
{
public:
	void init();

	HRESULT allocate(std::wstring name, size_t size);
	HRESULT update_constant_buffer(BYTE* buffer, size_t size, size_t offset) const;
	void    copy_buffer_on_gpu(ID3D12GraphicsCommandList* command_list);

	size_t get_size() {	return buffer_size;	}
	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() { return gpu_buffer->GetResource()->GetGPUVirtualAddress(); }

private:
	D3D12MA::Allocator* gpu_allocator = nullptr;
	ID3D12Device* d3d_device = nullptr;

	D3D12MA::Allocation* gpu_upload_buffer = nullptr;
	D3D12MA::Allocation* gpu_buffer = nullptr;
	size_t buffer_size = 0;
};

class constant_buffers_manager_c
{
public:
	HRESULT create_constant_buffer(int& buffer_id, std::wstring name, size_t size);
	HRESULT update_constant_buffer(int& buffer_id, BYTE* buffer, size_t size, size_t offset = 0);
	void copy_buffers_on_gpu(ID3D12GraphicsCommandList* command_list);

	D3D12_GPU_VIRTUAL_ADDRESS get_buffer_gpu_virtual_address(int buffer_id) {
		return constant_buffers_array[buffer_id].GetGPUVirtualAddress();
	}
	size_t get_buffer_size(int buffer_id) {
		return constant_buffers_array[buffer_id].get_size();
	}

private:
	std::vector<constant_buffer_c> constant_buffers_array;

	int current_constant_buffer_handle_idx = 0;

	// Synchronization objects.
	HANDLE upload_fence_event = 0;
	UINT64 upload_fence_value = 0;
	ComPtr<ID3D12Fence> upload_fence;
};