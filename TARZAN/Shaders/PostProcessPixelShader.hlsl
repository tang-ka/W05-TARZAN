cbuffer FogConstants : register(b0)
{
    float FogDensity;
    float FogHeightFalloff;
    float StartDistance;
    float FogCutoffDistance;
    float FogMaxOpacity;
    float3 padding; 
    float4 FogInscatteringColor;
    float3 CameraPosition;
    float padding1; 
};

Texture2D SceneColor : register(t0);
Texture2D SceneDepth : register(t1);

SamplerState SamplerColor : register(s0);
SamplerState SamplerDepth : register(s1);

struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
    float2 UV   : TEXCOORD;
};

float LinearizeDepth(float depth)
{
    // Depth 값을 0~1에서 선형 공간으로 변환
    float zNear = 0.1f;
    float zFar = 1000.0f;
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

float4 mainPS(VS_OUTPUT input) : SV_TARGET
{
    float3 sceneColor = SceneColor.Sample(SamplerColor, input.UV).rgb;
    float rawDepth = SceneDepth.Sample(SamplerDepth, input.UV).r;

    float linearDepth = LinearizeDepth(rawDepth);

    // 안개 강도 계산
    float fogFactor = saturate((linearDepth - StartDistance) * FogDensity);
    fogFactor = min(fogFactor, FogMaxOpacity);

    float3 finalColor = lerp(sceneColor, FogInscatteringColor.rgb, fogFactor);
    return float4(finalColor, 1.0);
}