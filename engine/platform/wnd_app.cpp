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
    wcex.hIcon = get_app_icon(instance);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = get_class_name();
    wcex.hIconSm = get_app_icon_sm(instance);

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
    create_mouse_keyboard_input_controller(mouse_keyboard_input_controller);
}


LRESULT CALLBACK wnd_app_c::wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    auto wnd = reinterpret_cast<wnd_app_c*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    if (wnd && wnd->custom_wnd_proc(hWnd, message, wParam, lParam))
        return true;

    switch (message)
    {
    case WM_CREATE:
    {
        auto pParent = reinterpret_cast<wnd_app_c*>(((LPCREATESTRUCT)lParam)->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pParent);
    }
    break;
    case WM_PAINT:         return wnd->on_paint();
    case WM_KEYDOWN:       return wnd->on_wm_keydown(wParam, lParam);
    case WM_KEYUP:         return wnd->on_wm_keyup(wParam, lParam);
    case WM_UNICHAR:       return wnd->on_wm_unichar(wParam, lParam);
    case WM_CHAR:          return wnd->on_wm_char(wParam, lParam);
    case WM_LBUTTONDBLCLK: return wnd->on_wm_lbuttondblclk(wParam, lParam);
    case WM_LBUTTONDOWN:   return wnd->on_wm_lbuttonddown(wParam, lParam);
    case WM_LBUTTONUP:     return wnd->on_wm_lbuttondup(wParam, lParam);
    case WM_MBUTTONDBLCLK: return wnd->on_wm_mbuttondblclk(wParam, lParam);
    case WM_MBUTTONDOWN:   return wnd->on_wm_mbuttonddown(wParam, lParam);
    case WM_MBUTTONUP:     return wnd->on_wm_mbuttondup(wParam, lParam);
    case WM_RBUTTONDBLCLK: return wnd->on_wm_rbuttondblclk(wParam, lParam);
    case WM_RBUTTONDOWN:   return wnd->on_wm_rbuttonddown(wParam, lParam);
    case WM_RBUTTONUP:     return wnd->on_wm_rbuttondup(wParam, lParam);
    case WM_MOUSEWHEEL:    return wnd->on_wm_mousewheel(wParam, lParam);
    case WM_MOUSEMOVE:     return wnd->on_wm_mousemove(wParam, lParam);
    case WM_DESTROY:
        //wnd->on_destroy();
        wnd->running = false;
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

int wnd_app_c::loop(wnd_app_c* ctx, render_callback_t render_cb) {
    MSG msg;
    while (ctx->running) {
        bool perform_idle = true;

        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                return (int)msg.wParam;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);

            // almost full fill window message queue problem fix
            if (msg.message == WM_KEYDOWN || msg.message == WM_KEYUP || msg.message == WM_CHAR || msg.message == WM_UNICHAR) {
                perform_idle = false;
            }
        }

        if (render_cb && perform_idle) render_cb(ctx);
    }
    ctx->on_destroy();

    return static_cast<int>(msg.wParam);
}


LRESULT wnd_app_c::on_paint() {
    WCHAR buffer[256];
    swprintf_s(buffer, 256, L"mouse X: %d\tmouse Y: %d", mouse_x, mouse_y);

    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(wnd_handle, &ps);
    TextOut(hdc, 5, 5, buffer, int(wcslen(buffer)));
    EndPaint(wnd_handle, &ps);
    return 0;
}


void wnd_app_c::on_destroy() {
    destroy_mouse_keyboard_input_controller(mouse_keyboard_input_controller);
}