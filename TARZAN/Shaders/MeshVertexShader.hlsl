// MatrixBuffer: 변환 행렬 관리
cbuffer MatrixConstants : register(b0)
{
    row_major float4x4 MVP;
    row_major float4x4 ModelMatrix;
    row_major float4x4 MInverseTranspose;
    float4 UUID;
    bool isSelected;
    float3 CameraPosition;
    float2 ViewPortSize;
    float2 ScreenSize;
};

struct VS_INPUT
{
    float4 position : POSITION; // 버텍스 위치
    float4 color : COLOR; // 버텍스 색상
    float3 normal : NORMAL; // 버텍스 노멀
    float2 texcoord : TEXCOORD;
    int materialIndex : MATERIAL_INDEX;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION; // 변환된 화면 좌표
    float4 Color : COLOR; // 버텍스 컬러
    float3 Normal : NORMAL; // 정규화된 노멀 벡터
    float2 TexCoord : TEXCOORD1; // UV 좌표
    float4 WorldPosition : POSITION; // 버텍스 위치
    float SceneDepth : TEXCOORD2; // 씬 뎁스
};

float LinearNormalizeDepth(float z_ndc, float nearZ, float farZ)
{
    float LinearValue = nearZ * farZ / (farZ - z_ndc * (farZ - nearZ));
    return saturate((LinearValue - nearZ) / (farZ - nearZ));
}

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    
    float4 worldPos = mul(float4(input.position.xyz, 1.0f), ModelMatrix);
    output.WorldPosition = worldPos;
    
    float4 clipPos = mul(float4(input.position.xyz, 1.0f), MVP);
    output.Position = clipPos;
    
    //float depth = length(CameraPosition - worldPos.xyz);
    float depth = clipPos.z / clipPos.w;
    output.SceneDepth = LinearNormalizeDepth(depth, 0.1, 1000);
    
    float3 worldNormal = mul(input.normal, (float3x3)MInverseTranspose);
    output.Normal = normalize(worldNormal);
    
    output.TexCoord = input.texcoord;
    
    return output;
}