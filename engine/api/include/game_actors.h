#pragma once
#include "hachiko_api.h"

class terrain_i
{
public:
	virtual HRESULT allocate_resources(renderer_i* renderer) = 0;
	virtual void render(ID3D12GraphicsCommandList* command_list) = 0;
};

void create_terrain_actor(terrain_i*& actor);
void destroy_terrain_actor(terrain_i* actor);