#pragma once
#include "D3D11RHI/GraphicDevice.h"

struct FTexture
{
public:
    FTexture(ID3D11ShaderResourceView* SRV, ID3D11Texture2D* Texture2D, ID3D11SamplerState* Sampler, uint32 _width, uint32 _height)
        : TextureSRV(SRV), Texture(Texture2D), SamplerState(Sampler), width(_width), height(_height), TextureRTV(nullptr)
    {}
    
    FTexture(ID3D11RenderTargetView* RTV, ID3D11ShaderResourceView* SRV, ID3D11Texture2D* Texture2D, ID3D11SamplerState* Sampler, uint32 _width, uint32 _height)
        : TextureSRV(SRV), Texture(Texture2D), SamplerState(Sampler), width(_width), height(_height), TextureRTV(RTV)
    {}
    ~FTexture()
    {
		
    }
    void Release() {
        if (TextureSRV) { TextureSRV->Release(); TextureSRV = nullptr; }
        if (Texture) { Texture->Release(); Texture = nullptr; }
        if (SamplerState) { SamplerState->Release(); SamplerState = nullptr; }
    }

    ID3D11RenderTargetView* GetRenderTargetView(); 
    
    ID3D11ShaderResourceView* TextureSRV = nullptr;
    ID3D11Texture2D* Texture = nullptr;
    ID3D11SamplerState* SamplerState = nullptr;
    uint32 width;
    uint32 height;

private:
    ID3D11RenderTargetView* TextureRTV = nullptr;
};


