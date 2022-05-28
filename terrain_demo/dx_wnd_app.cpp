#include "dx_wnd_app.h"
#include <assert.h>
#include <comdef.h>

#define CK(v) if (FAILED(hres = (v))) return hres

dx_wnd_app_c::dx_wnd_app_c(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show) {
    wnd_app_c::create_window(instance, prev_instance, cmd_line, cmd_show);
    create_render_device(d3d_renderer);
    create_mouse_keyboard_game_controller(mouse_keyboard_game_controller);

    mouse_keyboard_game_controller->register_action_id(VIRTUAL_KEY_UP,    false, true, ACTION_CAMERA_MOVE_UP);
    mouse_keyboard_game_controller->register_action_id(VIRTUAL_KEY_DOWN,  false, false, ACTION_CAMERA_MOVE_DOWN);
    mouse_keyboard_game_controller->register_action_id(VIRTUAL_KEY_LEFT,  false, false, ACTION_CAMERA_MOVE_LEFT);
    mouse_keyboard_game_controller->register_action_id(VIRTUAL_KEY_RIGHT, false, false, ACTION_CAMERA_MOVE_RIGHT);

    view_camera.subscribe(mouse_keyboard_game_controller);
}

dx_wnd_app_c::~dx_wnd_app_c() {
    destroy_render_device(d3d_renderer);
    destroy_mouse_keyboard_game_controller(mouse_keyboard_game_controller);
}

void dx_wnd_app_c::create_pipline(D3D_FEATURE_LEVEL feature_level) {
    d3d_renderer->create_pipeline(feature_level, wnd_handle);
}


HRESULT dx_wnd_app_c::on_render() {
    return d3d_renderer->on_render();
}


void dx_wnd_app_c::on_destroy() {
    d3d_renderer->destroy_pipeline();
}


void dx_wnd_app_c::static_render(wnd_app_c* wnd) {
    dx_wnd_app_c* dx_wnd = reinterpret_cast<dx_wnd_app_c*>(wnd);
    HRESULT hres = dx_wnd->on_render();
#ifdef _DEBUG
    if (hres != S_OK) {
        _com_error err(hres);
        OutputDebugString(err.ErrorMessage());
        Sleep(100);
    }
#endif
}


int dx_wnd_app_c::render_loop() {
    return wnd_app_c::loop(this, static_render);
}