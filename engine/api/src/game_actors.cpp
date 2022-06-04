#include "game_actors.h"
#include "terrain.h"

void create_terrain_actor(terrain_i*& actor)
{
	actor = new terrain_base_c();
}

void destroy_terrain_actor(terrain_i* actor)
{
	delete actor;
}