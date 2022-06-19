#include "input_controller.h"
#include "virtual_keys.hpp"
#include <Windowsx.h>

#define GET_SHIFT_STATE (GetAsyncKeyState(VK_LSHIFT) || GetAsyncKeyState(VK_RSHIFT))
#define GET_CTRL_STATE (GetAsyncKeyState(VK_LCONTROL) || GetAsyncKeyState(VK_RCONTROL))
#define GET_ALT_STATE (GetAsyncKeyState(VK_LMENU) || GetAsyncKeyState(VK_RMENU))

WORD convert_to_wnd_virtual_key(MOUSE_KEYBOARD_VIRTUAL_KEY key)
{
	return virtual_key_to_wnd_vk[key];
}

bool isValidVirtualKey(WORD vkey)
{
	return wnd_vk_to_virtual_key.find(vkey) != wnd_vk_to_virtual_key.end();
}

LRESULT mouse_keyboard_game_controller_c::on_wm_keydown(WPARAM wParam, LPARAM lParam) {
	const WORD vkey = LOWORD(wParam);
	if (isValidVirtualKey(vkey)) {
		const MOUSE_KEYBOARD_VIRTUAL_KEY virtual_key = wnd_vk_to_virtual_key[vkey];
		update_actions(virtual_key, GET_SHIFT_STATE, GET_CTRL_STATE, UPDATE_ACTION_SET);
	}
	return 0;
}

LRESULT mouse_keyboard_game_controller_c::on_wm_keyup(WPARAM wParam, LPARAM lParam) {
	const WORD vkey = LOWORD(wParam);

	if (isValidVirtualKey(vkey)) {
		const MOUSE_KEYBOARD_VIRTUAL_KEY virtual_key = wnd_vk_to_virtual_key[vkey];
		update_actions(virtual_key, GET_SHIFT_STATE, GET_CTRL_STATE, UPDATE_ACTION_RESET);
	}
	return 0;
}

LRESULT mouse_keyboard_game_controller_c::on_wm_unichar(WPARAM wParam, LPARAM lParam) {
	// no handle
	return 0;
}

LRESULT mouse_keyboard_game_controller_c::on_wm_char(WPARAM wParam, LPARAM lParam) {
	// no handle
	return 0;
}

LRESULT mouse_keyboard_game_controller_c::on_wm_lbuttondblclk(WPARAM wParam, LPARAM lParam) {
	update_actions(VIRTUAL_KEY_LBUTTON, GET_SHIFT_STATE, GET_CTRL_STATE, UPDATE_ACTION_DBLCLK);
	return 0;
}

LRESULT mouse_keyboard_game_controller_c::on_wm_lbuttonddown(WPARAM wParam, LPARAM lParam) {
	update_actions(VIRTUAL_KEY_LBUTTON, GET_SHIFT_STATE, GET_CTRL_STATE, UPDATE_ACTION_SET);
	return 0;
}

LRESULT mouse_keyboard_game_controller_c::on_wm_lbuttondup(WPARAM wParam, LPARAM lParam) {
	update_actions(VIRTUAL_KEY_LBUTTON, GET_SHIFT_STATE, GET_CTRL_STATE, UPDATE_ACTION_RESET);
	return 0;
}

LRESULT mouse_keyboard_game_controller_c::on_wm_mbuttondblclk(WPARAM wParam, LPARAM lParam) {
	update_actions(VIRTUAL_KEY_MBUTTON, GET_SHIFT_STATE, GET_CTRL_STATE, UPDATE_ACTION_DBLCLK);
	return 0;
}

LRESULT mouse_keyboard_game_controller_c::on_wm_mbuttonddown(WPARAM wParam, LPARAM lParam) {
	update_actions(VIRTUAL_KEY_MBUTTON, GET_SHIFT_STATE, GET_CTRL_STATE, UPDATE_ACTION_SET);
	return 0;
}

LRESULT mouse_keyboard_game_controller_c::on_wm_mbuttondup(WPARAM wParam, LPARAM lParam) {
	update_actions(VIRTUAL_KEY_MBUTTON, GET_SHIFT_STATE, GET_CTRL_STATE, UPDATE_ACTION_RESET);
	return 0;
}

LRESULT mouse_keyboard_game_controller_c::on_wm_rbuttondblclk(WPARAM wParam, LPARAM lParam) {
	update_actions(VIRTUAL_KEY_RBUTTON, GET_SHIFT_STATE, GET_CTRL_STATE, UPDATE_ACTION_DBLCLK);
	return 0;
}

LRESULT mouse_keyboard_game_controller_c::on_wm_rbuttonddown(WPARAM wParam, LPARAM lParam) {
	update_actions(VIRTUAL_KEY_RBUTTON, GET_SHIFT_STATE, GET_CTRL_STATE, UPDATE_ACTION_SET);
	return 0;
}

LRESULT mouse_keyboard_game_controller_c::on_wm_rbuttondup(WPARAM wParam, LPARAM lParam) {
	update_actions(VIRTUAL_KEY_RBUTTON, GET_SHIFT_STATE, GET_CTRL_STATE, UPDATE_ACTION_RESET);
	return 0;
}

LRESULT mouse_keyboard_game_controller_c::on_wm_mousewheel(WPARAM wParam, LPARAM lParam) {
	return 0;
}

LRESULT mouse_keyboard_game_controller_c::on_wm_mousemove(WPARAM wParam, LPARAM lParam) {
	const INT16 xPos = GET_X_LPARAM(lParam);
	const INT16 yPos = GET_Y_LPARAM(lParam);

	if (last_mouse_x != INT16_MIN || last_mouse_y != INT16_MIN)	{
		mouse_move_x = xPos - last_mouse_x;
		mouse_move_y = yPos - last_mouse_y;
	}
	last_mouse_x = xPos;
	last_mouse_y = yPos;

	for (auto& action : mouse_actions) {
		switch (action->mouse_mask) {
		case MOUSE_MOVE_MASK_MOUSE_X:
			action->ctrl_pos = last_mouse_x;
			action->ctrl_vel = mouse_move_x;
			if (action->state || action->activate_on_move) action->event(action->action_id, action_state_init(action));
			break;
		case MOUSE_MOVE_MASK_MOUSE_Y:
			action->ctrl_pos = last_mouse_y;
			action->ctrl_vel = mouse_move_y;
			if (action->state || action->activate_on_move) action->event(action->action_id, action_state_init(action));
			break;
		}
	}

	return 0;
}

void mouse_keyboard_game_controller_c::init(INPUT_CONTROLLER_CONFIG config) {
	double_tap_duration_threashold = static_cast<float>(config.double_tap_duration_threashold_ms) * 0.001f;
}

INPUT_CONTROLLER_ACTION_STATE action_state_init(mouse_keyboard_game_controller_c::ACTION_DESC* action)
{
	INPUT_CONTROLLER_ACTION_STATE state;
	state.action_id = action->action_id;
	state.state = action->state;
	state.double_clk = action->dbl_click;
	state.double_tap = action->dbl_tap;
	state.duration = static_cast<int>(action->duration / 0.001f);
	state.off_duration = static_cast<int>(action->off_duration / 0.001f);
	state.press_count = action->press_count;
	state.control_pos = action->ctrl_pos;
	state.control_vel = action->ctrl_vel;
	return state;
}


INPUT_CONTROLLER_ACTION_STATE mouse_keyboard_game_controller_c::get_action_state(ACTION_ID action) {
	INPUT_CONTROLLER_ACTION_STATE state;
	if (is_action_id_available(action))
		state = action_state_init(actions_desc[action]);
	return state;
}

void mouse_keyboard_game_controller_c::associate_with_mouse(ACTION_ID action, INPUT_CONTROLLER_MOUSE_MOVE_MASK mouse_mask, bool activate_on_move) {
	if (is_action_id_available(action)) {
		actions_desc[action]->mouse_mask = mouse_mask;
		actions_desc[action]->activate_on_move = activate_on_move;
		mouse_actions.push_back(actions_desc[action]);
	}
}

mouse_keyboard_game_controller_c::~mouse_keyboard_game_controller_c()
{
	for (const auto& action : actions_map) {
		delete action;
	}
}

bool mouse_keyboard_game_controller_c::is_action_id_available(ACTION_ID action_id)
{
	return actions_desc.find(action_id) != actions_desc.end();
}

void mouse_keyboard_game_controller_c::register_action_id(const MOUSE_KEYBOARD_VIRTUAL_KEY vitrtual_key, const bool shift_key, const bool ctrl_key, const ACTION_ID action) {

	const bool is_available = is_action_id_available(action);
	auto* action_desc = is_available ? actions_desc[action] : new ACTION_DESC;
	if (!is_available) {
		actions_desc[action] = action_desc;
		actions_map.push_back(action_desc);
	}

	const VIRTUAL_KEY_MAP mask = virtual_key_bitmask[vitrtual_key];
	const auto state = static_cast<CTRL_SHIFT_STATE>((ctrl_key ? 1 : 0) + (shift_key ? 2 : 0));
	action_desc->vk_map[state].low_value  |= mask.low_value;
	action_desc->vk_map[state].high_value |= mask.high_value;
	action_desc->action_id = action;
}

void mouse_keyboard_game_controller_c::update_actions(MOUSE_KEYBOARD_VIRTUAL_KEY vitrtual_key, bool shift_key, bool ctrl_key, UPDATE_ACTION_PROC proc, bool call_action) {
	const VIRTUAL_KEY_MAP mask = virtual_key_bitmask[vitrtual_key];
	const auto state = static_cast<CTRL_SHIFT_STATE>((ctrl_key ? 1 : 0) + (shift_key ? 2 : 0));
	for (auto& action : actions_map) {
		if (((action->vk_map[state].low_value  & mask.low_value)  != 0) ||
			((action->vk_map[state].high_value & mask.high_value) != 0)) {
			switch (proc) {
			case UPDATE_ACTION_SET:   action->state = true; action->press_count++; break;
			case UPDATE_ACTION_RESET: action->state = false; break;
			case UPDATE_ACTION_DBLCLK:action->dbl_click = true; break;
			}
			
			if (call_action) {
				if (action->state)
					update_action_desc_on_set(action);
				else
					update_action_desc_on_reset(action);

				action->event(action->action_id, action_state_init(action));
			}
		}
	}
}

void mouse_keyboard_game_controller_c::every_frame_update(float elapsed_time) {
	for (const auto& action : actions_map) {
		if (action->state) {
			update_action_desc_on_set(action);
			action->duration += elapsed_time;
			action->off_duration = 0.0f;
		} else {
			update_action_desc_on_reset(action);
			action->duration += 0.0f;
			action->off_duration += elapsed_time;
		}
	}
}

void mouse_keyboard_game_controller_c::subscribe(action_event_handler_c* handler, ACTION_ID action) {
	auto* desc = actions_desc[action];
	if (desc) {
		desc->event += handler;
	}
}

void mouse_keyboard_game_controller_c::unsubscribe(action_event_handler_c* handler, ACTION_ID action) {
	auto* desc = actions_desc[action];
	if (desc) {
		desc->event -= handler;
	}
}
