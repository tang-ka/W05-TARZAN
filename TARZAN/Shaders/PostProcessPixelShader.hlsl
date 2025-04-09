cbuffer FogConstants : register(b0)
{
    float FogDensity;
    float FogHeightFalloff;
    float FogStartDistance;
    float FogCutoffDistance;
    float FogMaxOpacity;
    float3 FogPad0; 
    float4 FogColor;
    float3 CameraPosition;
    float FogHeight; 
    row_major float4x4 InverseView;
    row_major float4x4 InverseProjection;
    float DisableFog;
    float3 padding;
};

Texture2D LightColor : register(t0);
Texture2D LightPos : register(t1);

SamplerState Sampler : register(s0);

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

struct PS_OUTPUT
{
    float4 Color : SV_Target0;
};

float3 ReconstructWorldPos(float2 UV, float Depth)
{
    float4 NDC;
    NDC.xy = UV * 2.0 - 1.0; // [0,1] → [-1,1]
    NDC.y *= -1;
    NDC.z = Depth;
    NDC.w = 1.0;

    float4 WorldPos = mul(NDC, InverseProjection);
    WorldPos /= WorldPos.w;

    WorldPos = mul(WorldPos, InverseView);

    return WorldPos.xyz;
}


float ComputeFogFactor(float3 worldPos)
{
    float dist = distance(CameraPosition, worldPos);

    // 거리 기반
    float fogRange = FogCutoffDistance - FogStartDistance;
    float disFactor = saturate((dist - FogStartDistance) / fogRange); // 0~1  50일떄 0

    // 높이 기반 (지수 감쇠)
    float heightDiff = worldPos.z - FogHeight;
    heightDiff = max(heightDiff, 0.0);
    float heightFactor = saturate(exp(-heightDiff * FogHeightFalloff * FogDensity)); // 0~1
    
    float fogFactor = heightFactor * disFactor;

    // 최대 불투명도 제한
    fogFactor = min(fogFactor, FogMaxOpacity);

    return fogFactor;
}

PS_OUTPUT mainPS(PS_INPUT input) : SV_TARGET
{
    PS_OUTPUT output;

    float4 color = LightColor.Sample(Sampler, input.TexCoord);
    float4 worldPosTex = LightPos.Sample(Sampler, input.TexCoord);
    float3 worldPos = worldPosTex.xyz;
    float isValid = worldPosTex.w;

    float fogFactor;
    float3 fogColor;
    if (DisableFog < 0.5f)
    {
        if (isValid == 0.5f)
        {
            fogFactor = ComputeFogFactor(worldPos.xyz);
        }
        else //배경
        {
            float3 worldPosH = ReconstructWorldPos(input.TexCoord, 1);
            fogFactor = ComputeFogFactor(worldPosH.xyz);
        }
         fogColor = lerp(FogColor.rgb, color.rgb, fogFactor);
    }
    else
    {
        fogFactor = 0.0f;
        fogColor = color.rgb;
    }
    
    output.Color = float4(fogColor, color.a);
    return output;
}

