#pragma once
#include <vector>
#include <list>

#include "terrain_consts.h"
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

struct COMMON_TERRAIN_CB
{
	XMINT4 grid_dim = XMINT4(8, 8, 0, 0);
	XMFLOAT4 origin = XMFLOAT4(160.f, 1.f, 160.f, 0.f);
	XMFLOAT4 sizes  = XMFLOAT4(40.f, 40.f, 0.f, 100.f);
};


struct COMMON_TERRAIN_VIEW_CB
{
	XMINT4 packed_texture_srv_indices;
};

class terrain_grid
{
public:
	terrain_grid(XMINT2 dimensions_, XMFLOAT3 origin_, XMFLOAT2 tile_size_, float min_height_, float max_height_);

	void update_lods(XMFLOAT3 cam_pos)
	{
		XMVECTOR local_terrain_cam_pos = XMLoadFloat3(&cam_pos) - XMLoadFloat3(&origin);

		for (int row = 0; row < dimensions.x; row++) {
			for (int col = 0; col < dimensions.y; col++) {

				const XMVECTOR local_terrain_tile_pos = XMVectorSet(
					(static_cast<float>(row) + 0.5f) * tile_size.x, 0.0f,
					(static_cast<float>(col) + 0.5f) * tile_size.y, 0.0f
				);

				const float distance_tile_to_camera = XMVector3Length(local_terrain_tile_pos - local_terrain_cam_pos).m128_f32[0];

				if (distance_tile_to_camera > TERRAIN_TILE_MAX_DISTANCE) {
					auto& lifetime = tile_lifetime[row + col * dimensions.x];
					if (++lifetime > TERRAIN_TILE_MAX_LIFETIME) {
						lifetime = TERRAIN_TILE_MAX_LIFETIME;
					}
				}


			}
		}
	}

private:
	void fill_cb()
	{
		common_terrain_cb.grid_dim = XMINT4(dimensions.x, dimensions.y, 0, 0);
		common_terrain_cb.origin = XMFLOAT4(origin.x, origin.y, origin.z, 0.f);
		common_terrain_cb.sizes = XMFLOAT4(tile_size.x, tile_size.y, min_height, max_height);
	}

private:
	float min_height;
	float max_height;
	XMINT2 dimensions;
	XMFLOAT3 origin;
	XMFLOAT2 tile_size;
	COMMON_TERRAIN_CB common_terrain_cb;
	XMFLOAT3 last_view_position = {0.f,0.f,0.f};

	std::vector<int> tile_lod;
	std::vector<int> tile_lifetime;

	resource_manager_c* resource_manager = nullptr;
};

class terrain_base_c : public terrain_i
{
public:

	HRESULT allocate_resources();
	void render(ID3D12GraphicsCommandList* command_list);

	HRESULT prepare_frame(ID3D12GraphicsCommandList* command_list);
	HRESULT update(ID3D12GraphicsCommandList* command_list);
protected:

	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 uv;
	};

	struct {
		XMINT4 grid_dim = XMINT4(8, 8, 0, 0);
		XMFLOAT4 origin = XMFLOAT4(160.f, 10.f, 160.f, 0.f);
		XMFLOAT4 sizes  = XMFLOAT4(40.f, 40.f, 0.f, 100.f);
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