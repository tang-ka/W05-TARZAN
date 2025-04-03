#pragma once

#define _TCHAR_DEFINED
#include <d3d11.h>
#include "Define.h"
#include "Core/Container/Array.h"



class FRenderResourceManager {
public:
    void Initialize(ID3D11Device* InDevice);
    void Release();

    template<typename T>
    inline ID3D11Buffer* CreateBuffer(
        const T* data,
        UINT count,
        D3D11_BIND_FLAG bindFlag,
        D3D11_USAGE usage = D3D11_USAGE_DEFAULT,
        UINT cpuAccessFlags = 0,
        UINT miscFlags = 0,
        UINT structureByteStride = 0)
    {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(T) * count;
        desc.Usage = usage;
        desc.BindFlags = bindFlag;
        desc.CPUAccessFlags = cpuAccessFlags;
        desc.MiscFlags = miscFlags;
        desc.StructureByteStride = structureByteStride;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = data;

        ID3D11Buffer* buffer = nullptr;
        HRESULT hr = Device->CreateBuffer(data ? &desc : &desc, data ? &initData : nullptr, &buffer);
        return SUCCEEDED(hr) ? buffer : nullptr;
    }

    template<typename T>
    inline ID3D11Buffer* CreateBuffer(
        const TArray<T>& data,
        D3D11_BIND_FLAG bindFlag,
        D3D11_USAGE usage = D3D11_USAGE_IMMUTABLE,
        UINT cpuAccessFlags = 0, 
        UINT miscFlags = 0,
        UINT structureByteStride = 0)
    {
        return CreateBuffer(data.GetData(), data.Num(), bindFlag, usage, cpuAccessFlags, miscFlags, structureByteStride);
    }
     

    template<typename T>    
    inline ID3D11Buffer* CreateVertexBuffer(const T* data, UINT size) {
        return CreateBuffer(data, size, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    }

    template<typename T>
    inline ID3D11Buffer* CreateVertexBuffer(const TArray<T>& data) {
        return CreateBuffer(data, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    }

    template<typename T>
    inline ID3D11Buffer* CreateIndexBuffer(const T* data, UINT size) {
        return CreateBuffer(data, size, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    }

    template<typename T>
    inline ID3D11Buffer* CreateIndexBuffer(const TArray<T>& data) {
        return CreateBuffer(data, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
    }

    template<typename T>
    inline ID3D11Buffer* CreateStructuredBuffer(UINT size) {
        return CreateBuffer<T>(nullptr, size, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(T));
    }

    ID3D11Buffer* CreateConstantBuffer(UINT size);

    void ReleaseBuffer(ID3D11Buffer*& buffer);

private:
    ID3D11Device* Device = nullptr;
};
