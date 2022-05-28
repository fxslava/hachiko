#pragma once
#include <windows.h>
#include <atomic>
#include <vector>
#include <map>

#include "hachiko_api.h"
#include "game_events.h"

struct VIRTUAL_KEY_MAP
{
	uint64_t low_value = 0;
	uint64_t high_value = 0;
};

class mouse_keyboard_game_controller_c : public mouse_keyboard_game_controller_i
{
public:
	mouse_keyboard_game_controller_c();

	LRESULT on_wm_keydown(WPARAM wParam, LPARAM lParam);
	LRESULT on_wm_keyup(WPARAM wParam, LPARAM lParam);
	LRESULT on_wm_unichar(WPARAM wParam, LPARAM lParam);
	LRESULT on_wm_char(WPARAM wParam, LPARAM lParam);
	LRESULT on_wm_lbuttondblclk(WPARAM wParam, LPARAM lParam);
	LRESULT on_wm_lbuttonddown(WPARAM wParam, LPARAM lParam);
	LRESULT on_wm_lbuttondup(WPARAM wParam, LPARAM lParam);
	LRESULT on_wm_mbuttondblclk(WPARAM wParam, LPARAM lParam);
	LRESULT on_wm_mbuttonddown(WPARAM wParam, LPARAM lParam);
	LRESULT on_wm_mbuttondup(WPARAM wParam, LPARAM lParam);
	LRESULT on_wm_rbuttondblclk(WPARAM wParam, LPARAM lParam);
	LRESULT on_wm_rbuttonddown(WPARAM wParam, LPARAM lParam);
	LRESULT on_wm_rbuttondup(WPARAM wParam, LPARAM lParam);
	LRESULT on_wm_mousewheel(WPARAM wParam, LPARAM lParam);
	LRESULT on_wm_mousemove(WPARAM wParam, LPARAM lParam);

	bool get_action_state(ACTION_ID action);
	CONTROLLER_MOUSE_STATE get_mouse_state();

	void register_action_id(MOUSE_KEYBOARD_VIRTUAL_KEY vitrtual_key, bool shift_key, bool ctrl_key, ACTION_ID action);
	void update();
	void subscribe(action_event_handler_c* handler, ACTION_ID action);
	void unsubscribe(action_event_handler_c* handler, ACTION_ID action);
private:
	enum CTRL_SHIFT_STATE
	{
		NOPE = 0,
		CTRL,
		SHIFT,
		CTRL_SHIFT,
		CTRL_SHIFT_STATE_NUM
	};

	struct ACTION_DESC
	{
		action_event_c event;
		ACTION_ID action_id;
		VIRTUAL_KEY_MAP vk_map[CTRL_SHIFT_STATE_NUM];
		bool state;
		bool dbl_click;
	};

	enum UPDATE_ACTION_PROC
	{
		UPDATE_ACTION_SET,
		UPDATE_ACTION_RESET,
		UPDATE_ACTION_DBLCLK,
	};
	void update_actions(MOUSE_KEYBOARD_VIRTUAL_KEY vitrtual_key, bool shift_key, bool ctrl_key, UPDATE_ACTION_PROC proc, bool call_action = true);

	std::vector<ACTION_DESC*> actions_map;
	std::map<ACTION_ID, ACTION_DESC*> actions_desc;
};