Texture2D heightMap : register(t0);
SamplerState sampLinear : register(s0);

cbuffer frameBuffer : register(b4)
{
    float4 time;
    float4 aspect;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float height : TEXCOORD1;
    float3 wpos : TEXCOORD2;
};

// Простая функция шума для воды
float waterNoise(float2 p)
{
    return frac(sin(dot(p, float2(12.9898, 78.233))) * 43758.5453);
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float scale = 1024.0;
    float2 uv = (input.wpos.xy + float2(scale, scale)) / (2.0 * scale);
    float terrainHeight = heightMap.SampleLevel(sampLinear, uv / 4, 0).r;

    float depth = input.height - terrainHeight;

    // Приглушенные цвета воды
    float3 waterBaseColor = float3(0.07, 0.11, 0.13);

    // Шумовая текстура для воды
    float2 noiseUV = input.wpos.xy / 20.0 + time.x * 0.1;
    float noiseValue = waterNoise(floor(noiseUV * 10.0) / 10.0);

    // Добавляем небольшие вариации цвета на основе шума
    float3 waterColor = waterBaseColor * (0.9 + noiseValue * 0.2);

    // Прозрачность зависит от глубины
    float alpha = saturate(depth * 8.0) * 0.8;

    // Добавляем небольшие блики на основе шума
    float specular = pow(noiseValue, 4.0) * 0.1;
    waterColor += float3(specular * 0.5, specular * 0.7, specular);

    return float4(waterColor, alpha);
}