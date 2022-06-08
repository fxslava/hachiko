#pragma once
#include <vector>
#include "game_actors.h"
#include "shader_pass.h"
#include "resource_manager.h"
#include "D3D12MemAlloc.h"
#include "DirectXMath.h"
#include "DirectXCollision.h"
#include "D3D12MemAlloc.h"

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
	HRESULT allocate_resources(renderer_i* renderer);
	void render(ID3D12GraphicsCommandList* command_list);
protected:

	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	D3D12MA::Allocation* vertex_buffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view;
	shader_pass_c sample_shader_pass;
	resource_manager_c resource_manager;
};