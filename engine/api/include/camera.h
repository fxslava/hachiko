#pragma once
#include "hachiko_api.h"
#include "DirectXMath.h"

constexpr ACTION_ID ACTION_CAMERA_BEGIN         = 0;
constexpr ACTION_ID ACTION_CAMERA_MOVE_UP       = 0;
constexpr ACTION_ID ACTION_CAMERA_MOVE_DOWN     = 1;
constexpr ACTION_ID ACTION_CAMERA_MOVE_FORWARD  = 2;
constexpr ACTION_ID ACTION_CAMERA_MOVE_BACKWARD = 3;
constexpr ACTION_ID ACTION_CAMERA_MOVE_LEFT     = 4;
constexpr ACTION_ID ACTION_CAMERA_MOVE_RIGHT    = 5;
constexpr ACTION_ID ACTION_CAMERA_ROTATE_UP     = 6;
constexpr ACTION_ID ACTION_CAMERA_ROTATE_RIGHT  = 7;
constexpr ACTION_ID ACTION_CAMERA_END           = 8;

using namespace DirectX;

class camera_c
{
public:
	camera_c(FXMVECTOR position_, FXMVECTOR look_, FXMVECTOR up_, const float fov_angleY_, const float aspect_ratio_, const float nearZ_, const float farZ_);

	FXMMATRIX get_view() const;
	FXMMATRIX get_proj() const;
	void set_position(FXMVECTOR pos, FXMVECTOR look_at);

protected:
	XMFLOAT3A position;
	XMFLOAT3A look;
	XMFLOAT3A up;
	float     fov_angleY;
	float     aspect_ratio;
	float     nearZ;
	float     farZ;
};


class render_camera_c : public camera_c
{
public:
	render_camera_c(FXMVECTOR position_, FXMVECTOR look_, FXMVECTOR up_, const float fov_angleY_, const float aspect_ratio_, const float nearZ_, const float farZ_);
	void apply_camera_view() const;
};


class movable_camera_c : public render_camera_c
{
public:
	movable_camera_c();
	movable_camera_c(FXMVECTOR position_, FXMVECTOR look_, FXMVECTOR up_, const float fov_angleY_, const float aspect_ratio_, const float nearZ_, const float farZ_, const float velocity_);
	~movable_camera_c() = default;

	void motion_config(float velocity_);
	void subscribe(input_controller_i* controller);
	void update(float elapsed_time);

private:
	class camera_input_event_handler_c : public action_event_handler_c
	{
	public:
		camera_input_event_handler_c(movable_camera_c& camera) : camera(camera) {};
		void call(ACTION_ID action_id, INPUT_CONTROLLER_ACTION_STATE action_state);
	private:
		movable_camera_c& camera;
	};

protected:
	void XM_CALLCONV calculate_move_vectors();
	void make_move_dir();

private:
	camera_input_event_handler_c camera_input_event_listener;

	float azimuth  = 0.f;
	float polar    = XM_PIDIV2;
	float velocity = 0.0f;
	float mouse_speed = XM_2PI / 1000.f;

	XMFLOAT3A move_up{ 0.0f, 1.0f, 0.0f };
	XMFLOAT3A move_right{ 1.0f, 0.0f, 0.0f };
	XMFLOAT3A move_forward{ 0.0f, 0.0f, 1.0f };

	bool state_up = false;
	bool state_down = false;
	bool state_left = false;
	bool state_right = false;
	bool state_forward = false;
	bool state_backward = false;
};