#include "wnd_app.h"


wnd_app_c::wnd_app_c(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show) : instance(instance) {
    create_window(instance, prev_instance, cmd_line, cmd_show);
}


void wnd_app_c::create_window(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show) {
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = wnd_proc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = instance;
    wcex.hIcon = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_DEMO_APP));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = get_class_name();
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex)) {
        throw std::runtime_error("Call to RegisterClassEx failed!");
    }

    wnd_handle = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        get_class_name(),
        get_title_name(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1920, 1080,
        NULL,
        NULL,
        instance,
        this
    );

    if (!wnd_handle) {
        throw std::runtime_error("Call to CreateWindow failed!");
    }

    ShowWindow(wnd_handle, cmd_show);
    UpdateWindow(wnd_handle);
}


LRESULT CALLBACK wnd_app_c::wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    auto wnd = reinterpret_cast<wnd_app_c*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    switch (message)
    {
    case WM_CREATE:
    {
        auto pParent = reinterpret_cast<wnd_app_c*>(((LPCREATESTRUCT)lParam)->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pParent);
    }
    break;
    case WM_PAINT:
        return wnd->on_paint();
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

int wnd_app_c::loop(wnd_app_c* ctx, render_callback_t render_cb) {
    MSG msg;
    while (TRUE) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (render_cb) render_cb(ctx);
        }

        if (msg.message == WM_QUIT)  break;
    }
    ctx->on_destroy();

    return static_cast<int>(msg.wParam);
}


LRESULT wnd_app_c::on_paint() {
    WCHAR buffer[256];
    swprintf_s(buffer, 256, L"mouse X: %d\tmouse Y: %d", mouse_x, mouse_y);

    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(wnd_handle, &ps);
    TextOut(hdc, 5, 5, buffer, wcslen(buffer));
    EndPaint(wnd_handle, &ps);
    return 0;
}


void wnd_app_c::on_destroy() {

}