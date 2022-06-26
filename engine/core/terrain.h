#pragma once
#include <vector>

#include "game_actors.h"
#include "shader_pass.h"
#include "terrain_render_pass.h"
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

	HRESULT allocate_resources();
	void render(ID3D12GraphicsCommandList* command_list);

	HRESULT update(ID3D12GraphicsCommandList* command_list);
protected:

	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 uv;
	};

	struct {
		XMINT4 grid_dim = XMINT4(32, 32, 0, 0);
		XMFLOAT4 origin = XMFLOAT4(64.f, 1.f, 64.f, 0.f);
		XMFLOAT4 sizes  = XMFLOAT4(4.f, 4.f, 0.f, 100.f);
	} common_terrain_cb;

	CONSTANT_BUFFER_HANDLE common_terrain_cb_handle = CONSTANT_BUFFER_INVALID_HANDLE;

	terrain_render_pass_c terrain_render_pass;
	shader_pass_c sample_shader_pass;

	renderer_c* d3d_renderer = nullptr;
	resource_manager_c* resource_manager = nullptr;
	constant_buffers_manager_c* constant_buffers_manager = nullptr;

	ComPtr<ID3D12Device> d3d_device;
	ComPtr<ID3D12DescriptorHeap> srv_heap;

	bool srv_heap_not_empty = false;
};