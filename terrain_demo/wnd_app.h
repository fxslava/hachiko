#pragma once
#include <windows.h>
#include <stdexcept>
#include <string>

#include "resource.h"

class wnd_app_c
{
public:
	wnd_app_c() = delete;
	wnd_app_c(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show);

public:
	typedef void (*render_callback_t)(wnd_app_c* wnd);
	static int loop(wnd_app_c* ctx, render_callback_t render_cb);

protected:
	virtual void create_window(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show);

private:
	static LRESULT CALLBACK wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static const WCHAR* get_class_name() { return L"Hachico Application"; }
	static const WCHAR* get_title_name() { return L"Hachico game engine application"; }

	// wnd callbacks
	virtual LRESULT on_paint();

protected:
	HINSTANCE instance;
	HWND wnd_handle;
};

