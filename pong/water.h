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


float4 PS(VS_OUTPUT input) : SV_Target
{
    float scale = 1024.0;
float2 uv = (input.wpos.xy + float2(scale, scale)) / (2.0 * scale);
    float terrainHeight = heightMap.SampleLevel(sampLinear, uv / 4, 0).r;

    float depth = input.height-terrainHeight ;
   // if (depth < 0) discard;

    // Анимация ряби
    float2 movingUV = input.wpos.xy / 50.0 + time.x * 0.2;
    float ripple = sin(movingUV.x * 5.0 + time.x * 2.0) *
                   cos(movingUV.y * 4.0 + time.x * 1.5) * 0.1;

    // Цвет и прозрачность зависят от глубины
    float depthFactor = saturate(depth * 2.0);
    float3 waterColor = lerp(float3(0.0, 0.5, 0.9), float3(0.0, 0.2, 0.6), depthFactor);
    waterColor += ripple * 0.1; // Добавляем анимацию

    float alpha = saturate(depth * 5.0) * 0.9;

    return float4(waterColor, alpha);
}