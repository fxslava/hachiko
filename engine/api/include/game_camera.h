#pragma once
#include "hachiko_api.h"

constexpr ACTION_ID ACTION_CAMERA_BEGIN      = 0;
constexpr ACTION_ID ACTION_CAMERA_MOVE_UP    = 0;
constexpr ACTION_ID ACTION_CAMERA_MOVE_DOWN  = 1;
constexpr ACTION_ID ACTION_CAMERA_MOVE_LEFT  = 2;
constexpr ACTION_ID ACTION_CAMERA_MOVE_RIGHT = 3;
constexpr ACTION_ID ACTION_CAMERA_END        = 4;

class render_camera_c : public action_event_handler_c
{
public:
	void subscribe(game_controller_i* controller)
	{
		for (int action_id = ACTION_CAMERA_BEGIN; action_id < ACTION_CAMERA_END; action_id++)
			controller->subscribe(this, action_id);
	}

	void call(ACTION_ID action_id, CONTROLLER_ACTION_STATE action_state, bool state)
	{
		if (state) {
			switch (action_id)
			{
			case ACTION_CAMERA_MOVE_UP:
				ypos++;
				break;
			case ACTION_CAMERA_MOVE_DOWN:
				ypos--;
				break;
			case ACTION_CAMERA_MOVE_LEFT:
				xpos--;
				break;
			case ACTION_CAMERA_MOVE_RIGHT:
				xpos++;
				break;
			}
		}
	};

private:
	int xpos = 0;
	int ypos = 0;
};