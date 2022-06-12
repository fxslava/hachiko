#include "demo_terrain_app.h"
#include <assert.h>
#include <comdef.h>

#define CK(v) if (FAILED(hres = (v))) return hres

demo_terrain_app_c::demo_terrain_app_c(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show) {
    wnd_app_c::create_window(instance, prev_instance, cmd_line, cmd_show);
    create_render_device(d3d_renderer);

    mouse_keyboard_input_controller->register_action_id(VIRTUAL_KEY_UP,    false, false, ACTION_CAMERA_MOVE_FORWARD);
    mouse_keyboard_input_controller->register_action_id(VIRTUAL_KEY_DOWN,  false, false, ACTION_CAMERA_MOVE_BACKWARD);
    mouse_keyboard_input_controller->register_action_id(VIRTUAL_KEY_LEFT,  false, false, ACTION_CAMERA_MOVE_LEFT);
    mouse_keyboard_input_controller->register_action_id(VIRTUAL_KEY_RIGHT, false, false, ACTION_CAMERA_MOVE_RIGHT);
    mouse_keyboard_input_controller->register_action_id(VIRTUAL_KEY_W,     false, false, ACTION_CAMERA_MOVE_FORWARD);
    mouse_keyboard_input_controller->register_action_id(VIRTUAL_KEY_S,     false, false, ACTION_CAMERA_MOVE_BACKWARD);
    mouse_keyboard_input_controller->register_action_id(VIRTUAL_KEY_A,     false, false, ACTION_CAMERA_MOVE_LEFT);
    mouse_keyboard_input_controller->register_action_id(VIRTUAL_KEY_D,     false, false, ACTION_CAMERA_MOVE_RIGHT);

    view_camera.subscribe(mouse_keyboard_input_controller);

    create_terrain_actor(terrain);
}


HRESULT demo_terrain_app_c::create_pipline(D3D_FEATURE_LEVEL feature_level) {
    HRESULT hres;

    CK(d3d_renderer->create_pipeline(1920, 1080, feature_level, wnd_handle));

    CK(terrain->allocate_resources(d3d_renderer));

    return S_OK;
}


HRESULT demo_terrain_app_c::update()
{
    return terrain->update();
}


HRESULT demo_terrain_app_c::on_render() {

    ID3D12GraphicsCommandList* command_list;
    HRESULT hres;
    CK(d3d_renderer->begin_command_list(&command_list));

    update();

    float clear_color[] = { 0.4f, 0.4f, 0.7f, 1.0f };
    CK(d3d_renderer->clear_render_target(command_list, clear_color));

    terrain->render(command_list);

    CK(d3d_renderer->end_command_list(command_list));

    return d3d_renderer->on_render();
}


void demo_terrain_app_c::on_destroy() {
    destroy_terrain_actor(terrain);
    d3d_renderer->destroy_pipeline();
    destroy_render_device(d3d_renderer);
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
    return wnd_app_c::loop(this, static_render);
}