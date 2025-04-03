#include "RenderResourceManager.h"

void FRenderResourceManager::Initialize(ID3D11Device* InDevice) {
    Device = InDevice;
}

ID3D11Buffer* FRenderResourceManager::CreateConstantBuffer(UINT size)
{
    D3D11_BUFFER_DESC constantbufferdesc = {};
    constantbufferdesc.ByteWidth = size + 0xf & 0xfffffff0;
    constantbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
    constantbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constantbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    ID3D11Buffer* buffer = nullptr;
    Device->CreateBuffer(&constantbufferdesc, nullptr, &buffer);
    return buffer;
}

void FRenderResourceManager::ReleaseBuffer(ID3D11Buffer*& buffer) {
    if (buffer)
    {
        buffer->Release();
        buffer = nullptr;
    }
}