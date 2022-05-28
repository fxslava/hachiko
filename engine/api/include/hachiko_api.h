#pragma once
#include <windows.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl.h>

// <<---------------------------------- RENDERER        ---------------------------------->> 

class renderer_i {
public:
	virtual HRESULT create_pipeline(D3D_FEATURE_LEVEL feature_level, HWND wnd_handle) = 0;
	virtual HRESULT wait_for_prev_frame() = 0;
	virtual HRESULT prerecord_render() = 0;
	virtual HRESULT on_render() = 0;
	virtual void destroy_pipeline() = 0;
};

void create_render_device(renderer_i*& renderer);
void destroy_render_device(renderer_i* renderer);

// <<---------------------------------- GAME CONTROLLER ---------------------------------->>
#include "input_controller_api.h"
#include "game_events.h"

typedef int32_t ACTION_ID;
constexpr ACTION_ID INVALID_ACTION_ID = -1;

struct INPUT_CONTROLLER_ACTION_STATE
{
	BOOL      state        = false;
	BOOL      double_clk   = false;
	BOOL      double_tap   = false;
	UINT32    duration     = 0;
	UINT32    off_duration = 0;
	UINT32    press_count  = 0;
	ACTION_ID action_id    = INVALID_ACTION_ID;
};

struct INPUT_CONTROLLER_MOUSE_STATE
{
	INT32     mouse_X = -1;
	INT32     mouse_Y = -1;
	INT32     mouse_speed_X = 0;
	INT32     mouse_speed_Y = 0;
};

struct INPUT_CONTROLLER_CONFIG
{
	INT32     double_tap_duration_threashold_ms;
};

using action_event_c         = game_event_c   <ACTION_ID, INPUT_CONTROLLER_ACTION_STATE>;
using action_event_handler_c = event_handler_i<ACTION_ID, INPUT_CONTROLLER_ACTION_STATE>;

class input_controller_i
{
public:
	virtual void init(INPUT_CONTROLLER_CONFIG config) = 0;
	virtual INPUT_CONTROLLER_ACTION_STATE get_action_state(ACTION_ID action) = 0;
	virtual INPUT_CONTROLLER_MOUSE_STATE get_mouse_state() = 0;
	virtual void subscribe(action_event_handler_c* handler, ACTION_ID action) = 0;
	virtual void unsubscribe(action_event_handler_c* handler, ACTION_ID action) = 0;
	virtual void every_frame_update(float elapsed_time) = 0;
};

class mouse_keyboard_listener_i
{
public:
	virtual LRESULT on_wm_keydown(WPARAM wParam, LPARAM lParam) = 0;
	virtual LRESULT on_wm_keyup(WPARAM wParam, LPARAM lParam) = 0;
	virtual LRESULT on_wm_unichar(WPARAM wParam, LPARAM lParam) = 0;
	virtual LRESULT on_wm_char(WPARAM wParam, LPARAM lParam) = 0;
	virtual LRESULT on_wm_lbuttondblclk(WPARAM wParam, LPARAM lParam) = 0;
	virtual LRESULT on_wm_lbuttonddown(WPARAM wParam, LPARAM lParam) = 0;
	virtual LRESULT on_wm_lbuttondup(WPARAM wParam, LPARAM lParam) = 0;
	virtual LRESULT on_wm_mbuttondblclk(WPARAM wParam, LPARAM lParam) = 0;
	virtual LRESULT on_wm_mbuttonddown(WPARAM wParam, LPARAM lParam) = 0;
	virtual LRESULT on_wm_mbuttondup(WPARAM wParam, LPARAM lParam) = 0;
	virtual LRESULT on_wm_rbuttondblclk(WPARAM wParam, LPARAM lParam) = 0;
	virtual LRESULT on_wm_rbuttonddown(WPARAM wParam, LPARAM lParam) = 0;
	virtual LRESULT on_wm_rbuttondup(WPARAM wParam, LPARAM lParam) = 0;
	virtual LRESULT on_wm_mousewheel(WPARAM wParam, LPARAM lParam) = 0;
	virtual LRESULT on_wm_mousemove(WPARAM wParam, LPARAM lParam) = 0;
};

class mouse_keyboard_input_controller_i : public input_controller_i, public mouse_keyboard_listener_i
{
public:
	virtual void register_action_id(MOUSE_KEYBOARD_VIRTUAL_KEY vitrtual_key, bool shift_key, bool ctrl_key, ACTION_ID action) = 0;
};

void create_mouse_keyboard_input_controller(mouse_keyboard_input_controller_i*& input_controller);
void destroy_mouse_keyboard_input_controller(mouse_keyboard_input_controller_i* input_controller);