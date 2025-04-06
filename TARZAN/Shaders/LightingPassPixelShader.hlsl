Texture2D g_GBufferNormal : register(t0);
Texture2D g_GBufferAlbedo : register(t1);
Texture2D g_GBufferAmbient : register(t2);
Texture2D g_GBufferPosition : register(t3);

SamplerState g_sampler : register(s0);

struct FLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Emissive;
    float Padding1;
    float3 Direction;
    float Padding2;
};

//struct FMaterial
//{
//    float4 Diffuse;
//    float4 Ambient;
//    float4 Specular;
//    float3 Emissive;
//    float Roughness;
//};

cbuffer LightConstants : register(b0)
{
    FLight GlobalLight;
    float3 CameraPosition;
    float Padding;
};

//cbuffer MaterialConstants : register(b1)
//{
//    FMaterial Material;
//};

struct PS_Input
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

struct PS_OUTPUT
{
    float4 color : SV_Target0;
};

float4 ComputeDirectionalLight(float3 normal, float3 worldPosition, float3 albedo, float3 ambient)
{
    // 각 광원 요소에 대한 색상을 초기화
    float4 ambientColor = 0;
    float4 diffuseColor = 0;
    float4 specularColor = 0;
    float4 emissiveColor = 0;
    
    float3 lightDir = normalize(-GlobalLight.Direction);
    normal = normalize(normal);
    
    // === Ambient ===
    ambientColor = float4(albedo, 1.0f) * GlobalLight.Ambient * float4(ambient, 1.0f);
    
    // === Diffuse ===
    float NdotL = saturate(dot(normal, lightDir));
    //float diffuseBoost = lerp(1.0f, 1.3f, 1.0f - Material.Roughness);
    diffuseColor = float4(albedo, 1.0f) * GlobalLight.Diffuse * NdotL /** diffuseBoost*/;
    
    return ambientColor + diffuseColor;
    
    //// === Specular ===
    //float3 viewDir = normalize(CameraPosition - worldPosition);
    //float3 reflectDir = reflect(-lightDir, normal);
    //float specFactor = saturate(dot(reflectDir, viewDir));
    ////float shininess = lerp(64.0f, 1.0f, 1.0f - saturate(Material.Roughness));
    //float shininess = lerp(64.0f, 1.0f, 1.0f - Material.Roughness);
    //float specular = pow(specFactor, shininess);
    //specular *= lerp(1.0f, 0.1f, pow(1.0f - Material.Roughness, 2.0f));
    //specularColor = GlobalLight.Specular * Material.Specular * specular;
    
    //// === Emissive ===
    //float viewDot = saturate(dot(viewDir, normal));
    //float emissive = pow(smoothstep(0.0f, 1.0f, 1.0f - viewDot), 2.0f);
    //emissiveColor = float4(GlobalLight.Emissive * Material.Emissive * emissive, 1.0f);

    //return ambientColor + diffuseColor + specularColor + emissiveColor;
}

PS_OUTPUT main(PS_Input input)
{
    PS_OUTPUT output;
    
    float2 uv = input.TexCoord;
    
    float3 normal = g_GBufferNormal.Sample(g_sampler, uv).xyz;
    float3 albedo = g_GBufferAlbedo.Sample(g_sampler, uv).rgb;
    float3 ambient = g_GBufferAmbient.Sample(g_sampler, uv).rgb;
    float3 worldPos = g_GBufferPosition.Sample(g_sampler, uv).xyz;

    //if (length(worldPos) == 0)
    //    output.color = float4(1,0,0, 1); //ComputeDirectionalLight(normal, worldPos, albedo, ambient);
    //else
    //    output.color = float4(0, 1, 0, 1);
    
    output.color = float4(albedo.xyz, 1);
    
    //output.color = float4(1, 1, 1, 1);
    //output.color = float4(input.Position.xyz, 1);
    return output;
}