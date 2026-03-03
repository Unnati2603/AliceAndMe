/**
 * texture_loader.h — Loads image files (PNG/JPEG) into GPU textures
 * 
 * WHAT: Takes a filename on disk, decodes the pixels with stb_image,
 *       uploads them to the GPU as a DX11 texture, and returns a
 *       handle (ImTextureID) that ImGui can draw with ImGui::Image().
 * 
 * WHY:  ImGui doesn't load images itself. It only draws textures that
 *       are already on the GPU. This file bridges the gap:
 *       disk file → pixel bytes → GPU texture → ImGui can draw it.
 * 
 * FUNCTIONS:
 *   LoadTextureFromFile()  → load PNG/JPEG → DX11 texture
 *   ReleaseTexture()       → free GPU memory for a texture
 * 
 * STRUCTS:
 *   Texture { srv, width, height } — holds the GPU handle + dimensions
 */
#pragma once
#include <d3d11.h>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Texture {
    ID3D11ShaderResourceView* srv = nullptr;
    int width  = 0;
    int height = 0;
};

// Load a PNG/JPEG from disk and create a DX11 texture + shader resource view
inline bool LoadTextureFromFile(
    const char* filename,
    ID3D11Device* device,
    Texture* outTex)
{
    // Load image pixels
    int width, height, channels;
    unsigned char* pixels = stbi_load(filename, &width, &height, &channels, 4); // force RGBA
    if (!pixels)
        return false;

    // Create DX11 texture
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width            = width;
    desc.Height           = height;
    desc.MipLevels        = 1;
    desc.ArraySize        = 1;
    desc.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage            = D3D11_USAGE_DEFAULT;
    desc.BindFlags        = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA subResource = {};
    subResource.pSysMem          = pixels;
    subResource.SysMemPitch      = width * 4;

    ID3D11Texture2D* texture = nullptr;
    HRESULT hr = device->CreateTexture2D(&desc, &subResource, &texture);
    stbi_image_free(pixels);
    if (FAILED(hr))
        return false;

    // Create shader resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format                    = desc.Format;
    srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels       = 1;

    hr = device->CreateShaderResourceView(texture, &srvDesc, &outTex->srv);
    texture->Release();
    if (FAILED(hr))
        return false;

    outTex->width  = width;
    outTex->height = height;
    return true;
}

inline void ReleaseTexture(Texture* tex) {
    if (tex->srv) {
        tex->srv->Release();
        tex->srv = nullptr;
    }
    tex->width  = 0;
    tex->height = 0;
}
