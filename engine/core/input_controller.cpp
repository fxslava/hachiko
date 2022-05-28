#include "input_controller.h"
#include "virtual_keys.hpp"

#define GET_SHIFT_STATE (GetAsyncKeyState(VK_LSHIFT) || GetAsyncKeyState(VK_RSHIFT))
#define GET_CTRL_STATE (GetAsyncKeyState(VK_LCONTROL) || GetAsyncKeyState(VK_RCONTROL))
#define GET_ALT_STATE (GetAsyncKeyState(VK_LMENU) || GetAsyncKeyState(VK_RMENU))

uint8_t convert_to_wnd_virtual_key(MOUSE_KEYBOARD_VIRTUAL_KEY key)
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
	return 0;
}

bool mouse_keyboard_game_controller_c::get_action_state(ACTION_ID action) {
	return actions_desc[action]->state;
}

CONTROLLER_MOUSE_STATE mouse_keyboard_game_controller_c::get_mouse_state() {
	CONTROLLER_MOUSE_STATE state;
	return state;
}

mouse_keyboard_game_controller_c::mouse_keyboard_game_controller_c()
{
	for (auto& action : actions_map) {
		for (int i = 0; i < CTRL_SHIFT_STATE_NUM; i++) {
			action->vk_map[i].low_value = 0;
			action->vk_map[i].high_value = 0;
		}
		action->state = false;
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
			case UPDATE_ACTION_SET:   action->state = true;  break;
			case UPDATE_ACTION_RESET: action->state = false; break;
			case UPDATE_ACTION_DBLCLK:action->dbl_click = true; break;
			}
			if (call_action) {
				CONTROLLER_ACTION_STATE state;
				action->event(action->action_id, state, action->state);
			}
		}
	}
}

void mouse_keyboard_game_controller_c::update() {
}

void mouse_keyboard_game_controller_c::subscribe(action_event_handler_c* handler, ACTION_ID action) {
	auto* desc = actions_desc[action];
	desc->event += handler;
}

void mouse_keyboard_game_controller_c::unsubscribe(action_event_handler_c* handler, ACTION_ID action) {
	auto* desc = actions_desc[action];
	desc->event -= handler;
}
