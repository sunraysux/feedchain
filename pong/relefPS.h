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
    float3 low = float3(0.05, 0.25, 0.05);   // трава (зелёный)
    float3 mid = float3(0.45, 0.35, 0.2);    // камень (коричневатый)
    float3 high = float3(0.9, 0.9, 0.95);     // снег (почти белый)

    float3 c;

    if (h < 0.4)
        c = lerp(low, mid, smoothstep(0.2, 0.4, h));
    else
        c = lerp(mid, high, smoothstep(0.4, 0.9, h));

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

float3 color = heightPalette(h);

// вариация внутри биома (также от глобальных координат)
float biomeNoise = noise(worldPos * 0.12);
color *= (0.9 + biomeNoise * 0.2);

return float4(color, 1.0f);
}