#pragma once
#include "pch.h"
#include "wnd_app.h"
#include "camera.h"
#include "terrain.h"
#include "renderer.h"

#include "resource.h"

using Microsoft::WRL::ComPtr;

class demo_terrain_app_c : public wnd_app_c
{
public:
	demo_terrain_app_c(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show);

	HRESULT create_pipline(D3D_FEATURE_LEVEL feature_level);
	HRESULT update();
	HRESULT on_render();

	void on_destroy();
	int render_loop();

private:
	demo_terrain_app_c() {};
	void update_timestamp();
	virtual const HICON get_app_icon(HINSTANCE instance) const { return LoadIcon(instance, MAKEINTRESOURCE(IDI_DEMO_APP)); }
	virtual const HICON get_app_icon_sm(HINSTANCE instance) const { return LoadIcon(instance, MAKEINTRESOURCE(IDI_DEMO_APP)); }

protected:
	static void static_render(wnd_app_c* wnd);
	renderer_c*                 d3d_renderer = nullptr;
	resource_manager_c*         resource_menager = nullptr;
	constant_buffers_manager_c* constant_buffers_manager = nullptr;
	movable_camera_c            view_camera;
	terrain_base_c              terrain;
	INT64                       last_timestamp = -1;
	float                       elapsed_time = 0.0f;
};

