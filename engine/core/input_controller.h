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

class mouse_keyboard_game_controller_c : public mouse_keyboard_input_controller_i
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

	void init(INPUT_CONTROLLER_CONFIG config);
	INPUT_CONTROLLER_ACTION_STATE get_action_state(ACTION_ID action);
	INPUT_CONTROLLER_MOUSE_STATE get_mouse_state();
	void subscribe(action_event_handler_c* handler, ACTION_ID action);
	void unsubscribe(action_event_handler_c* handler, ACTION_ID action);
	void subscribe(action_move_event_handler_c* handler, ACTION_ID action);
	void unsubscribe(action_move_event_handler_c* handler, ACTION_ID action);
	void every_frame_update(float elapsed_time);

	void register_action_id(MOUSE_KEYBOARD_VIRTUAL_KEY vitrtual_key, bool shift_key, bool ctrl_key, ACTION_ID action);
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
		action_move_event_c move_event;
		ACTION_ID action_id = INVALID_ACTION_ID;
		VIRTUAL_KEY_MAP vk_map[CTRL_SHIFT_STATE_NUM] = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };
		bool state = false;
		bool dbl_click = false;
		bool dbl_tap = false;
		float duration = 0.0f;
		float off_duration = 0.0f;
		uint32_t press_count = 0;

		INT16 move_pos_x = 0;
		INT16 move_pos_y = 0;
		INT16 move_speed_x = 0;
		INT16 move_speed_y = 0;
	};

	friend INPUT_CONTROLLER_ACTION_STATE action_state_init(mouse_keyboard_game_controller_c::ACTION_DESC* action);
	friend INPUT_CONTROLLER_MOUSE_STATE move_action_state_init(mouse_keyboard_game_controller_c::ACTION_DESC* action);

	void update_action_desc_on_set(ACTION_DESC* action)
	{
		if (action->off_duration > double_tap_duration_threashold) {
			action->dbl_tap = true;
		}
	}

	void update_action_desc_on_reset(ACTION_DESC* action)
	{
		action->dbl_tap = false;
	}

	enum UPDATE_ACTION_PROC
	{
		UPDATE_ACTION_SET,
		UPDATE_ACTION_RESET,
		UPDATE_ACTION_MOVE,
		UPDATE_ACTION_DBLCLK,
	};
	void update_actions(MOUSE_KEYBOARD_VIRTUAL_KEY vitrtual_key, bool shift_key, bool ctrl_key, UPDATE_ACTION_PROC proc, bool call_action = true);
	bool is_action_id_available(ACTION_ID action_id);

	std::vector<ACTION_DESC*> actions_map;
	std::map<ACTION_ID, ACTION_DESC*> actions_desc;

	float double_tap_duration_threashold = 0.0f;

	INT16 last_mouse_x = INT16_MIN;
	INT16 last_mouse_y = INT16_MIN;
	INT16 mouse_move_x = 0;
	INT16 mouse_move_y = 0;
};