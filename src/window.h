/**
 * window.h — Win32 window creation and message handling
 * 
 * WHAT: Creates the actual OS window you see on screen and handles
 *       user input (dragging, keyboard, resizing). This is the
 *       "operating system layer" of the app.
 * 
 * WHY:  On Windows, every app needs a Win32 window. Even though ImGui
 *       draws the UI, the window itself is managed by the OS. This file
 *       isolates all that Win32 boilerplate.
 * 
 * KEY BEHAVIORS:
 *   - WS_POPUP:      no title bar, no borders (borderless window)
 *   - WS_EX_TOPMOST: always stays above other windows
 *   - WM_NCHITTEST:  makes the entire window draggable by click+drag
 *   - VK_ESCAPE:     pressing Escape closes the app
 */
#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "imgui_impl_win32.h"
#include "renderer.h"

// ImGui's Win32 input handler (mouse, keyboard, etc.)
extern "C++" LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Window dimensions
static const int WINDOW_W = 500;
static const int WINDOW_H = 600;

// Win32 message callback — handles input and window events
inline LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // Let ImGui process input first (mouse clicks, keyboard, etc.)
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {

    // DRAGGING: treat any click on the window body as a title-bar drag
    case WM_NCHITTEST: {
        LRESULT hit = DefWindowProcW(hWnd, msg, wParam, lParam);
        if (hit == HTCLIENT)
            return HTCAPTION;  // "pretend this is the title bar"
        return hit;
    }

    // KEYBOARD: Escape to quit
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
            PostQuitMessage(0);
        return 0;

    // CLOSE: clean exit
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    // RESIZE: recreate the DX11 render target when window size changes
    case WM_SIZE:
        if (g_pd3dDevice && wParam != SIZE_MINIMIZED) {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam),
                                         DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// Create and register the Win32 window, returns the HWND (window handle)
inline HWND CreateAppWindow(HINSTANCE hInstance) {
    WNDCLASSEXW wc = {};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_CLASSDC;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance      = hInstance;
    wc.lpszClassName  = L"AliceAndMeClass";
    wc.hCursor        = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512)); // arrow cursor
    RegisterClassExW(&wc);

    // Center on screen
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    HWND hwnd = CreateWindowExW(
        WS_EX_TOPMOST,             // always on top
        wc.lpszClassName,
        L"Alice And Me",
        WS_POPUP,                   // borderless (no title bar)
        (screenW - WINDOW_W) / 2,   // centered X
        (screenH - WINDOW_H) / 2,   // centered Y
        WINDOW_W, WINDOW_H,
        nullptr, nullptr, hInstance, nullptr
    );
    return hwnd;
}
