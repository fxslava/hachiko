#include "../core/renderer.h"
#include "../core/game_controller.h"
#include "hachiko_api.h"

void create_render_device(renderer_i*& renderer) {
	renderer = new renderer_c();
}

void destroy_render_device(renderer_i* renderer) {
	delete renderer;
}

void create_mouse_keyboard_game_controller(mouse_keyboard_game_controller_i*& game_controller) {
	game_controller = new mouse_keyboard_game_controller_c();
}

void destroy_mouse_keyboard_game_controller(mouse_keyboard_game_controller_i* game_controller) {
	delete game_controller;
}