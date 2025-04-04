#pragma once
#include <memory>
#include "Texture.h"
#include "Container/Map.h"

class FRenderer;
class FGraphicsDevice;
class FResourceMgr
{

public:
    void Initialize(FRenderer* renderer, FGraphicsDevice* device);
    void Release(FRenderer* renderer);
    HRESULT LoadTextureFromFile(ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* filename);
    HRESULT LoadTextureFromDDS(ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* filename);

    std::shared_ptr<FTexture> GetTexture(const FWString& name) const;
    std::shared_ptr<FTexture> CreateTexture(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* Res, const FWString& name);
    std::shared_ptr<FTexture> CreateTexture(ID3D11Device* device, ID3D11DeviceContext* context, D3D11_TEXTURE2D_DESC textureDesc, const FWString& name);
private:
    std::shared_ptr<FTexture> CreateTextureView(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* texture2D, D3D11_TEXTURE2D_DESC textureDesc, const
                                                FWString& name);
    
    TMap<FWString, std::shared_ptr<FTexture>> textureMap;
};