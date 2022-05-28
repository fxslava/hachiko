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
constexpr ACTION_ID ACTION_CAMERA_END           = 6;

using namespace DirectX;

class camera_c
{
public:
	camera_c(FXMVECTOR position_, FXMVECTOR look_, FXMVECTOR up_, const float fov_angleY_, const float aspect_ratio_, const float nearZ_, const float farZ_) :
		fov_angleY(fov_angleY_), aspect_ratio(aspect_ratio_), nearZ(nearZ_), farZ(farZ_)
	{
		XMStoreFloat3(&position, position_);
		XMStoreFloat3(&look, look_);
		XMStoreFloat3(&up, up_);
	}

	FXMMATRIX get_view() const {
		const XMVECTOR vposition = XMLoadFloat3(&position);
		const XMVECTOR vlook = XMLoadFloat3(&look);
		const XMVECTOR vup = XMLoadFloat3(&up);
		return XMMatrixLookToLH(vposition, vlook, vup);
	}

	FXMMATRIX get_perspective() const {
		return XMMatrixPerspectiveFovLH(fov_angleY, aspect_ratio, nearZ, farZ);
	}

protected:
	XMFLOAT3A position;
	XMFLOAT3A look;
	XMFLOAT3A up;
	float     fov_angleY;
	float     aspect_ratio;
	float     nearZ;
	float     farZ;
};

class movable_camera_c : public action_event_handler_c, public camera_c
{
public:
	movable_camera_c() :
		camera_c(
			XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
			XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), 
			XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), 
			XM_PIDIV2, 1920.0f / 1080.0f, 1.0f, 1000.0f),
		velocity(1.0f)
	{
		calculate_move_vectors();
	}
	movable_camera_c(FXMVECTOR position_, FXMVECTOR look_, FXMVECTOR up_, const float fov_angleY_, const float aspect_ratio_, const float nearZ_, const float farZ_, const float velocity_) :
		camera_c(position_, look_, up_, fov_angleY_, aspect_ratio_, nearZ_, farZ_),
		velocity(velocity_)
	{
		calculate_move_vectors();
	}

	~movable_camera_c()
	{
		
	}

	void motion_config(float velocity_)
	{
		velocity = velocity_;
	}

	void subscribe(input_controller_i* controller)
	{
		for (ACTION_ID action_id = ACTION_CAMERA_BEGIN; action_id < ACTION_CAMERA_END; action_id++)
			controller->subscribe(this, action_id);
	}

	void call(ACTION_ID action_id, INPUT_CONTROLLER_ACTION_STATE action_state)
	{
		switch (action_id)
		{
		case ACTION_CAMERA_MOVE_UP:	      state_up       = action_state.state; break;
		case ACTION_CAMERA_MOVE_DOWN:     state_down     = action_state.state; break;
		case ACTION_CAMERA_MOVE_LEFT:     state_left     = action_state.state; break;
		case ACTION_CAMERA_MOVE_RIGHT:    state_right    = action_state.state; break;
		case ACTION_CAMERA_MOVE_FORWARD:  state_forward  = action_state.state; break;
		case ACTION_CAMERA_MOVE_BACKWARD: state_backward = action_state.state; break;
		}
	};

	void update(float elapsed_time)
	{
		const auto up = XMLoadFloat3A(&move_up);
		const auto right = XMLoadFloat3A(&move_right);
		const auto forward = XMLoadFloat3A(&move_forward);
		const auto down = XMVectorScale(up, -1.0f);
		const auto left = XMVectorScale(right, -1.0f);
		const auto backward = XMVectorScale(forward, -1.0f);

		auto vel = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		if (state_up)       vel += up;
		if (state_down)     vel += down;
		if (state_left)     vel += left;
		if (state_right)    vel += right;
		if (state_forward)  vel += forward;
		if (state_backward) vel += backward;

		auto pos = XMLoadFloat3A(&position);
		pos += XMVectorScale(XMVector3Normalize(vel), velocity);

		XMStoreFloat3A(&position, pos);
	}
protected:
	void XM_CALLCONV calculate_move_vectors()
	{
		FXMVECTOR vmove_forward = XMLoadFloat3A(&look);
		FXMVECTOR vmove_right = XMVector3Normalize(XMVector3Cross(vmove_forward, XMLoadFloat3A(&up)));
		FXMVECTOR vmove_up = XMVector3Normalize(XMVector3Cross(vmove_right, vmove_forward));
		XMStoreFloat3A(&move_forward, vmove_forward);
		XMStoreFloat3A(&move_right, vmove_right);
		XMStoreFloat3A(&move_up, vmove_up);
	}

private:
	float velocity;
	XMFLOAT3A move_up;
	XMFLOAT3A move_right;
	XMFLOAT3A move_forward;

	bool state_up = false;
	bool state_down = false;
	bool state_left = false;
	bool state_right = false;
	bool state_forward = false;
	bool state_backward = false;

	CRITICAL_SECTION m_cs;
};