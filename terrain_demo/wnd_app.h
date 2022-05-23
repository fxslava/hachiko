#pragma once
#include <windows.h>
#include <stdexcept>
#include <string>

#include "resource.h"

class wnd_app_c
{
public:
	wnd_app_c(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show);

public:
	HWND get_hwnd() { return wnd_handle; }
	typedef void (*render_callback_t)(wnd_app_c* wnd);
	static int loop(wnd_app_c* ctx, render_callback_t render_cb);
	virtual LRESULT on_paint();
	virtual void on_destroy();

private:
	static LRESULT CALLBACK wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static const WCHAR* get_class_name() { return L"Hachico Application"; }
	static const WCHAR* get_title_name() { return L"Hachico game engine application"; }

protected:
	wnd_app_c() {};
	virtual void create_window(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show);

	HINSTANCE instance = 0;
	HWND wnd_handle = 0;
	int mouse_x = 0;
	int mouse_y = 0;
	int mouse_wheel = 0;
};

