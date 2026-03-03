/**
 * main.cpp — Application entry point and render loop
 * 
 * WHAT: The "main" of the app. Creates the window, initializes ImGui,
 *       loads textures, then runs the frame loop (check time → draw
 *       character image → draw time pill → present to screen).
 * 
 * WHY:  This is intentionally short. All the heavy lifting is in:
 *         - window.h        → Win32 window + input handling
 *         - renderer.h      → DX11 GPU setup
 *         - texture_loader.h → loading PNG/JPEG → GPU texture
 *         - time_map.h      → which image to show at what hour
 * 
 * FLOW:
 *   1. CreateAppWindow()    → borderless always-on-top Win32 window
 *   2. CreateDeviceD3D()    → DX11 device + swap chain
 *   3. ImGui init           → sets up ImGui with DX11 + Win32 backends
 *   4. PreloadAllTextures() → loads all 13 time PNGs + fallback into GPU
 *   5. Main loop:
 *      a. Poll Win32 messages (mouse, keyboard, OS events)
 *      b. Check if the hour changed → swap character image
 *      c. Draw character image (aspect-ratio scaled, centered)
 *      d. Draw time pill overlay ("3:42 PM")
 *      e. Present frame to screen (vsync'd at 60fps)
 *   6. Cleanup on exit
 */

#include <string>
#include <ctime>
#include <map>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "window.h"
#include "renderer.h"
#include "texture_loader.h"
#include "time_map.h"

// ── Texture cache ─────────────────────────────────────────────
static std::map<std::string, Texture> g_textures;

// Load all 13 time-based images + fallback into GPU memory at startup
static bool PreloadAllTextures() {
    for (const auto& entry : GetTimeMap()) {
        std::string path = std::string("moods/time/") + entry.filename;
        Texture tex;
        if (LoadTextureFromFile(path.c_str(), g_pd3dDevice, &tex))
            g_textures[path] = tex;
    }
    Texture fallback;
    if (LoadTextureFromFile("moods/extra/anya1.jpeg", g_pd3dDevice, &fallback))
        g_textures["moods/extra/anya1.jpeg"] = fallback;

    return !g_textures.empty();
}

// Look up the texture for a given image path, with fallback
static Texture* FindTexture(const std::string& path) {
    auto it = g_textures.find(path);
    if (it != g_textures.end()) return &it->second;
    auto fb = g_textures.find("moods/extra/anya1.jpeg");
    if (fb != g_textures.end()) return &fb->second;
    return nullptr;
}

// ── Drawing helpers ───────────────────────────────────────────

// Draw the character image, scaled to fit window, centered
static void DrawCharacterImage(Texture* tex) {
    if (!tex || !tex->srv) return;

    float imgAspect = (float)tex->width / (float)tex->height;
    float winAspect = (float)WINDOW_W / (float)WINDOW_H;
    float drawW, drawH;

    if (imgAspect > winAspect) {
        drawW = (float)WINDOW_W;
        drawH = drawW / imgAspect;
    } else {
        drawH = (float)WINDOW_H;
        drawW = drawH * imgAspect;
    }

    float offsetX = ((float)WINDOW_W - drawW) * 0.5f;
    float offsetY = ((float)WINDOW_H - drawH) * 0.5f;
    ImGui::SetCursorPos(ImVec2(offsetX, offsetY));
    ImGui::Image((ImTextureID)tex->srv, ImVec2(drawW, drawH));
}

// Draw the time pill at the top center ("3:42 PM")
static void DrawTimePill() {
    std::string timeStr = GetCurrentTimeString();
    ImDrawList* dl = ImGui::GetWindowDrawList();

    ImVec2 textSize = ImGui::CalcTextSize(timeStr.c_str());
    float pad = 8.0f;
    float pillW = textSize.x + pad * 2;
    float pillH = textSize.y + pad * 2;
    float pillX = ((float)WINDOW_W - pillW) * 0.5f;
    float pillY = 10.0f;

    // Dark rounded background
    dl->AddRectFilled(
        ImVec2(pillX, pillY), ImVec2(pillX + pillW, pillY + pillH),
        IM_COL32(30, 30, 30, 200), pillH * 0.5f
    );
    // Pink border
    dl->AddRect(
        ImVec2(pillX, pillY), ImVec2(pillX + pillW, pillY + pillH),
        IM_COL32(255, 180, 200, 200), pillH * 0.5f, 0, 1.5f
    );
    // White text
    dl->AddText(
        ImVec2(pillX + pad, pillY + pad),
        IM_COL32(255, 255, 255, 255), timeStr.c_str()
    );
}

// ── Entry point ───────────────────────────────────────────────
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    // 1. Create window
    HWND hwnd = CreateAppWindow(hInstance);

    // 2. Init DX11
    if (!CreateDeviceD3D(hwnd)) {
        CleanupDeviceD3D();
        return 1;
    }
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // 3. Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding   = 0.0f;
    style.WindowBorderSize = 0.0f;
    style.WindowPadding    = ImVec2(0, 0);

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // 4. Preload images
    PreloadAllTextures();

    // 5. Main loop
    std::string currentImagePath = GetImageForCurrentHour();
    int lastCheckedMinute = -1;
    bool running = true;

    while (running) {
        // 5a. Process OS messages
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
            if (msg.message == WM_QUIT) running = false;
        }
        if (!running) break;

        // 5b. Check for time change (once per minute)
        {
            std::time_t now = std::time(nullptr);
            std::tm local{};
            localtime_s(&local, &now);
            if (local.tm_min != lastCheckedMinute) {
                lastCheckedMinute = local.tm_min;
                currentImagePath = GetImageForCurrentHour();
            }
        }

        // 5c. Begin ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Full-window panel (no decorations)
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2((float)WINDOW_W, (float)WINDOW_H));
        ImGui::Begin("MainWindow", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground);

        // 5d. Draw content
        DrawCharacterImage(FindTexture(currentImagePath));
        DrawTimePill();

        ImGui::End();

        // 5e. Present to screen
        ImGui::Render();
        const float clear[4] = { 0.1f, 0.1f, 0.12f, 1.0f };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        g_pSwapChain->Present(1, 0); // vsync on
    }

    // 6. Cleanup
    for (auto& [path, tex] : g_textures) ReleaseTexture(&tex);
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    return 0;
}
