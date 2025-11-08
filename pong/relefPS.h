struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float height : TEXCOORD1;
    float2 wpos : TEXCOORD2;
};


float3 desertPalette(float h)
{
    float3 deepWater = float3(0.08, 0.15, 0.25);
    float3 shallowWater = float3(0.15, 0.25, 0.35);
    float3 sand = float3(0.85, 0.75, 0.55);
    float3 dryGrass = float3(0.65, 0.58, 0.35);
    float3 rock = float3(0.45, 0.4, 0.35);
    float3 mountain = float3(0.35, 0.3, 0.25);
    float3 snow = float3(0.95, 0.95, 0.9);

    float normalized_h = h / 1500.0;

    if (normalized_h < 0.4)
        return lerp(deepWater, shallowWater, smoothstep(0.0, 0.4, normalized_h));
    else if (normalized_h < 0.45)
        return lerp(shallowWater, sand, smoothstep(0.4, 0.45, normalized_h));
    else if (normalized_h < 0.6)
        return lerp(sand, dryGrass, smoothstep(0.45, 0.6, normalized_h));
    else if (normalized_h < 0.75)
        return lerp(dryGrass, rock, smoothstep(0.6, 0.75, normalized_h));
    else if (normalized_h < 0.9)
        return lerp(rock, mountain, smoothstep(0.75, 0.9, normalized_h));
    else
        return lerp(mountain, snow, smoothstep(0.9, 1.0, normalized_h));
}

float3 arcticPalette(float h)
{
    float3 deepWater = float3(0.05, 0.1, 0.2);
    float3 shallowWater = float3(0.15, 0.25, 0.4);
    float3 ice = float3(0.7, 0.8, 0.9);
    float3 tundra = float3(0.4, 0.45, 0.5);
    float3 darkRock = float3(0.25, 0.25, 0.3);
    float3 mountain = float3(0.5, 0.55, 0.6);
    float3 snow = float3(0.95, 0.95, 1.0);

    float normalized_h = h / 1500.0;

    if (normalized_h < 0.4)
        return lerp(deepWater, shallowWater, smoothstep(0.0, 0.4, normalized_h));
    else if (normalized_h < 0.42)
        return lerp(shallowWater, ice, smoothstep(0.4, 0.42, normalized_h));
    else if (normalized_h < 0.5)
        return lerp(ice, tundra, smoothstep(0.42, 0.5, normalized_h));
    else if (normalized_h < 0.7)
        return lerp(tundra, darkRock, smoothstep(0.5, 0.7, normalized_h));
    else if (normalized_h < 0.85)
        return lerp(darkRock, mountain, smoothstep(0.7, 0.85, normalized_h));
    else
        return lerp(mountain, snow, smoothstep(0.85, 1.0, normalized_h));
}

float3 tropicalPalette(float h)
{
    float3 deepWater = float3(0.0, 0.1, 0.25);
    float3 shallowWater = float3(0.1, 0.4, 0.6);
    float3 beach = float3(0.9, 0.85, 0.6);
    float3 jungle = float3(0.15, 0.4, 0.2);
    float3 forest = float3(0.1, 0.35, 0.15);
    float3 mountain = float3(0.3, 0.35, 0.25);
    float3 cloud = float3(0.8, 0.85, 0.9);

    float normalized_h = h / 1500.0;

    if (normalized_h < 0.4)
        return lerp(deepWater, shallowWater, smoothstep(0.0, 0.4, normalized_h));
    else if (normalized_h < 0.42)
        return lerp(shallowWater, beach, smoothstep(0.4, 0.42, normalized_h));
    else if (normalized_h < 0.5)
        return lerp(beach, jungle, smoothstep(0.42, 0.5, normalized_h));
    else if (normalized_h < 0.7)
        return jungle;
    else if (normalized_h < 0.85)
        return lerp(jungle, mountain, smoothstep(0.7, 0.85, normalized_h));
    else
        return lerp(mountain, cloud, smoothstep(0.85, 1.0, normalized_h));
}

float3 volcanicPalette(float h)
{
    float3 deepWater = float3(0.05, 0.05, 0.1);
    float3 shallowWater = float3(0.1, 0.1, 0.2);
    float3 blackSand = float3(0.15, 0.15, 0.15);
    float3 darkRock = float3(0.25, 0.2, 0.15);
    float3 redRock = float3(0.4, 0.2, 0.1);
    float3 lava = float3(0.8, 0.3, 0.1);
    float3 magma = float3(0.9, 0.5, 0.2);

    float normalized_h = h / 1500.0;

    if (normalized_h < 0.4)
        return lerp(deepWater, shallowWater, smoothstep(0.0, 0.4, normalized_h));
    else if (normalized_h < 0.42)
        return lerp(shallowWater, blackSand, smoothstep(0.4, 0.42, normalized_h));
    else if (normalized_h < 0.5)
        return lerp(blackSand, darkRock, smoothstep(0.42, 0.5, normalized_h));
    else if (normalized_h < 0.7)
        return lerp(darkRock, redRock, smoothstep(0.5, 0.7, normalized_h));
    else if (normalized_h < 0.85)
        return lerp(redRock, lava, smoothstep(0.7, 0.85, normalized_h));
    else
        return lerp(lava, magma, smoothstep(0.85, 1.0, normalized_h));
}

float3 fantasyPalette(float h)
{
    float3 deepWater = float3(0.1, 0.05, 0.3);
    float3 shallowWater = float3(0.3, 0.2, 0.6);
    float3 purpleBeach = float3(0.6, 0.4, 0.8);
    float3 magicGrass = float3(0.3, 0.6, 0.2);
    float3 enchantedForest = float3(0.2, 0.4, 0.3);
    float3 crystalMountain = float3(0.5, 0.7, 0.9);
    float3 glowingPeak = float3(0.8, 0.9, 1.0);

    float normalized_h = h / 1500.0;

    if (normalized_h < 0.4)
        return lerp(deepWater, shallowWater, smoothstep(0.0, 0.4, normalized_h));
    else if (normalized_h < 0.42)
        return lerp(shallowWater, purpleBeach, smoothstep(0.4, 0.42, normalized_h));
    else if (normalized_h < 0.5)
        return lerp(purpleBeach, magicGrass, smoothstep(0.42, 0.5, normalized_h));
    else if (normalized_h < 0.7)
        return lerp(magicGrass, enchantedForest, smoothstep(0.5, 0.7, normalized_h));
    else if (normalized_h < 0.85)
        return lerp(enchantedForest, crystalMountain, smoothstep(0.7, 0.85, normalized_h));
    else
        return lerp(crystalMountain, glowingPeak, smoothstep(0.85, 1.0, normalized_h));
}

float3 realisticPalette(float h)
{
    float3 deepOcean = float3(0.02, 0.05, 0.1);
    float3 ocean = float3(0.1, 0.2, 0.3);
    float3 shallowWater = float3(0.2, 0.35, 0.45);
    float3 sand = float3(0.76, 0.7, 0.5);
    float3 grass = float3(0.35, 0.5, 0.2);
    float3 forest = float3(0.2, 0.35, 0.1);
    float3 rock = float3(0.4, 0.4, 0.35);
    float3 darkRock = float3(0.3, 0.3, 0.25);
    float3 snow = float3(0.95, 0.95, 0.95);

    float normalized_h = h / 1500.0;

    if (normalized_h < 0.35)
        return lerp(deepOcean, ocean, smoothstep(0.0, 0.35, normalized_h));
    else if (normalized_h < 0.4)
        return lerp(ocean, shallowWater, smoothstep(0.35, 0.4, normalized_h));
    else if (normalized_h < 0.42)
        return lerp(shallowWater, sand, smoothstep(0.4, 0.42, normalized_h));
    else if (normalized_h < 0.5)
        return lerp(sand, grass, smoothstep(0.42, 0.5, normalized_h));
    else if (normalized_h < 0.65)
        return lerp(grass, forest, smoothstep(0.5, 0.65, normalized_h));
    else if (normalized_h < 0.75)
        return lerp(forest, rock, smoothstep(0.65, 0.75, normalized_h));
    else if (normalized_h < 0.85)
        return lerp(rock, darkRock, smoothstep(0.75, 0.85, normalized_h));
    else if (normalized_h < 0.92)
        return lerp(darkRock, snow, smoothstep(0.85, 0.92, normalized_h));
    else
        return snow;
}
// ==== палитра по высоте ====
float3 heightPalette(float h)
{
    // Приглушённые, "грязные" цвета с зелёным оттенком
    float3 deepWater = float3(0.08, 0.12, 0.14);     // Тёмная глубокая вода
    float3 shallowWater = float3(0.15, 0.22, 0.25);  // Мелководье
    float3 mud = float3(0.3, 0.28, 0.22);            // Грязь/ил
    float3 grass = float3(0.40, 0.47, 0.28);         // Приглушённая грязно-зелёная трава
    float3 darkGrass = float3(0.35, 0.38, 0.25);     // Тёмная трава
    float3 mountain = float3(0.35, 0.4, 0.4);         // Горы
    float3 snow = float3(0.9, 0.9, 0.9);             // Снег

    float3 c;

    // Нормализуем высоту к диапазону 0-1 для удобства
    float normalized_h = h / 1500.0;

    if (normalized_h < 0.35)
        c = lerp(deepWater, shallowWater, smoothstep(0.0, 0.35, normalized_h));
    else if (normalized_h < 0.45)
        c = lerp(shallowWater, mud, smoothstep(0.35, 0.45, normalized_h));
    else if (normalized_h < 0.55)
        c = lerp(mud, darkGrass, smoothstep(0.45, 0.55, normalized_h));
    else if (normalized_h < 0.75)
        c = lerp(darkGrass, grass, smoothstep(0.55, 0.75, normalized_h));
    else if (normalized_h < 0.9)
        c = lerp(grass, mountain, smoothstep(0.75, 0.9, normalized_h));
    else
        c = lerp(mountain, snow, smoothstep(0.9, 1.0, normalized_h));

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
    // Нормализуем высоту для работы с шумом
    float normalized_height = input.height / 1500.0;
//return float4(normalized_height, normalized_height, normalized_height, 1.0f);
    float2 worldPos = input.wpos.xy;

    // Применяем шум к нормализованной высоте
    float erosion = fbm(worldPos * 0.3);
    float n = fbm(worldPos * 0.8 + erosion * 0.5) * 0.1 - 0.05;
    normalized_height = saturate(normalized_height + n);

    // Конвертируем обратно в абсолютные значения для палитры
    float absolute_height = input.height;
    float3 color = fantasyPalette(absolute_height);

   // // Эффекты эрозии
   // float erosionEffect = 0.9 + erosion * 0.2;
   // color *= erosionEffect;
   //
   // // Текстурный шум в зависимости от типа местности
   // float textureNoise;
   // if (normalized_height < 0.4) // Вода и низменности
   //     textureNoise = fbm(worldPos * 1.5) * 0.1;
   // else if (normalized_height < 0.6) // Равнины и холмы
   //     textureNoise = fbm(worldPos * 4.0) * 0.15;
   // else // Горы
   //     textureNoise = fbm(worldPos * 8.0) * 0.2;
   //
   // color *= (0.85 + textureNoise);
   //
    return float4(color, 1.0f);
}