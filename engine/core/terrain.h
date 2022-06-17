#pragma once
#include <vector>
#include "game_actors.h"
#include "shader_pass.h"
#include "resource_manager.h"
#include "D3D12MemAlloc.h"
#include "DirectXMath.h"
#include "DirectXCollision.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

class terrain_base_c : public terrain_i
{
public:
	~terrain_base_c()
	{
		if (vertex_buffer) {
			vertex_buffer->Release();
		}
		resource_manager.destroy_resource_factory();
	}
	HRESULT allocate_resources(renderer_i* renderer);
	void render(ID3D12GraphicsCommandList* command_list);

	HRESULT update();
protected:

	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	D3D12MA::Allocation* vertex_buffer = nullptr;
	D3D12MA::Allocation* constant_buffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view;
	shader_pass_c sample_shader_pass;
	resource_manager_c resource_manager;

	ComPtr<ID3D12Device> d3d_device;
	ComPtr<ID3D12DescriptorHeap> srv_heap;
	ComPtr<ID3D12DescriptorHeap> cbv_heap;

	bool srv_heap_not_empty = false;
};