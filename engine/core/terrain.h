#pragma once
#include "pch.h"

#include "terrain_consts.h"
#include "engine.h"
#include "game_actors.h"
#include "shader_pass.h"
#include "terrain_render_pass.h"
#include "resource_manager.h"
#include "constant_buffers_manager.h"

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
	void init(XMINT2 dimensions_, XMFLOAT3 origin_, float tile_size_, float min_height_, float max_height_);
	void update_lods(XMFLOAT3 cam_pos);

	struct TILE_STATE_TRANSITION {
		int tile_id;
		int old_lod;
		int new_lod;
	};

	TILE_STATE_TRANSITION* pop_lod_transition();

private:
	void push_tile_state_transition(int tile_idx, int old_lod, int new_lod);

private:
	float    min_height = 0.0f;
	float    max_height = 0.0f;
	XMINT2   dimensions = { 0, 0 };
	XMFLOAT3 origin     = { 0.f, 0.f, 0.f };
	float    tile_size  = 1.0f;

	std::vector<int> tile_lod;
	std::vector<int> tile_lifetime;
	std::vector<TILE_STATE_TRANSITION> transitions_pool;
	std::queue<TILE_STATE_TRANSITION*> transitions;
};

class terrain_base_c : public terrain_i
{
public:

	HRESULT allocate_resources(const TERRAIN_DESC& desc);
	void render(ID3D12GraphicsCommandList* command_list);

	HRESULT prepare_frame(ID3D12GraphicsCommandList* command_list);
	HRESULT update(ID3D12GraphicsCommandList* command_list);

protected:
	void update_constant_buffer();

protected:
	GAME_ENGINE_INSTANCES
	LOWLEVEL_GAME_ENGINE_INSTANCES

	XMFLOAT3 terrain_origin = { 0.f, 0.f, 0.f };
	XMINT2 terrain_dimensions = { 0, 0 };
	float tile_size = 1.f;
	float min_height = 0.f;
	float max_height = 0.f;

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

	terrain_grid grid;
	terrain_render_pass_c terrain_render_pass;
	shader_pass_c sample_shader_pass;

	ComPtr<ID3D12DescriptorHeap> srv_heap;

	bool srv_heap_not_empty = false;
};