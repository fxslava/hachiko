#include "camera.h"
#include "engine.h"

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
	auto* cb_mng = engine.get_constant_buffers_manager();

	FXMMATRIX view_mat = get_view();
	FXMMATRIX proj_mat = get_proj();
	FXMMATRIX view_proj_mat = XMMatrixMultiply(view_mat, proj_mat);

	XMStoreFloat4x4A(&cb_mng->engine_common.main_camera.view_proj_mat, view_proj_mat);
}


movable_camera_c::movable_camera_c() :
	render_camera_c(
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
		XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
		XM_PIDIV2, 1920.0f / 1080.0f, 0.1f, 1000.0f),
	velocity(1.0f)
{
	calculate_move_vectors();
}


movable_camera_c::movable_camera_c(FXMVECTOR position_, FXMVECTOR look_, FXMVECTOR up_, const float fov_angleY_, const float aspect_ratio_, const float nearZ_, const float farZ_, const float velocity_) :
	render_camera_c(position_, look_, up_, fov_angleY_, aspect_ratio_, nearZ_, farZ_),
	velocity(velocity_)
{
	calculate_move_vectors();
}


void movable_camera_c::motion_config(float velocity_) {
	velocity = velocity_;
}


void movable_camera_c::subscribe(input_controller_i* controller)
{
	for (ACTION_ID action_id = ACTION_CAMERA_BEGIN; action_id < ACTION_CAMERA_END; action_id++)
		controller->subscribe(this, action_id);
}


void movable_camera_c::call(ACTION_ID action_id, INPUT_CONTROLLER_ACTION_STATE action_state)
{
	switch (action_id)
	{
	case ACTION_CAMERA_MOVE_UP:	      state_up = action_state.state; break;
	case ACTION_CAMERA_MOVE_DOWN:     state_down = action_state.state; break;
	case ACTION_CAMERA_MOVE_LEFT:     state_left = action_state.state; break;
	case ACTION_CAMERA_MOVE_RIGHT:    state_right = action_state.state; break;
	case ACTION_CAMERA_MOVE_FORWARD:  state_forward = action_state.state; break;
	case ACTION_CAMERA_MOVE_BACKWARD: state_backward = action_state.state; break;
	}
};


FXMVECTOR movable_camera_c::make_look_dir(const float azim, const float polar) const
{
	FXMVECTOR up      = XMLoadFloat3A(&move_up);
	FXMVECTOR forward = XMLoadFloat3A(&move_forward);
	FXMVECTOR right   = XMLoadFloat3A(&move_right);

	const float sin_polar = sinf(polar);
	FXMVECTOR x = XMVectorScale(right,   sin_polar * cosf(azim));
	FXMVECTOR y = XMVectorScale(forward, sin_polar * sinf(azim));
	FXMVECTOR z = XMVectorScale(up,      cosf(polar));

	return x + y + z;
}


void movable_camera_c::update(float elapsed_time)
{
	auto pos = XMLoadFloat3A(&position);
	auto look_at = XMLoadFloat3A(&look);

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
	//look_at += vel;

	XMStoreFloat3A(&position, pos);
	XMStoreFloat3A(&look, look_at);
}


void XM_CALLCONV movable_camera_c::calculate_move_vectors()
{
	FXMVECTOR vmove_forward = XMLoadFloat3A(&look);
	FXMVECTOR vmove_right = -XMVector3Normalize(XMVector3Cross(vmove_forward, XMLoadFloat3A(&up)));
	FXMVECTOR vmove_up = XMVector3Normalize(XMVector3Cross(vmove_right, vmove_forward));
	XMStoreFloat3A(&move_forward, vmove_forward);
	XMStoreFloat3A(&move_right, vmove_right);
	XMStoreFloat3A(&move_up, vmove_up);
}