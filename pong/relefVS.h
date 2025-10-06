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
    int centerChunkX = gConst[0].z; // центральный чанк камеры
    int centerChunkY = gConst[0].w;

    int quadID = vID / 6;
    int localVertex = vID % 6;

    int tileX = iID % 3 - 1; // -1, 0, 1
    int tileY = iID / 3 - 1; // -1, 0, 1

    int x = quadID % gridX;
    int y = quadID / gridX;

    float2 offset;
    if (localVertex == 0) offset = float2(0, 0);
    else if (localVertex == 1) offset = float2(1, 0);
    else if (localVertex == 2) offset = float2(0, 1);
    else if (localVertex == 3) offset = float2(1, 0);
    else if (localVertex == 4) offset = float2(1, 1);
    else offset = float2(0, 1);

    // Смещение чанка относительно центра [-1024, 1024]
    float2 chunkWorldOffset = float2(tileX * 2048.0, tileY * 2048.0); // ±2048 единиц

    float2 normalizedPos = float2(
        (x + offset.x) / gridX,
        (y + offset.y) / gridY
    );

    // Преобразуем в мировые координаты с учетом смещения чанка
    float2 p = float2(
        (normalizedPos.x - 0.5) * 2.0 * base_rangex + chunkWorldOffset.x,
        (normalizedPos.y - 0.5) * 2.0 * base_rangey + chunkWorldOffset.y
    );

    // Вычисляем координаты текстуры с враппингом
    int textureTileX = (centerChunkX + tileX + 16) % 16;
    int textureTileY = (centerChunkY + tileY + 16) % 16;

    float2 textureTileOffset = float2(textureTileX, textureTileY);
    float tileSize = 1.0 / 16; // Текстура 16x16 тайлов

    // Финальные UV
    float2 regionUV = (normalizedPos * tileSize) + (textureTileOffset * tileSize);

    float4 pos = float4(p, 0, 1);
    float height = heightMap.SampleLevel(sampLinear, regionUV, 0).r;
    float heightScale = 100;
    pos.z += exp(height * 2.5) * heightScale;
    output.wpos = pos.xy;
    output.pos = mul(pos, mul(view[0], proj[0]));
    output.height = height;
    return output;
}