// MeshPixelShader.hlsl

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

//cbuffer TextureConstants : register(b2)
//{
//    float2 UVOffset;
//    float2 Padding;
//};

Texture2D g_DiffuseMap : register(t0);
SamplerState g_sampler0 : register(s0);

struct PS_INPUT
{
    float4 Position : SV_POSITION; // 변환된 화면 좌표
    float3 Normal : NORMAL; // 정규화된 노멀 벡터
    float2 TexCoord : TEXCOORD1;
    float4 WorldPosition : POSITION; // 버텍스 위치
};

struct PS_OUTPUT
{
    float4 Normal : SV_Target0;
    float4 Albedo : SV_Target1;
    float4 Ambient : SV_Target2;
    float4 WorldPos : SV_Target3;
};

PS_OUTPUT main(PS_INPUT input)
{   
    PS_OUTPUT output;
    
    float3 normal = normalize(input.Normal);
    //output.Normal = float4(normal.xyz, 1); // TODO: WorldSpace에서의 Normal값으로 변경
    output.Normal = float4(normal * 0.5 + 0.5, 1.0); // Normal
    
    float2 uv = input.TexCoord/* + UVOffset*/;
    float4 diffuseColor = float4(Material.DiffuseColor, 1.0f);
    float4 textureColor = g_DiffuseMap.Sample(g_sampler0, float2(uv.x, uv.y));
    
    // Light값에 영향을 받지 않는 색상 (Diffuse -> Albedo)
    output.Albedo = diffuseColor * textureColor;
    output.Ambient = (Material.AmbientColor.xyz, 1.f);
    output.WorldPos = input.WorldPosition;
    
    //output.Albedo = float4(input.color.rgb, 1.0); // Albedo
    
    return output;
}


