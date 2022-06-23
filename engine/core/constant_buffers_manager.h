#pragma once
#include "pch.h"
#include "utils.h"
#include "hachiko_api.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

#define ALIGN_CB(x) ((x + 255) & ~255)

enum CONSTANT_BUFFER_TYPE {
	CB_TYPE_ENGINE_COMMON,
	CB_TYPE_OBJECT_COMMON
};

struct ENGINE_COMMON_CB
{
	XMFLOAT4X4A main_camera;
};

struct OBJECT_COMMON_CB
{
	XMFLOAT4X4A world;
};

constexpr size_t ENGINE_COMMON_CB_SIZE = ALIGN_CB(sizeof(ENGINE_COMMON_CB));
constexpr size_t OBJECT_COMMON_CB_SIZE = ALIGN_CB(sizeof(OBJECT_COMMON_CB));
constexpr size_t TOTAL_CB_SIZE = ENGINE_COMMON_CB_SIZE + OBJECT_COMMON_CB_SIZE;

class constant_buffers_manager_c
{
public:
	ENGINE_COMMON_CB engine_common;

	HRESULT allocate_resources(renderer_i* renderer);
	void destroy_resources();
	HRESULT begin(ENGINE_COMMON_CB*& common_cb, OBJECT_COMMON_CB*& object_cb);
	void end(ID3D12GraphicsCommandList* command_list);

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
};