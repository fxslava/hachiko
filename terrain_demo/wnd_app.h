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

	virtual LRESULT on_wm_keydown(WPARAM wParam, LPARAM lParam) { return 0; };		 // WM_KEYDOWN
	virtual LRESULT on_wm_keyup(WPARAM wParam, LPARAM lParam) { return 0; };		 // WM_KEYUP
	virtual LRESULT on_wm_unichar(WPARAM wParam, LPARAM lParam) { return 0; };		 // WM_UNICHAR
	virtual LRESULT on_wm_char(WPARAM wParam, LPARAM lParam) { return 0; };			 // WM_CHAR
	virtual LRESULT on_wm_lbuttondblclk(WPARAM wParam, LPARAM lParam) { return 0; }; // WM_LBUTTONDBLCLK
	virtual LRESULT on_wm_lbuttonddown(WPARAM wParam, LPARAM lParam) { return 0; };  // WM_LBUTTONDOWN
	virtual LRESULT on_wm_lbuttondup(WPARAM wParam, LPARAM lParam) { return 0; };    // WM_LBUTTONUP
	virtual LRESULT on_wm_mbuttondblclk(WPARAM wParam, LPARAM lParam) { return 0; }; // WM_MBUTTONDBLCLK
	virtual LRESULT on_wm_mbuttonddown(WPARAM wParam, LPARAM lParam) { return 0; };  // WM_MBUTTONDOWN
	virtual LRESULT on_wm_mbuttondup(WPARAM wParam, LPARAM lParam) { return 0; };    // WM_MBUTTONUP
	virtual LRESULT on_wm_rbuttondblclk(WPARAM wParam, LPARAM lParam) { return 0; }; // WM_RBUTTONDBLCLK
	virtual LRESULT on_wm_rbuttonddown(WPARAM wParam, LPARAM lParam) { return 0; };  // WM_RBUTTONDOWN
	virtual LRESULT on_wm_rbuttondup(WPARAM wParam, LPARAM lParam) { return 0; };    // WM_RBUTTONUP
	virtual LRESULT on_wm_mousewheel(WPARAM wParam, LPARAM lParam) { return 0; };    // WM_MOUSEWHEEL
	virtual LRESULT on_wm_mousemove(WPARAM wParam, LPARAM lParam) { return 0; };     // WM_MOUSEMOVE

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

