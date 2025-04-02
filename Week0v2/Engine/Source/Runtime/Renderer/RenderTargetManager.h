#pragma once
#include <d3d11.h>

class FRenderTargetManager
{
public:
    void Initialize(ID3D11Device* device, HWND hWnd);
    void Resize(ID3D11Device* device, HWND hWnd);
    void Release();

    void BindAll(ID3D11DeviceContext* context);
    void BindMainOnly(ID3D11DeviceContext* context);
    void Clear(ID3D11DeviceContext* context, const float clearColor[4]);

    ID3D11RenderTargetView* GetMainRTV() const { return MainRTV; }
    ID3D11RenderTargetView* GetUUIDRTV() const { return UUIDRTV; }
    ID3D11DepthStencilView* GetDSV() const { return DepthStencilView; }

    int GetWidth() const { return Width; }
    int GetHeight() const { return Height; }

private:
    void CreateBuffers(ID3D11Device* device, HWND hWnd);

private:
    ID3D11RenderTargetView* MainRTV = nullptr;
    ID3D11RenderTargetView* UUIDRTV = nullptr;
    ID3D11DepthStencilView* DepthStencilView = nullptr;

    ID3D11Texture2D* MainRT = nullptr;
    ID3D11Texture2D* UUIDRT = nullptr;
    ID3D11Texture2D* DepthStencilBuffer = nullptr;

    UINT Width = 0;
    UINT Height = 0;
};
