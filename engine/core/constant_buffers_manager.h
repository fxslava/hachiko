#pragma once
#include "pch.h"
#include "utils.h"
#include "hachiko_api.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

struct CAMERA_CB
{
	XMFLOAT4X4A view_proj_mat;

	CAMERA_CB()
	{
		XMStoreFloat4x4A(&view_proj_mat, XMMatrixIdentity());
	}
};

struct OBJECT_CB
{
	XMFLOAT4X4A world_mat;
};

struct ENGINE_COMMON_CB
{
	CAMERA_CB main_camera;
	OBJECT_CB world;
	XMFLOAT4  padding[8];
};

constexpr size_t ENGINE_COMMON_CB_SIZE = sizeof(ENGINE_COMMON_CB);

class constant_buffers_manager_c
{
public:
	ENGINE_COMMON_CB engine_common;

	HRESULT allocate_resources(renderer_i* renderer);
	void destroy_resources();
	HRESULT begin(ENGINE_COMMON_CB*& common_cb);
	void end(ID3D12GraphicsCommandList* command_list, D3D12_RANGE& written_range);

	ID3D12DescriptorHeap* get_cbv_heap()
	{
		return cbv_heap.Get();
	}
private:

private:
	D3D12MA::Allocation* engine_common_cb = nullptr;
	D3D12MA::Allocation* upload_engine_common_cb = nullptr;
	ComPtr<ID3D12Device> d3d_device;
	ComPtr<ID3D12DescriptorHeap> cbv_heap;

	ENGINE_COMMON_CB* engine_common_cb_ptr = nullptr;
};