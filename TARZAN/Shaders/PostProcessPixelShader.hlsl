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
    
    if (isValid == 0.5f)
    {
        fogFactor = ComputeFogFactor(worldPos);
        float3 fogColor = lerp(FogColor.rgb, color.rgb, 1.0 - fogFactor);
        output.Color = float4(fogColor, color.a);
    }
    else
    {
        output.Color = color;
    }
    
    
    return output;
}