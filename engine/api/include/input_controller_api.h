#pragma once
#include <windows.h>
#include "game_events.h"

enum MOUSE_KEYBOARD_VIRTUAL_KEY {
	VIRTUAL_KEY_LBUTTON = 0,
	VIRTUAL_KEY_RBUTTON,
	VIRTUAL_KEY_CANCEL,
	VIRTUAL_KEY_MBUTTON,
	VIRTUAL_KEY_XBUTTON1,
	VIRTUAL_KEY_XBUTTON2,
	VIRTUAL_KEY_BACK,
	VIRTUAL_KEY_TAB,
	VIRTUAL_KEY_CLEAR,
	VIRTUAL_KEY_RETURN,
	VIRTUAL_KEY_SHIFT,
	VIRTUAL_KEY_CONTROL,
	VIRTUAL_KEY_MENU,
	VIRTUAL_KEY_PAUSE,
	VIRTUAL_KEY_CAPITAL,
	VIRTUAL_KEY_HANGUL,
	VIRTUAL_KEY_IME_ON,
	VIRTUAL_KEY_JUNJA,
	VIRTUAL_KEY_FINAL,
	VIRTUAL_KEY_HANJA,
	VIRTUAL_KEY_KANJI,
	VIRTUAL_KEY_IME_OFF,
	VIRTUAL_KEY_ESCAPE,
	VIRTUAL_KEY_CONVERT,
	VIRTUAL_KEY_NONCONVERT,
	VIRTUAL_KEY_ACCEPT,
	VIRTUAL_KEY_MODECHANGE,
	VIRTUAL_KEY_SPACE,
	VIRTUAL_KEY_PRIOR,
	VIRTUAL_KEY_NEXT,
	VIRTUAL_KEY_END,
	VIRTUAL_KEY_HOME,
	VIRTUAL_KEY_LEFT,
	VIRTUAL_KEY_UP,
	VIRTUAL_KEY_RIGHT,
	VIRTUAL_KEY_DOWN,
	VIRTUAL_KEY_SELECT,
	VIRTUAL_KEY_PRINT,
	VIRTUAL_KEY_EXECUTE,
	VIRTUAL_KEY_SNAPSHOT,
	VIRTUAL_KEY_INSERT,
	VIRTUAL_KEY_DELETE,
	VIRTUAL_KEY_HELP,
	VIRTUAL_KEY_0,
	VIRTUAL_KEY_1,
	VIRTUAL_KEY_2,
	VIRTUAL_KEY_3,
	VIRTUAL_KEY_4,
	VIRTUAL_KEY_5,
	VIRTUAL_KEY_6,
	VIRTUAL_KEY_7,
	VIRTUAL_KEY_8,
	VIRTUAL_KEY_9,
	VIRTUAL_KEY_A,
	VIRTUAL_KEY_B,
	VIRTUAL_KEY_C,
	VIRTUAL_KEY_D,
	VIRTUAL_KEY_E,
	VIRTUAL_KEY_F,
	VIRTUAL_KEY_G,
	VIRTUAL_KEY_H,
	VIRTUAL_KEY_I,
	VIRTUAL_KEY_J,
	VIRTUAL_KEY_K,
	VIRTUAL_KEY_L,
	VIRTUAL_KEY_M,
	VIRTUAL_KEY_N,
	VIRTUAL_KEY_O,
	VIRTUAL_KEY_P,
	VIRTUAL_KEY_Q,
	VIRTUAL_KEY_R,
	VIRTUAL_KEY_S,
	VIRTUAL_KEY_T,
	VIRTUAL_KEY_U,
	VIRTUAL_KEY_V,
	VIRTUAL_KEY_W,
	VIRTUAL_KEY_X,
	VIRTUAL_KEY_Y,
	VIRTUAL_KEY_Z,
	VIRTUAL_KEY_NUMPAD0,
	VIRTUAL_KEY_NUMPAD1,
	VIRTUAL_KEY_NUMPAD2,
	VIRTUAL_KEY_NUMPAD3,
	VIRTUAL_KEY_NUMPAD4,
	VIRTUAL_KEY_NUMPAD5,
	VIRTUAL_KEY_NUMPAD6,
	VIRTUAL_KEY_NUMPAD7,
	VIRTUAL_KEY_NUMPAD8,
	VIRTUAL_KEY_NUMPAD9,
	VIRTUAL_KEY_MULTIPLY,
	VIRTUAL_KEY_ADD,
	VIRTUAL_KEY_SEPARATOR,
	VIRTUAL_KEY_SUBTRACT,
	VIRTUAL_KEY_DECIMAL,
	VIRTUAL_KEY_DIVIDE,
	VIRTUAL_KEY_F1,
	VIRTUAL_KEY_F2,
	VIRTUAL_KEY_F3,
	VIRTUAL_KEY_F4,
	VIRTUAL_KEY_F5,
	VIRTUAL_KEY_F6,
	VIRTUAL_KEY_F7,
	VIRTUAL_KEY_F8,
	VIRTUAL_KEY_F9,
	VIRTUAL_KEY_F10,
	VIRTUAL_KEY_F11,
	VIRTUAL_KEY_F12,
	VIRTUAL_KEY_NUM
};

// <<---------------------------------- INPUT CONTROLLER ---------------------------------->>

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