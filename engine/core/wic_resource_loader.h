#pragma once
#include "pch.h"
#include "utils.h"
#include "image.h"

constexpr size_t RESOURCE_UPLOAD_BUFFER_SIZE_BYTES = 1024 * 1024 * 512;
constexpr size_t RESOURCE_UPLOAD_BUFFER_SIZE = RESOURCE_UPLOAD_BUFFER_SIZE_BYTES / D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT;

using Microsoft::WRL::ComPtr;
using namespace DirectX;
namespace fs = std::filesystem;

class wic_image_loader_c;

class wic_image_c : public image_c {
public:
	wic_image_c(wic_image_loader_c& owner, const fs::path& image_path);

private:
	HRESULT load_image(const fs::path& image_path);

private:
	const wic_image_loader_c& loader;
};

class wic_image_loader_c
{
	friend wic_image_c;
public:
	HRESULT create_resource_factory(D3D12MA::Allocator* allocator);
	void destroy_resource_factory();

	HRESULT upload_texture_image(
		ID3D12Device* device, 
		ID3D12GraphicsCommandList* command_list,
		const wic_image_c& image,
		int mip_level_id,
		int layer_id,
		const std::string& resource_name,
		int max_layers,
		int max_mips,
		D3D12MA::Allocation*& resource,
		D3D12_RESOURCE_DESC& desc,
		D3D12MA::Allocation*& upload_buffer);

private:
	IWICImagingFactory* wic_factory = nullptr;

	D3D12MA::Allocator* d3d_allocator = nullptr;
	D3D12MA::Allocation* upload_ring_buffer = nullptr;

	std::atomic<size_t> head = 0;
	std::atomic<size_t> tail = 0;
};