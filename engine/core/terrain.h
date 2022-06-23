#pragma once
#include <vector>

#include "game_actors.h"
#include "shader_pass.h"
#include "resource_manager.h"
#include "constant_buffers_manager.h"

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
	}
	HRESULT allocate_resources();
	void render(ID3D12GraphicsCommandList* command_list);

	HRESULT update();
protected:

	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	D3D12MA::Allocation* vertex_buffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view;
	shader_pass_c sample_shader_pass;

	renderer_c* d3d_renderer = nullptr;
	resource_manager_c* resource_manager = nullptr;
	constant_buffers_manager_c* constant_buffers_manager = nullptr;

	ComPtr<ID3D12Device> d3d_device;
	ComPtr<ID3D12DescriptorHeap> srv_heap;

	bool srv_heap_not_empty = false;
};