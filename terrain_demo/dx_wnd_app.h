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
private:
	dx_wnd_app_c() {};

protected:
	static void static_render(wnd_app_c* wnd);
	hch_render_device_i* d3d_render_device = nullptr;
};

