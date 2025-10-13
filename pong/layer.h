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


struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;   // глобальные UV для heightMap
    float3 wpos : TEXCOORD1;   // мировые XY + waterLevel Z
};

VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    float waterLevel = gConst[0].x;

    // 9 регионов
    int regionID = iID;
    int regionX = regionID % 3 - 1;
    int regionY = regionID / 3 - 1;

    // Позиции вершин
    float2 positions[6] = {
        float2(0,0), float2(1,0), float2(0,1),
        float2(1,0), float2(1,1), float2(0,1)
    };
    float2 localPos = positions[vID];

    const float REGION_SIZE = 32768.0;

    // Смещение региона
    float2 regionOffset = float2(regionX * REGION_SIZE, regionY * REGION_SIZE);

    // Мировые координаты
    float2 worldXY = regionOffset + (localPos - 0.5) * REGION_SIZE;

    // ФИКС: ПРОСТО ИСПОЛЬЗУЕМ localPos ДЛЯ UV
    float2 regionUV = localPos;

    float4 worldPos = float4(worldXY.x, worldXY.y, waterLevel, 1.0);

    output.pos = mul(worldPos, mul(view[0], proj[0]));
    output.uv = regionUV;
    output.wpos = float3(worldXY, waterLevel);

    return output;
}