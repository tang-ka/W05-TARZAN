#define Max_Fireball 300

Texture2D Textures : register(t0);
SamplerState Sampler : register(s0);

cbuffer MatrixConstants : register(b0)
{
    row_major float4x4 MVP;
    row_major float4x4 MInverseTranspose;
    float4 UUID;
    bool isSelected;
    float3 MatrixPad0;
};

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

cbuffer MaterialConstants : register(b1)
{
    FMaterial Material;
}

cbuffer LightingConstants : register(b2)
{
    float3 LightDirection; // 조명 방향 (단위 벡터; 빛이 들어오는 방향의 반대 사용)
    float LightPad0; // 16바이트 정렬용 패딩
    float3 LightColor; // 조명 색상 (예: (1, 1, 1))
    float LightPad1; // 16바이트 정렬용 패딩
    float AmbientFactor; // ambient 계수 (예: 0.1)
    float3 LightPad2; // 16바이트 정렬 맞춤 추가 패딩
};

cbuffer FlagConstants : register(b3)
{
    bool IsLit;
    float2 flagPad0;
}

cbuffer SubMeshConstants : register(b4)
{
    bool IsSelectedSubMesh;
    float3 SubMeshPad0;
}

cbuffer TextureConstants : register(b5)
{
    float2 UVOffset;
    float2 TexturePad0;
}

struct FireballConstants
{
    float3 FireballPosition;
    float FireballIndensity;
    float radius;
    float RadiusFallOff;
    float pad0;
    float pad1;
    float4 FireballColor;
};

cbuffer FireballBuffer : register(b6)
{
    FireballConstants Fireball[Max_Fireball];
    int FierballCount;
    float3 padding;
}

cbuffer FogConstants : register(b7)
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


float3 ComputeFireballLighting(float4 worldPos, float3 normal)
{
    float3 N = normalize(normal);
    float3 V = float3(0, 0, 1); 
    float3 totalLighting = float3(0, 0, 0);

    for (int i = 0; i < FierballCount; ++i)
    {
        float3 L = normalize(Fireball[i].FireballPosition - worldPos.xyz);
        float3 H = normalize(L + V);

        float dist = length(Fireball[i].FireballPosition - worldPos.xyz);
        float attenuation = saturate(1.0 - pow(dist / Fireball[i].radius, Fireball[i].RadiusFallOff));
        attenuation *= Fireball[i].FireballIndensity;

        float diffuse = saturate(dot(N, L));
        float specular = pow(saturate(dot(N, H)), Material.SpecularScalar * 32) * Material.SpecularScalar;

        float3 diffuseColor = diffuse * Fireball[i].FireballColor.rgb * attenuation;
        float3 specularColor = specular * Material.SpecularColor * Fireball[i].FireballColor.rgb * attenuation;

        totalLighting += diffuseColor + specularColor;
    }
    return totalLighting;
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
    float heightFactor = saturate(exp(-heightDiff * FogHeightFalloff)); // 0~1
    
    float fogFactor =  heightFactor * disFactor;

    // 밀도 곱
    fogFactor *= FogDensity;

    // 최대 불투명도 제한
   fogFactor = min(fogFactor, FogMaxOpacity);

    return fogFactor;
}



struct PS_INPUT
{
    float4 position : SV_POSITION; // 변환된 화면 좌표
    float4 color : COLOR; // 전달할 색상
    float3 normal : NORMAL; // 정규화된 노멀 벡터
    bool normalFlag : TEXCOORD0; // 노멀 유효성 플래그 (1.0: 유효, 0.0: 무효)
    float2 texcoord : TEXCOORD1;
    int materialIndex : MATERIAL_INDEX;
    float4 Worldposition : POSITION; // 버텍스 위치
};

struct PS_OUTPUT
{
    float4 color : SV_Target0;
    float4 UUID : SV_Target1;
};

float noise(float3 p)
{
    return frac(sin(dot(p, float3(12.9898, 78.233, 37.719))) * 43758.5453);
}

float4 PaperTexture(float3 originalColor)
{
    // 입력 색상을 0~1 범위로 제한
    float3 color = saturate(originalColor);
    
    float3 paperColor = float3(0.95, 0.95, 0.95);
    float blendFactor = 0.5;
    float3 mixedColor = lerp(color, paperColor, blendFactor);
    
    // 정적 grain 효과
    float grain = noise(color * 10.0) * 0.1;
    
    // 거친 질감 효과: 두 단계의 노이즈 레이어를 결합
    float rough1 = (noise(color * 20.0) - 0.5) * 0.15; // 첫 번째 레이어: 큰 규모의 노이즈
    float rough2 = (noise(color * 40.0) - 0.5) * 0.01; // 두 번째 레이어: 세부 질감 추가
    float rough = rough1 + rough2;
    
    // vignette 효과: 중앙에서 멀어질수록 어두워지는 효과
    float vignetting = smoothstep(0.4, 1.0, length(color.xy - 0.5) * 2.0);
    
    // 최종 색상 계산
    float3 finalColor = mixedColor + grain + rough - vignetting * 0.1;
    return float4(saturate(finalColor), 1.0);
}

float3 ComputeVolumetricFog(float3 worldPos)
{
    float3 rayDir = normalize(worldPos - CameraPosition);
    float stepLength = 1.0; // 단위 거리당 샘플링
    int numSteps = 16;

    float3 fogColor = FogColor.rgb;
    float3 accumFog = float3(0, 0, 0);
    float transmittance = 1.0;

    for (int i = 0; i < numSteps; ++i)
    {
        float dist = (i + 1) * stepLength;
        float3 samplePos = CameraPosition + rayDir * dist;

        // 높이에 따른 밀도 (height fog 요소 포함)
        float heightDiff = max(samplePos.z - FogHeight, 0.0);
        float heightFactor = exp(-heightDiff * FogHeightFalloff);

        float fogDensity = FogDensity * heightFactor;

        // 라이트 방향 기반 산란 간단 표현 (볼륨 안에서 라이트 쪽을 더 밝게)
        float lightIntensity = saturate(dot(normalize(LightDirection), -rayDir));

        float3 sampleFog = fogColor * fogDensity * lightIntensity;

        accumFog += sampleFog * transmittance * stepLength;
        transmittance *= exp(-fogDensity * stepLength);
    }

    return accumFog;
}


PS_OUTPUT mainPS(PS_INPUT input)
{
    PS_OUTPUT output;
    
    output.UUID = UUID;
    
    float3 texColor = Textures.Sample(Sampler, input.texcoord + UVOffset);
    float3 color;
    bool isEmptyPixel = false;
    if (texColor.g == 0) // TODO: boolean으로 변경
    {
        color = saturate(Material.DiffuseColor);
        isEmptyPixel = true;
    }
    else
    {
        color = texColor + Material.DiffuseColor;
    }
    
    if (isSelected)
    {
        color += float3(0.2f, 0.2f, 0.0f); // 노란색 틴트로 하이라이트
        if (IsSelectedSubMesh)
            color = float3(1, 1, 1);
    }
    
    float fogFactor = ComputeFogFactor(input.Worldposition.xyz);

    float3 volumetricFog = ComputeVolumetricFog(input.Worldposition.xyz);

  
    if (IsLit == 1)
    {
        if (input.normalFlag > 0.5)
        {
            float3 N = normalize(input.normal);
            float3 L = normalize(LightDirection);
            float3 V = float3(0, 0, 1);
            float3 H = normalize(L + V);

            float diffuse = saturate(dot(N, L));
            float specular = pow(saturate(dot(N, H)), Material.SpecularScalar * 32) * Material.SpecularScalar;

            float3 ambient = Material.AmbientColor * AmbientFactor;
            float3 diffuseLight = diffuse * LightColor;
            float3 specularLight = specular * Material.SpecularColor * LightColor;

            color = ambient + (diffuseLight * color) + specularLight;
            
        }
        float3 fireballLighting = ComputeFireballLighting(input.Worldposition, input.normal);
        color += fireballLighting;
        color += Material.EmissiveColor;
       // 기존 Fog 처리 후
        color = lerp(FogColor.rgb, color, 1.0 - fogFactor);

// Volumetric Fog를 추가로 더해주기
        color += volumetricFog;

        output.color = float4(color, Material.TransparencyScalar);
        return output;
    }
    else // unlit 상태일 때
    {
        if (input.normalFlag < 0.5)
        {
            // 기존 Fog 처리 후
            color = lerp(FogColor.rgb, color, 1.0 - fogFactor);

// Volumetric Fog를 추가로 더해주기
            color += volumetricFog;


            output.color = float4(color, Material.TransparencyScalar);
            return output;
        }

     
        // 기존 Fog 처리 후
        color = lerp(FogColor.rgb, color, 1.0 - fogFactor);

// Volumetric Fog를 추가로 더해주기
        color += volumetricFog;


        output.color = float4(color, Material.TransparencyScalar);
        return output;
    }

}