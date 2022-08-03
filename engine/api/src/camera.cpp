#include "camera.h"
#include "engine.h"
#include "utils.h"

camera_c::camera_c(FXMVECTOR position_, FXMVECTOR look_, FXMVECTOR up_, const float fov_angleY_, const float aspect_ratio_, const float nearZ_, const float farZ_) :
	fov_angleY(fov_angleY_), aspect_ratio(aspect_ratio_), nearZ(nearZ_), farZ(farZ_)
{
	XMStoreFloat3(&position, position_);
	XMStoreFloat3(&look, look_);
	XMStoreFloat3(&up, up_);
}


FXMMATRIX camera_c::get_view() const {
	const XMVECTOR vposition = XMLoadFloat3(&position);
	const XMVECTOR vlook = XMLoadFloat3(&look);
	const XMVECTOR vup = XMLoadFloat3(&up);
	return XMMatrixLookToLH(vposition, vlook, vup);
}


FXMMATRIX camera_c::get_proj() const {
	return XMMatrixPerspectiveFovLH(fov_angleY, aspect_ratio, nearZ, farZ);
}


void camera_c::set_position(FXMVECTOR pos, FXMVECTOR look_at)
{
	XMStoreFloat3A(&position, pos);
	XMStoreFloat3A(&look, look_at);
}


render_camera_c::render_camera_c(FXMVECTOR position_, FXMVECTOR look_, FXMVECTOR up_, const float fov_angleY_, const float aspect_ratio_, const float nearZ_, const float farZ_) :
	camera_c(position_, look_, up_, fov_angleY_, aspect_ratio_, nearZ_, farZ_)
{}


void render_camera_c::apply_camera_view() const {
	auto& engine = engine_c::get_instance();
	auto& view_proj_mat_cb = engine.common_engine_cb.view_proj_mat;

	FXMMATRIX view_mat = get_view();
	FXMMATRIX proj_mat = get_proj();
	FXMMATRIX view_proj_mat = XMMatrixMultiply(view_mat, proj_mat);

	XMStoreFloat4x4A(&view_proj_mat_cb, view_proj_mat);
}


movable_camera_c::movable_camera_c() :
	render_camera_c(
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
		XM_PIDIV2, 1920.0f / 1080.0f, 0.1f, 1000.0f),
	velocity(1.0f),
	camera_input_event_listener(*this)
{
	calculate_move_vectors();
}


movable_camera_c::movable_camera_c(FXMVECTOR position_, FXMVECTOR look_, FXMVECTOR up_, const float fov_angleY_, const float aspect_ratio_, const float nearZ_, const float farZ_, const float velocity_) :
	render_camera_c(position_, look_, up_, fov_angleY_, aspect_ratio_, nearZ_, farZ_),
	velocity(velocity_),
	camera_input_event_listener(*this)
{
	calculate_move_vectors();
}


void movable_camera_c::motion_config(float velocity_) {
	velocity = velocity_;
}


void movable_camera_c::subscribe(input_controller_i* controller)
{
	for (ACTION_ID action_id = ACTION_CAMERA_BEGIN; action_id < ACTION_CAMERA_END; action_id++) {
		controller->subscribe(&camera_input_event_listener, action_id);
	}
}


void movable_camera_c::camera_input_event_handler_c::call(ACTION_ID action_id, INPUT_CONTROLLER_ACTION_STATE action_state)
{
	switch (action_id)
	{
	case ACTION_CAMERA_MOVE_UP:	      camera.state_up = action_state.state; break;
	case ACTION_CAMERA_MOVE_DOWN:     camera.state_down = action_state.state; break;
	case ACTION_CAMERA_MOVE_LEFT:     camera.state_left = action_state.state; break;
	case ACTION_CAMERA_MOVE_RIGHT:    camera.state_right = action_state.state; break;
	case ACTION_CAMERA_MOVE_FORWARD:  camera.state_forward = action_state.state; break;
	case ACTION_CAMERA_MOVE_BACKWARD: camera.state_backward = action_state.state; break;
	case ACTION_CAMERA_ROTATE_RIGHT:
		camera.azimuth += action_state.control_vel * camera.mouse_speed;
		break;
	case ACTION_CAMERA_ROTATE_UP:
		camera.polar = CLAMP(camera.polar + action_state.control_vel * camera.mouse_speed, 0.f, XM_PI - 0.01f);
		break;
	}
};


void movable_camera_c::make_move_dir()
{
	FXMVECTOR up      = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	FXMVECTOR forward = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	FXMVECTOR right   = XMVectorSet(1.f, 0.f, 0.f, 0.f);

	const float sin_polar   = sinf(polar);
	const float cos_polar   = cosf(polar);
	const float sin_azimuth = sinf(azimuth);
	const float cos_azimuth = cosf(azimuth);

	FXMVECTOR look_x = XMVectorScale(right,   sin_polar * sin_azimuth);
	FXMVECTOR look_z = XMVectorScale(forward, sin_polar * cos_azimuth);
	FXMVECTOR look_y = XMVectorScale(up,      cos_polar);

	XMStoreFloat3A(&look,         look_x + look_y + look_z);
	XMStoreFloat3A(&move_forward, XMVectorScale(right, sin_azimuth) + XMVectorScale(forward, cos_azimuth));
	XMStoreFloat3A(&move_right,   XMVectorScale(right, cos_azimuth) + XMVectorScale(forward, -sin_azimuth));
}


void movable_camera_c::update(float elapsed_time)
{
	auto pos = XMLoadFloat3A(&position);
	make_move_dir();

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

	vel = XMVectorScale(XMVector3Normalize(vel), velocity);

	pos += vel;

	XMStoreFloat3A(&position, pos);
}


void XM_CALLCONV movable_camera_c::calculate_move_vectors()
{
	FXMVECTOR vmove_forward = XMLoadFloat3A(&look);
	FXMVECTOR vmove_right = -XMVector3Normalize(XMVector3Cross(vmove_forward, XMLoadFloat3A(&up)));
	FXMVECTOR vmove_up = XMVector3Normalize(XMVector3Cross(vmove_forward, vmove_right));
	XMStoreFloat3A(&move_forward, vmove_forward);
	XMStoreFloat3A(&move_right, vmove_right);
	XMStoreFloat3A(&move_up, vmove_up);
}