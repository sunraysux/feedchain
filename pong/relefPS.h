struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float height : TEXCOORD1;
    float2 wpos : TEXCOORD2;
};

// ==== палитра по высоте ====
float3 heightPalette(float h)
{
    // Приглушённые, "грязные" цвета с зелёным оттенком
    float3 deepWater = float3(0.08, 0.12, 0.14);     // Тёмная глубокая вода
    float3 shallowWater = float3(0.15, 0.22, 0.25);  // Мелководье
    float3 mud = float3(0.3, 0.28, 0.22);            // Грязь/ил
    float3 grass = float3(0.40, 0.42, 0.28);         // Приглушённая грязно-зелёная трава
    float3 darkGrass = float3(0.35, 0.38, 0.25);      // Тёмная трава

    float3 c;

    if (h < 0.35)
        c = lerp(deepWater, shallowWater, smoothstep(0.0, 0.35, h));
    else if (h < 0.45)
        c = lerp(shallowWater, mud, smoothstep(0.35, 0.45, h));
    else if (h < 0.55)
        c = lerp(mud, darkGrass, smoothstep(0.45, 0.55, h));
    else if (h < 0.65)
        c = lerp(darkGrass, grass, smoothstep(0.55, 0.65, h));
    else 
        c = grass;

    return c;
}

float hash21(float2 p)
{
    p = frac(p * float2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return frac(p.x * p.y);
}

float noise(float2 p)
{
    float2 i = floor(p);
    float2 f = frac(p);

    float a = hash21(i);
    float b = hash21(i + float2(1, 0));
    float c = hash21(i + float2(0, 1));
    float d = hash21(i + float2(1, 1));

    float2 u = f * f * (3.0 - 2.0 * f);

    return lerp(lerp(a, b, u.x), lerp(c, d, u.x), u.y);
}

float fbm(float2 p)
{
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 0.05;

    for (int i = 0; i < 4; i++)
    {
        value += amplitude * noise(p * frequency);
        amplitude *= 0.5;
        frequency *= 2.0;
    }

    return value;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float h = input.height;
    float2 worldPos = input.wpos.xy;

    float erosion = fbm(worldPos * 0.3);
    float n = fbm(worldPos * 0.8 + erosion * 0.5) * 0.1 - 0.05;
    h = saturate(h + n);

    float3 color = heightPalette(h);

    float erosionEffect = 0.9 + erosion * 0.2;
    color *= erosionEffect;

    float textureNoise;
    if (h < 0.4)
        textureNoise = fbm(worldPos * 1.5) * 0.1; 
    else if (h < 0.6)
        textureNoise = fbm(worldPos * 4.0) * 0.15; 
    else
        textureNoise = fbm(worldPos * 8.0) * 0.2; 
    
    color *= (0.85 + textureNoise);

    return float4(color, 1.0f);
}