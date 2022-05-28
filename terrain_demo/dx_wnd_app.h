#pragma once
#include <vector>
#include <string>
#include <d3d12.h>
#include "wnd_app.h"
#include "hachiko_api.h"

class dx_wnd_app_c : public wnd_app_c
{
public:
	dx_wnd_app_c(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show);
	~dx_wnd_app_c();

	virtual void create_pipline(D3D_FEATURE_LEVEL feature_level);
	virtual HRESULT on_render();
	virtual void on_destroy();
	virtual int render_loop();

	LRESULT on_wm_keydown(WPARAM wParam, LPARAM lParam) { return mouse_keyboard_game_controller->on_wm_keydown(wParam, lParam); }
	LRESULT on_wm_keyup(WPARAM wParam, LPARAM lParam) { return mouse_keyboard_game_controller->on_wm_keyup(wParam, lParam); }
	LRESULT on_wm_unichar(WPARAM wParam, LPARAM lParam) { return mouse_keyboard_game_controller->on_wm_unichar(wParam, lParam); }
	LRESULT on_wm_char(WPARAM wParam, LPARAM lParam) { return mouse_keyboard_game_controller->on_wm_char(wParam, lParam); }
	LRESULT on_wm_lbuttondblclk(WPARAM wParam, LPARAM lParam) { return mouse_keyboard_game_controller->on_wm_lbuttondblclk(wParam, lParam); }
	LRESULT on_wm_lbuttonddown(WPARAM wParam, LPARAM lParam) { return mouse_keyboard_game_controller->on_wm_lbuttonddown(wParam, lParam); }
	LRESULT on_wm_lbuttondup(WPARAM wParam, LPARAM lParam) { return mouse_keyboard_game_controller->on_wm_lbuttondup(wParam, lParam); }
	LRESULT on_wm_mbuttondblclk(WPARAM wParam, LPARAM lParam) { return mouse_keyboard_game_controller->on_wm_mbuttondblclk(wParam, lParam); }
	LRESULT on_wm_mbuttonddown(WPARAM wParam, LPARAM lParam) { return mouse_keyboard_game_controller->on_wm_mbuttonddown(wParam, lParam); }
	LRESULT on_wm_mbuttondup(WPARAM wParam, LPARAM lParam) { return mouse_keyboard_game_controller->on_wm_mbuttondup(wParam, lParam); }
	LRESULT on_wm_rbuttondblclk(WPARAM wParam, LPARAM lParam) { return mouse_keyboard_game_controller->on_wm_rbuttondblclk(wParam, lParam); }
	LRESULT on_wm_rbuttonddown(WPARAM wParam, LPARAM lParam) { return mouse_keyboard_game_controller->on_wm_rbuttonddown(wParam, lParam); }
	LRESULT on_wm_rbuttondup(WPARAM wParam, LPARAM lParam) { return mouse_keyboard_game_controller->on_wm_rbuttondup(wParam, lParam); }
	LRESULT on_wm_mousewheel(WPARAM wParam, LPARAM lParam) { return mouse_keyboard_game_controller->on_wm_mousewheel(wParam, lParam); }
	LRESULT on_wm_mousemove(WPARAM wParam, LPARAM lParam) { return mouse_keyboard_game_controller->on_wm_mousemove(wParam, lParam); }

private:
	dx_wnd_app_c() {};

protected:
	static void static_render(wnd_app_c* wnd);
	renderer_i* d3d_renderer = nullptr;
	mouse_keyboard_game_controller_i* mouse_keyboard_game_controller = nullptr;
};

