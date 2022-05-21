#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include "wnd_app.h"

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow
)
{
    wnd_app_c App(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    return wnd_app_c::loop(&App, nullptr);
}