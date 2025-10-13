struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float height : TEXCOORD1;
    float2 wpos : TEXCOORD2;
};

float3 heightPalette(float h)
{
    // оПХЦКСЬ╦ММШЕ, "ЦПЪГМШЕ" ЖБЕРЮ Я ГЕК╦МШЛ НРРЕМЙНЛ
    float3 deepWater = float3(0.08, 0.12, 0.14);     // р╦ЛМЮЪ ЦКСАНЙЮЪ БНДЮ
    float3 shallowWater = float3(0.15, 0.22, 0.25);  // лЕКЙНБНДЭЕ
    float3 mud = float3(0.3, 0.28, 0.22);            // цПЪГЭ/ХК
    float3 grass = float3(0.40, 0.47, 0.28);         // оПХЦКСЬ╦ММЮЪ ЦПЪГМН-ГЕК╦МЮЪ РПЮБЮ
    float3 darkGrass = float3(0.35, 0.38, 0.25);     // р╦ЛМЮЪ РПЮБЮ

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

float fbm(float2 p, float frequency, int octaves)
{
    float value = 0.0;
    float amplitude = 0.5;
    float gain = 0.5;
    float lacunarity = 2.0;

    for (int i = 0; i < octaves; i++)
    {
        value += amplitude * noise(p * frequency);
        amplitude *= gain;
        frequency *= lacunarity;
    }

    return value;
}

// пЮГМШЕ РХОШ ЬСЛЮ ДКЪ БЮПХЮРХБМНЯРХ
float ridgeNoise(float2 p, float frequency, int octaves)
{
    float value = 0.0;
    float amplitude = 0.5;
    float gain = 0.5;
    float lacunarity = 2.0;

    for (int i = 0; i < octaves; i++)
    {
        float n = 1.0 - abs(noise(p * frequency));
        n = n * n; // ridge
        value += n * amplitude;
        amplitude *= gain;
        frequency *= lacunarity;
    }

    return value;
}

float domainWarpNoise(float2 p)
{
    // Domain warping ДКЪ ЯКНФМШУ ОЮРРЕПМНБ
    float2 q = float2(
        fbm(p + float2(0.0, 0.0), 0.1, 2),
        fbm(p + float2(5.2, 1.3), 0.1, 2)
    );

    float2 r = float2(
        fbm(p + 4.0 * q + float2(1.7, 9.2), 0.1, 2),
        fbm(p + 4.0 * q + float2(8.3, 2.8), 0.1, 2)
    );

    return fbm(p + 4.0 * r, 0.1, 2);
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float h = input.height;
    float2 worldPos = input.wpos;

    // анкее хмрепеямше ялеыемхъ х вюярнрш
    float2 noisePos1 = worldPos * 0.0003 + float2(12345.67, 89012.34);
    float2 noisePos2 = worldPos * 0.0009 + float2(56789.01, 23456.78);
    float2 noisePos3 = worldPos * 0.0027 + float2(90123.45, 67890.12);
    float2 noisePos4 = worldPos * 0.0081 + float2(34567.89, 12345.67);
    float2 noisePos5 = worldPos * 0.0243 + float2(78901.23, 45678.90);

    // пюгмннапюгмше рхош ьслю я анкэьеи дерюкхгюжхеи
    float largeFeatures = fbm(noisePos1, 0.8, 4) * 0.6;
    float mediumDetails = ridgeNoise(noisePos2, 2.5, 3) * 0.4;
    float smallDetails = fbm(noisePos3, 7.2, 4) * 0.3;
    float microDetails = fbm(noisePos4, 21.6, 3) * 0.2;

    // DOMAIN WARP дкъ якнфмшу оюррепмнб
    float complexPattern = domainWarpNoise(noisePos5 * 0.3) * 0.4;

    // хепюпухвеяйне йнлахмхпнбюмхе ьслнб
    float combinedNoise = (largeFeatures * 0.3 + mediumDetails * 0.25 +
                          smallDetails * 0.2 + microDetails * 0.15 +
                          complexPattern * 0.1);

    // анкее гмювхрекэмне бкхъмхе мю бшянрс
    float heightVariation = (fbm(noisePos2 * 1.7, 3.1, 3) * 0.15 - 0.075) +
                           (ridgeNoise(noisePos4 * 0.8, 5.2, 2) * 0.1);

    h = saturate(h + heightVariation + combinedNoise * 0.12);

    // аюгнбши жбер я анкее ъпйхлх оепеундюлх
    float3 color = heightPalette(h);

    // пюгмше рейярспш дкъ пюгмшу ахнлнб
    float waterDetail = fbm(noisePos3 * 2.0, 12.0, 3) * 0.25;
    float sandDetail = ridgeNoise(noisePos4 * 1.5, 8.0, 2) * 0.35;
    float grassDetail = fbm(noisePos5 * 0.8, 6.0, 4) * 0.4;
    float mountainDetail = ridgeNoise(noisePos2 * 0.5, 4.0, 3) * 0.5;

    // окюбмше оепеундш лефдс рейярспюлх
    float waterMask = smoothstep(0.25, 0.35, h);
    float sandMask = smoothstep(0.35, 0.45, h);
    float grassMask = smoothstep(0.45, 0.65, h);
    float mountainMask = smoothstep(0.65, 0.75, h);

    float textureNoise = waterDetail;
    textureNoise = lerp(textureNoise, sandDetail, sandMask);
    textureNoise = lerp(textureNoise, grassDetail, grassMask);
    textureNoise = lerp(textureNoise, mountainDetail, mountainMask);

    // анкее бшпюфеммше рейярспмше щттейрш
    color *= (0.8 + textureNoise * 0.4);

    // жбернбше бюпхюжхх дкъ еяреярбеммнярх
    float colorVariationR = fbm(noisePos1 * 1.3, 2.7, 2) * 0.1;
    float colorVariationG = fbm(noisePos2 * 1.1, 3.3, 2) * 0.08;
    float colorVariationB = fbm(noisePos3 * 0.9, 4.1, 2) * 0.06;

    float3 colorShift = float3(colorVariationR, colorVariationG, colorVariationB);
    color += colorShift * 0.15;

    // щттейр щпнгхх х еяреярбеммшу опнжеяянб
    float erosionEffect = 0.7 + combinedNoise * 0.6;
    color *= erosionEffect;

    // днаюбкъел мелмнцн йнмрпюярю
    color = pow(color, 0.9);

    // кецйюъ ъпйнярэ дкъ анкее фхбнцн бхдю
    color *= 1.1;

    return float4(saturate(color), 1.0f);
}