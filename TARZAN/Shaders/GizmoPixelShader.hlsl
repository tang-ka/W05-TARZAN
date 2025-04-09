
struct FMaterial
{
    float3 DiffuseColor;
    float TransparencyScalar;
    float3 AmbientColor;
    float DensityScalar;
    float3 SpecularColor;
    float SpecularScalar;
    float3 EmissiveColor;
    float MaterialPad0;
};

cbuffer MaterialConstants : register(b0)
{
    FMaterial Material;
}

cbuffer ScreenInfo : register(b2)
{
    float2 ViewPortRatio;
    float2 ViewPortPosition;
}

struct PS_INPUT
{
    float4 Position : SV_POSITION; // 변환된 화면 좌표
    float4 Color : COLOR; // 버텍스 컬러
    float3 Normal : NORMAL; // 정규화된 노멀 벡터
    float2 TexCoord : TEXCOORD1; // UV 좌표
    float4 WorldPosition : POSITION; // 버텍스 위치
    float SceneDepth : TEXCOORD2; // 씬 뎁스
};

struct PS_OUTPUT
{
    float4 Color : SV_Target0;
};

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;
    
    output.Color = float4(Material.DiffuseColor, 1);
    
    return output;
}