#include "../core/renderer.h"
#include "hachiko_api.h"

void create_render_device(renderer_i*& renderer) {
	renderer = new renderer_c();
}

void destroy_render_device(renderer_i* renderer) {
	delete renderer;
}