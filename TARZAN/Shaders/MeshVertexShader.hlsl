// MatrixBuffer: 변환 행렬 관리
cbuffer MatrixConstants : register(b0)
{
    row_major float4x4 MVP;
    row_major float4x4 ModelMatrix;
    row_major float4x4 MInverseTranspose;
    float4 UUID;
    bool isSelected;
    float3 MatrixPad0;
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
    float3 Normal : NORMAL; // 정규화된 노멀 벡터
    float2 TexCoord : TEXCOORD1;
    float4 WorldPosition : POSITION; // 버텍스 위치
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    
    float4 worldPos = mul(float4(input.position.xyz, 1.0f), ModelMatrix);
    output.WorldPosition = worldPos;
    output.Position = mul(float4(input.position.xyz, 1.0f), MVP);
    
    float3 worldNormal = mul(input.normal, (float3x3)MInverseTranspose);
    output.Normal = normalize(worldNormal);
    
    output.TexCoord = input.texcoord;
    
    //// 위치 변환
    //output.position = mul(input.position, MVP);
    //output.color = input.color;
    //if (isSelected)
    //    output.color *= 0.5;
    //// 입력 normal 값의 길이 확인
    //float normalThreshold = 0.001;
    //float normalLen = length(input.normal);
    
    //if (normalLen < normalThreshold)
    //{
    //    output.normalFlag = 0.0;
    //}
    //else
    //{
    //    //output.normal = normalize(input.normal);
    //    output.normal = mul(input.normal, MInverseTranspose);
    //    output.normalFlag = 1.0;
    //}
    //output.texcoord = input.texcoord;
    //output.Worldposition = mul(input.position, Model);
    
    return output;
}