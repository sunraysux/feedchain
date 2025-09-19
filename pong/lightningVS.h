cbuffer global : register(b5)
{
    float4 gConst[32]; // x, y = центр квадрата, z = высота, w = seed
};

cbuffer camera : register(b3)
{
    float4x4 world[2];
    float4x4 view[2];
    float4x4 proj[2];
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
};

// Функция простого шума для смещения молнии
float randomOffset(float2 p, float seed)
{
    return frac(sin(dot(p.xy + seed, float2(12.9898, 78.233))) * 43758.5453) * 0.5 - 0.25;
}

VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    float scale = 50.0;              // масштаб квадрата
    float lightningHeight = gConst[0].z+100;   // высота молнии сверху

    // Начальная и конечная точка молнии
    float3 start = float3(gConst[0].x, gConst[0].y, lightningHeight);
    float3 end = float3(gConst[0].x, gConst[0].y, gConst[0].z);

    // Разобьём молнию на 10 сегментов
    int segment = vID / 6; // каждые 6 вершин = 1 сегмент (2 треугольника)
    float t0 = segment / 10.0;
    float t1 = (segment + 1) / 10.0;

    // Интерполяция по высоте
    float3 p0 = lerp(start, end, t0);
    float3 p1 = lerp(start, end, t1);

    // Добавим случайный смещённый X/Y
    float seedVal = gConst[0].w;
    p0.xy += float2(randomOffset(float2(segment, 0), seedVal), randomOffset(float2(segment, 1), seedVal)) * 20.0;
    p1.xy += float2(randomOffset(float2(segment + 1, 0), seedVal), randomOffset(float2(segment + 1, 1), seedVal)) * 20.0;

    // Ширина молнии
    float width = seedVal/20;
    float3 dir = normalize(p1 - p0);
    float3 right = float3(-dir.y, dir.x, 0.0); // перпендикуляр XY

    // Два треугольника сегмента
    float3 verts[6] = {
        p0 - right * width, p1 - right * width, p0 + right * width,
        p0 + right * width, p1 - right * width, p1 + right * width
    };

    output.pos = mul(float4(verts[vID % 6], 1.0), mul(view[0], proj[0]));

    return output;
}