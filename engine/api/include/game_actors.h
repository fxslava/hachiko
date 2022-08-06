#pragma once
#include "hachiko_api.h"

struct TERRAIN_DESC {
	int dimension_x;
	int dimension_y;
	float origin_x;
	float origin_y;
	float origin_z;
	float tile_size;
	float min_height;
	float max_height;
};

class terrain_i
{
public:
	virtual ~terrain_i() {};
	virtual HRESULT allocate_resources(const TERRAIN_DESC& desc) = 0;
	virtual HRESULT prepare_frame(ID3D12GraphicsCommandList* command_list) = 0;
	virtual HRESULT update(ID3D12GraphicsCommandList* command_list) = 0;
	virtual void render(ID3D12GraphicsCommandList* command_list) = 0;
};

void create_terrain_actor(terrain_i*& actor);
void destroy_terrain_actor(terrain_i* actor);
