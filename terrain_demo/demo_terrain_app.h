#pragma once
#include <vector>
#include <string>
#include <d3d12.h>
#include "wnd_app.h"
#include "hachiko_api.h"
#include "camera.h"

#include "resource.h"

class dx_wnd_app_c : public wnd_app_c
{
public:
	dx_wnd_app_c(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show);
	~dx_wnd_app_c();

	void create_pipline(D3D_FEATURE_LEVEL feature_level);
	HRESULT on_render();
	void on_destroy();
	int render_loop();

private:
	dx_wnd_app_c() {};
	virtual const HICON  get_app_icon(HINSTANCE instance) const { return LoadIcon(instance, MAKEINTRESOURCE(IDI_DEMO_APP)); }
	virtual const HICON  get_app_icon_sm(HINSTANCE instance) const { return LoadIcon(instance, MAKEINTRESOURCE(IDI_DEMO_APP)); }

protected:
	static void static_render(wnd_app_c* wnd);
	renderer_i* d3d_renderer = nullptr;
	movable_camera_c view_camera;
};

