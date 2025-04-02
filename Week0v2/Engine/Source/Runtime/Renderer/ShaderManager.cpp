#include "ShaderManager.h"
#include <d3dcompiler.h>

void FShaderManager::Initialize(ID3D11Device* InDevice)
{
    Device = InDevice;
}

void FShaderManager::Release()
{
    Device = nullptr;
}

bool FShaderManager::CreateVertexShader(
    const std::wstring& vsPath,
    const std::string& vsEntry,
    ID3D11VertexShader*& outVS,
    const D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc,
    UINT numElements,
    ID3D11InputLayout** outInputLayout,
    UINT* outStride,
    UINT vertexSize)
{
    ID3DBlob* vsBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(vsPath.c_str(), nullptr, nullptr, vsEntry.c_str(), "vs_5_0", 0, 0, &vsBlob, nullptr);
    if (FAILED(hr)) return false;

    hr = Device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &outVS);
    if (FAILED(hr)) { vsBlob->Release(); return false; }

    if (outInputLayout)
    {
        hr = Device->CreateInputLayout(inputLayoutDesc, numElements, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), outInputLayout);
        if (FAILED(hr)) { vsBlob->Release(); return false; }
    }

    if (outStride)
        *outStride = vertexSize;

    vsBlob->Release();
    return true;
}

bool FShaderManager::CreatePixelShader(
    const std::wstring& psPath,
    const std::string& psEntry,
    ID3D11PixelShader*& outPS)
{
    ID3DBlob* psBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(psPath.c_str(), nullptr, nullptr, psEntry.c_str(), "ps_5_0", 0, 0, &psBlob, nullptr);
    if (FAILED(hr)) return false;

    hr = Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &outPS);
    psBlob->Release();
    return SUCCEEDED(hr);
}

void FShaderManager::ReleaseShader(ID3D11InputLayout* layout, ID3D11VertexShader* vs, ID3D11PixelShader* ps)
{
    if (layout)
    {
        layout->Release();
        layout = nullptr;
    }
    if (vs)
    {
        vs->Release();
        vs = nullptr;
    }
    if (ps)
    {
        ps->Release();
        ps = nullptr;
    }
}
