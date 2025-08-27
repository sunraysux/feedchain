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
    float4 gConst[32]; // x = gridX, y = gridY
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float height : TEXCOORD1;
    float2 wpos : TEXCOORD2;
};

VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    int gridX = gConst[0].x;
    int gridY = gConst[0].y;
    int base_rangex = gConst[1].x;
    int base_rangey = gConst[1].y;

    int quadID = vID / 6;
    int localVertex = vID % 6;

    int x = quadID % gridX;
    int y = quadID / gridX;

    float2 offset;
    if (localVertex == 0) offset = float2(0, 0);
    else if (localVertex == 1) offset = float2(1, 0);
    else if (localVertex == 2) offset = float2(0, 1);
    else if (localVertex == 3) offset = float2(1, 0);
    else if (localVertex == 4) offset = float2(1, 1);
    else offset = float2(0, 1);

    float CHUNK_SIZE = 8.0;

    // базовая позиция квадрата
    float2 p = float2(-base_rangex + x * CHUNK_SIZE,
        -base_rangey + y * CHUNK_SIZE)
        + offset * CHUNK_SIZE;

    // --- смещение инстанса ---
    // Например, рисуем 3x3 копии вокруг центра
    int tilesPerSide = 3;
    int instX = iID % tilesPerSide - tilesPerSide / 2;
    int instY = iID / tilesPerSide - tilesPerSide / 2;

    // сдвигаем на размер всего "мира" (gridX * CHUNK_SIZE)
    p += float2(instX * gridX * CHUNK_SIZE,
        instY * gridY * CHUNK_SIZE);

    // uv заворачиваем (тайлим)
    float2 uv = frac(p / float2(base_rangex, base_rangey) * 0.5 + 0.5);

    float4 pos = float4(p, 0, 1);

    // высота
    float height = heightMap.SampleLevel(sampLinear, uv, 0).r;
    float heightScale = 100;
    pos.z += height * heightScale;
    output.wpos = pos.xy; // пробрасываем мировые XY отдельно
    output.pos = mul(pos, mul(view[0], proj[0]));
    output.uv = uv;
    output.height = height;
    return output;
}