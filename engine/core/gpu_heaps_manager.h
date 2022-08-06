#pragma once
#include "pch.h"
#include "renderer.h"

constexpr int GPU_HEAP_DEFAULT_DESCRIPTORS_CHUNK_SIZE   = 64;
constexpr int GPU_HEAP_DEFAULT_DESCRIPTORS_COUNT        = 1024;
constexpr int GPU_STATIC_HEAP_DEFAULT_DESCRIPTORS_COUNT = 1024;
constexpr int GPU_HEAP_DEFAULT_HEAPS_NUM                 = 2;

using namespace DirectX;
using Microsoft::WRL::ComPtr;

struct DESCRIPTOR_HEAP_ID {
	int descriptor_idx = 0;
	bool static_descriptor = true;
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle;
};

class cpu_heap_c {
public:
	HRESULT create(int num_descriptors = GPU_HEAP_DEFAULT_DESCRIPTORS_CHUNK_SIZE, D3D12_DESCRIPTOR_HEAP_TYPE heap_type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	ID3D12DescriptorHeap* get_heap() { return srv_heap.Get(); }
	bool allocate(DESCRIPTOR_HEAP_ID& desc_id, int num_descriptors);
	void reset();

private:
	renderer_c* d3d_renderer = nullptr;
	ID3D12Device* d3d_device = nullptr;

	uint32_t descriptor_size = 0;
	int current_descriptor_id = 0;
	int num_descriptors = 1;
	int available_descriptors = 1;
	CD3DX12_CPU_DESCRIPTOR_HANDLE current_cpu_descriptor_handle;
	ComPtr<ID3D12DescriptorHeap> srv_heap;
};


class gpu_heap_c {
public:
	HRESULT create(int num_descriptors, D3D12_DESCRIPTOR_HEAP_TYPE heap_type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	HRESULT resize(int new_descriptors_num);
	HRESULT flush_heaps(int offset, std::vector<ID3D12DescriptorHeap*> heaps);
	bool empty() { return !srv_heap; }
	ID3D12DescriptorHeap* get_descriptor_heap() { return srv_heap.Get(); }

protected:
	HRESULT create_heap(int new_descriptors_num, ID3D12DescriptorHeap** heap);

private:
	renderer_c* d3d_renderer = nullptr;
	ID3D12Device* d3d_device = nullptr;

	int num_descriptors = 1;
	uint32_t descriptor_size = 0;
	D3D12_DESCRIPTOR_HEAP_TYPE heap_type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	ComPtr<ID3D12DescriptorHeap> srv_heap{};
};


class gpu_heap_allocator_c {
public:
	void init(int descriptors_chunk_size = GPU_HEAP_DEFAULT_DESCRIPTORS_CHUNK_SIZE, D3D12_DESCRIPTOR_HEAP_TYPE heap_type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	size_t get_heaps_count() {
		return dynamic_heaps.empty() ? 0 : dynamic_heaps.size();
	}
	void get_heap_pool(ID3D12DescriptorHeap** p_heaps);
	DESCRIPTOR_HEAP_ID dynamic_alloc(int num_descriptors);
	void reset();

protected:
	DESCRIPTOR_HEAP_ID current_descriptor_id{};
	std::vector<cpu_heap_c> dynamic_heaps;
	int descriptors_chunk_size = GPU_HEAP_DEFAULT_DESCRIPTORS_CHUNK_SIZE;
	D3D12_DESCRIPTOR_HEAP_TYPE heap_type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
};


struct DESCRIPTOR_MANAGER_HANDLE {
	ID3D12DescriptorHeap* p_heap;
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpu_handle;
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpu_handle;
};


class gpu_heaps_manager_c {
public:

	HRESULT create(
		int num_back_buffer_frames = 2, 
		int descriptors_chunk_size = GPU_HEAP_DEFAULT_DESCRIPTORS_CHUNK_SIZE);

	DESCRIPTOR_HEAP_ID dynamic_alloc(int num_descriptors);
	DESCRIPTOR_MANAGER_HANDLE static_alloc(int num_descriptors);

	void next_frame();

	CD3DX12_GPU_DESCRIPTOR_HANDLE get_gpu_descriptor_handle(DESCRIPTOR_HEAP_ID descriptor_id) {
		auto heap = gpu_heap.get_descriptor_heap();
		int offset = descriptor_id.descriptor_idx;
		if (!descriptor_id.static_descriptor) {
			offset += current_descriptor_id.descriptor_idx;
		}
		return CD3DX12_GPU_DESCRIPTOR_HANDLE(heap->GetGPUDescriptorHandleForHeapStart(), offset, descriptor_size);
	}

	ID3D12DescriptorHeap* get_descriptor_heap() { return gpu_heap.get_descriptor_heap(); }

protected:
	void assembly_heaps_current_list();
	
private:
	renderer_c* d3d_renderer = nullptr;
	ID3D12Device* d3d_device = nullptr;

	std::vector<ID3D12DescriptorHeap*> current_heaps;
	gpu_heap_allocator_c dynamic_heap_pool;
	gpu_heap_c gpu_heap;

	int num_back_buffer_frames = 2;
	int current_frame_idx = 0;
	UINT descriptor_size = 0;
	DESCRIPTOR_HEAP_ID current_descriptor_id{};
};