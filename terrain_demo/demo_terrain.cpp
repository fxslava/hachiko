#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include "dx_wnd_app.h"

void Render(wnd_app_c* wnd) {
    auto hwnd = wnd->get_hwnd();
}

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow
)
{
    dx_wnd_app_c App(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

    App.create_pipeline(D3D_FEATURE_LEVEL_12_0);

    return App.render_loop();
}