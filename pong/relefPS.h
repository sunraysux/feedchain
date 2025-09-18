struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float height : TEXCOORD1; // добавляем
    float2 wpos : TEXCOORD2;
};

// ==== палитра по высоте ====
float3 heightPalette(float h)
{
    // Базовые цвета с примесью грязно-зеленого
    float3 deepWater = float3(0.08, 0.16, 0.22);      // Глубокая вода с зеленоватым оттенком
    float3 shallowWater = float3(0.22, 0.3, 0.4);     // Мелководье с зеленоватым оттенком
    float3 wetSand = float3(0.5, 0.48, 0.35);         // Мокрый песок с зеленоватым оттенком
    float3 drySand = float3(0.55, 0.52, 0.35);        // Сухой песок с зеленоватым оттенком
    float3 grass = float3(0.15, 0.35, 0.1);           // Трава с более грязным зеленым оттенком
    float3 rock = float3(0.35, 0.33, 0.25);           // Скалы с зеленоватым оттенком
    float3 snow = float3(0.65, 0.68, 0.6);            // Снег с легким зеленоватым оттенком

    float3 c;

    if (h < 0.3)
        c = deepWater;
    else if (h < 0.38)
        c = lerp(deepWater, shallowWater, smoothstep(0.5, 0.58, h));
    else if (h < 0.4)
        c = lerp(shallowWater, wetSand, smoothstep(0.58, 0.6, h));
    else if (h < 0.43)
        c = lerp(wetSand, drySand, smoothstep(0.6, 0.63, h));
    else if (h < 0.48)
        c = lerp(drySand, grass, smoothstep(0.63, 0.68, h));
    else if (h < 0.65)
        c = grass;
    else if (h < 0.75)
        c = lerp(grass, rock, smoothstep(0.75, 0.85, h));
    else if (h < 0.8)
        c = lerp(rock, snow, smoothstep(0.85, 0.9, h));
    else
        c = snow;

    return c;
}

// ==== псевдо-шум ====
float hash21(float2 p)
{
    // быстрый детерминированный шум
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

// ==== основной пиксельный шейдер ====
float4 PS(VS_OUTPUT input) : SV_Target
{
    float h = input.height; // [0..1]

// используем глобальные координаты для шума
float2 worldPos = input.wpos.xy; // до проекции можно пробросить p.xy из VS
float n = noise(worldPos * 0.05) * 0.08 - 0.04;
h = saturate(h + n);

float3 color = heightPalette(h-0.09);

// вариация внутри биома (также от глобальных координат)
float biomeNoise = noise(worldPos * 0.12);
color *= (0.9 + biomeNoise * 0.2);

return float4(color, 1.0f);
}