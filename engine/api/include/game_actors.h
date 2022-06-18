#pragma once
#include "hachiko_api.h"

class terrain_i
{
public:
	virtual ~terrain_i() {};
	virtual HRESULT allocate_resources() = 0;
	virtual HRESULT update() = 0;
	virtual void render(ID3D12GraphicsCommandList* command_list) = 0;
};

void create_terrain_actor(terrain_i*& actor);
void destroy_terrain_actor(terrain_i* actor);
