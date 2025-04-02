#pragma once
#include <d3d11.h>
#include <string>

class FShaderManager
{
public:
    void Initialize(ID3D11Device* InDevice);
    void Release();

    bool CreateVertexShader(
        const std::wstring& vsPath,
        const std::string& vsEntry,
        ID3D11VertexShader*& outVS,
        const D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc,
        UINT numElements,
        ID3D11InputLayout** outInputLayout = nullptr,
        UINT* outStride = nullptr,
        UINT vertexSize = 0);

    bool CreatePixelShader(
        const std::wstring& psPath,
        const std::string& psEntry,
        ID3D11PixelShader*& outPS);

    void ReleaseShader(ID3D11InputLayout* layout, ID3D11VertexShader* vs, ID3D11PixelShader* ps);

private:
    ID3D11Device* Device = nullptr;
};

