#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "demo_terrain_app.h"

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow
)
{
    demo_terrain_app_c App(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

    App.create_pipline(D3D_FEATURE_LEVEL_12_0);

    return App.render_loop();
}