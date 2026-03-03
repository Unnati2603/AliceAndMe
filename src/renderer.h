/**
 * renderer.h — DirectX 11 initialization and management
 * 
 * WHAT: Sets up the DX11 device, swap chain, and render target that
 *       ImGui draws into. This is the "GPU plumbing" — it creates the
 *       connection between our app and the graphics card.
 * 
 * WHY:  ImGui doesn't render pixels itself. It generates draw commands,
 *       and DX11 is the backend that actually puts pixels on screen.
 *       This file isolates all that DX11 boilerplate so main.cpp stays clean.
 * 
 * KEY CONCEPTS:
 *   - Device:       represents the GPU, used to create textures/buffers
 *   - DeviceContext: sends draw commands to the GPU
 *   - SwapChain:    double-buffer (draw on back buffer, flip to front)
 *   - RenderTarget: the back buffer texture we draw into each frame
 */
#pragma once

#include <d3d11.h>

// ── Global DX11 objects (used by main.cpp and texture_loader.h) ───
inline ID3D11Device*            g_pd3dDevice           = nullptr;
inline ID3D11DeviceContext*     g_pd3dDeviceContext     = nullptr;
inline IDXGISwapChain*          g_pSwapChain           = nullptr;
inline ID3D11RenderTargetView*  g_mainRenderTargetView = nullptr;

// Create the back-buffer render target view from the swap chain
inline void CreateRenderTarget() {
    ID3D11Texture2D* pBackBuffer = nullptr;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (pBackBuffer) {
        g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
        pBackBuffer->Release();
    }
}

// Release the render target (called before resizing swap chain buffers)
inline void CleanupRenderTarget() {
    if (g_mainRenderTargetView) {
        g_mainRenderTargetView->Release();
        g_mainRenderTargetView = nullptr;
    }
}

// Initialize DX11: create device, swap chain, and render target
// Returns false if the GPU doesn't support DX11
inline bool CreateDeviceD3D(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount                        = 2;
    sd.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator   = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags                              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow                       = hWnd;
    sd.SampleDesc.Count                   = 1;
    sd.Windowed                           = TRUE;
    sd.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;

    const D3D_FEATURE_LEVEL featureLevelArray[] = { D3D_FEATURE_LEVEL_11_0 };
    D3D_FEATURE_LEVEL featureLevel;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        featureLevelArray, 1, D3D11_SDK_VERSION,
        &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext
    );
    if (FAILED(hr))
        return false;

    CreateRenderTarget();
    return true;
}

// Release all DX11 resources (called once at app shutdown)
inline void CleanupDeviceD3D() {
    CleanupRenderTarget();
    if (g_pSwapChain)       { g_pSwapChain->Release();       g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice)       { g_pd3dDevice->Release();       g_pd3dDevice = nullptr; }
}
