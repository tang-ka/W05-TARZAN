#include "Texture.h"

#include "EditorEngine.h"
#include "Engine/Engine.h"
#include "Windows/D3D11RHI/GraphicDevice.h"

ID3D11RenderTargetView* FTexture::GetRenderTargetView()
{
    if (TextureRTV == nullptr)
    {
        GEngine->graphicDevice.Device->CreateRenderTargetView(Texture, nullptr, &TextureRTV);
    }
    return TextureRTV;
}
