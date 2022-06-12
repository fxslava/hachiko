#pragma once
#define NOMINMAX
#include <windows.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl.h>
#include <wincodec.h>

#include <atomic>

#include "utils.h"
#include "d3dx12.h"
#include "D3D12MemAlloc.h"
#include <filesystem>

constexpr size_t RESOURCE_UPLOAD_BUFFER_SIZE_BYTES = 1024 * 1024 * 512;
constexpr size_t RESOURCE_UPLOAD_BUFFER_SIZE = RESOURCE_UPLOAD_BUFFER_SIZE_BYTES / D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT;

using Microsoft::WRL::ComPtr;
namespace fs = std::filesystem;

class wic_image_loader_c
{
private:
	static size_t get_ring_pointer(size_t ring_index) {
		return (ring_index % RESOURCE_UPLOAD_BUFFER_SIZE) * D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT;
	}

	static size_t get_ring_index(size_t gpu_pointer) {
		return (gpu_pointer + D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT - 1) / D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT;
	}

	struct ring_allocation_t
	{
		ring_allocation_t(size_t index, size_t size) : ring_index(index), size(size) {
			gpu_pointer = get_ring_pointer(ring_index);
		}

		size_t ring_index = 0;
		size_t gpu_pointer = 0;
		size_t size = 0;
	};

	ring_allocation_t alloc_ring(size_t size) {
		auto sz = get_ring_index(size);
		while (true) {
			size_t old_head = head.load();
			if ((old_head + sz) < (tail.load() + RESOURCE_UPLOAD_BUFFER_SIZE)) {
				if (head.compare_exchange_strong(old_head, old_head + sz)) {
					return ring_allocation_t(old_head, sz);
				}
			}
		}
	}

	void free_ring(ring_allocation_t allocation) {
		while (tail.compare_exchange_strong(allocation.ring_index, allocation.ring_index + allocation.size)) {};
	}
public:
	struct payload_t
	{
		ring_allocation_t allocation{0,0};
	};

	HRESULT create_resource_factory(D3D12MA::Allocator* allocator);
	HRESULT load_texture(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, const fs::path& texture_path, const std::wstring& resource_name, payload_t& payload, D3D12MA::Allocation*& texture);
	void pay(payload_t payload)
	{
		free_ring(payload.allocation);
	}

private:
	ComPtr<IWICImagingFactory> wic_factory;

	D3D12MA::Allocator* d3d_allocator = nullptr;
	D3D12MA::Allocation* upload_ring_buffer = nullptr;

	std::atomic<size_t> head = 0;
	std::atomic<size_t> tail = 0;
};