Texture2D heightMap : register(t0);
SamplerState sampLinear : register(s0);

cbuffer camera : register(b3)
{
    float4x4 world[2];
    float4x4 view[2];
    float4x4 proj[2];
};

cbuffer drawer : register(b5)
{
    float4 gConst[32];
};


struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 wpos : TEXCOORD1;
};

cbuffer frameBuffer : register(b4)
{
    float4 time;
};

float waterNoise(float2 p)
{
    return frac(sin(dot(p, float2(12.9898, 78.233))) * 43758.5453);
}

float4 PS(PS_INPUT input) : SV_Target
{
    const float HEIGHT_SCALE = 1500.0;

    // получаем высоту и глубину рельефа
    float terrainHeight = heightMap.SampleLevel(sampLinear, input.uv, 0).r ;
    // нижн€€ граница рельефа (дно)
    float height = terrainHeight * HEIGHT_SCALE;
    // глубина воды над рельефом
    float depth = saturate((input.wpos.z - height)/70 );
    
    // базовый цвет воды
    float3 waterColor = float3(0.07, 0.11, 0.13);
    
    // шум и блики
    float2 noiseUV = input.wpos.xy / 20.0 + time.x * 0.1;
    float noiseValue = waterNoise(floor(noiseUV * 10.0) / 10.0);
    waterColor *= (0.9 + noiseValue * 0.2);
    
    float specular = pow(noiseValue, 4.0) * 0.1;
    waterColor += float3(specular * 0.5, specular * 0.7, specular);
    
    // прозрачность по глубине
    float alpha = depth*0.8 ;
    
    return float4(waterColor, alpha);
}