#include "demo_terrain_app.h"
#include "engine.h"
#include <comdef.h>

#define CK(v) if (FAILED(hres = (v))) return hres

demo_terrain_app_c::demo_terrain_app_c(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show) {
    wnd_app_c::create_window(instance, prev_instance, cmd_line, cmd_show);

    mouse_keyboard_input_controller->register_action_id(VIRTUAL_KEY_UP,      false, false, ACTION_CAMERA_MOVE_FORWARD);
    mouse_keyboard_input_controller->register_action_id(VIRTUAL_KEY_DOWN,    false, false, ACTION_CAMERA_MOVE_BACKWARD);
    mouse_keyboard_input_controller->register_action_id(VIRTUAL_KEY_LEFT,    false, false, ACTION_CAMERA_MOVE_LEFT);
    mouse_keyboard_input_controller->register_action_id(VIRTUAL_KEY_RIGHT,   false, false, ACTION_CAMERA_MOVE_RIGHT);
    mouse_keyboard_input_controller->register_action_id(VIRTUAL_KEY_W,       false, false, ACTION_CAMERA_MOVE_FORWARD);
    mouse_keyboard_input_controller->register_action_id(VIRTUAL_KEY_S,       false, false, ACTION_CAMERA_MOVE_BACKWARD);
    mouse_keyboard_input_controller->register_action_id(VIRTUAL_KEY_A,       false, false, ACTION_CAMERA_MOVE_LEFT);
    mouse_keyboard_input_controller->register_action_id(VIRTUAL_KEY_D,       false, false, ACTION_CAMERA_MOVE_RIGHT);
    mouse_keyboard_input_controller->register_action_id(VIRTUAL_KEY_E,       false, false, ACTION_CAMERA_MOVE_UP);
    mouse_keyboard_input_controller->register_action_id(VIRTUAL_KEY_Q,       false, false, ACTION_CAMERA_MOVE_DOWN);
    mouse_keyboard_input_controller->register_action_id(VIRTUAL_KEY_LBUTTON, false, false, ACTION_CAMERA_ROTATE_RIGHT);
    mouse_keyboard_input_controller->register_action_id(VIRTUAL_KEY_LBUTTON, false, false, ACTION_CAMERA_ROTATE_UP);

    mouse_keyboard_input_controller->associate_with_mouse(ACTION_CAMERA_ROTATE_RIGHT, MOUSE_MOVE_MASK_MOUSE_X, false);
    mouse_keyboard_input_controller->associate_with_mouse(ACTION_CAMERA_ROTATE_UP, MOUSE_MOVE_MASK_MOUSE_Y, false);

    view_camera.set_position(
        XMVectorSet(0.f, 0.f,-1.f, 0.f),
        XMVectorSet(0.f, 0.f, 1.f, 0.f)
    );
    view_camera.motion_config(0.1f);
    view_camera.subscribe(mouse_keyboard_input_controller);

    update_timestamp();
}


void demo_terrain_app_c::update_timestamp()
{
    const auto duration = std::chrono::steady_clock::now().time_since_epoch();
    const auto new_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    if (last_timestamp >= 0) {
        elapsed_time = static_cast<float>(new_timestamp - last_timestamp) * (1.f / 1000.f);
    }
}


HRESULT demo_terrain_app_c::create_pipline(D3D_FEATURE_LEVEL feature_level) {
    HRESULT hres;

    auto& engine = engine_c::get_instance();

    CK(engine.init_sub_systems(1920, 1080, feature_level, wnd_handle));
    CK(terrain.allocate_resources());

    // get subsystems pointers
    d3d_renderer = engine.get_renderer();
    resource_menager = engine.get_resource_manager();
    constant_buffers_manager = engine.get_constant_buffers_manager();

    return S_OK;
}


HRESULT demo_terrain_app_c::update(ID3D12GraphicsCommandList* command_list) {
    HRESULT hres;
    auto& engine = engine_c::get_instance();

    update_timestamp();
    view_camera.update(elapsed_time);
    view_camera.apply_camera_view();
    CK(terrain.update(command_list));

    return S_OK;
}


HRESULT demo_terrain_app_c::prepare_frame(ID3D12GraphicsCommandList* command_list) {
    HRESULT hres;
    auto& engine = engine_c::get_instance();

    CK(terrain.prepare_frame(command_list));
    CK(engine.prepare_frame(command_list));

    return S_OK;
}


HRESULT demo_terrain_app_c::on_render() {

    ID3D12GraphicsCommandList* command_list;
    HRESULT hres;
    CK(d3d_renderer->begin_render(&command_list));

    prepare_frame(command_list);
    update(command_list);

    float clear_color[] = { 0.4f, 0.4f, 0.7f, 1.0f };
    CK(d3d_renderer->clear_render_target(clear_color));

    terrain.render(command_list);

    CK(d3d_renderer->end_render(command_list));
    return S_OK;
}


void demo_terrain_app_c::on_destroy() {
    auto& engine = engine_c::get_instance();
    engine.shut_down();
}


void demo_terrain_app_c::static_render(wnd_app_c* wnd) {
    demo_terrain_app_c* dx_wnd = reinterpret_cast<demo_terrain_app_c*>(wnd);
    HRESULT hres = dx_wnd->on_render();
#ifdef _DEBUG
    if (hres != S_OK) {
        _com_error err(hres);
        OutputDebugString(err.ErrorMessage());
        Sleep(100);
    }
#endif
}


int demo_terrain_app_c::render_loop() {
    return loop(this, static_render);
}