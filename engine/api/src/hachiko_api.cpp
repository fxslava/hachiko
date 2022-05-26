#include "../core/hch_renderer.h"
#include "hachiko_api.h"

void create_render_device(hch_render_device_i*& renderer) {
	renderer = new hch_render_device_c();
}

void destroy_render_device(hch_render_device_i* renderer) {
	delete renderer;
}