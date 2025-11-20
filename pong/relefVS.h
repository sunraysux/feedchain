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
    float4 gConst[4002];
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float height : TEXCOORD1;
    float2 wpos : TEXCOORD2;
    uint instanceID : TEXCOORD3;
};

VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    int gridX = gConst[0].x;
    int gridY = gConst[0].y;

    int quadID = vID / 6;
    int localVertex = vID % 6;

    const float AREA_SIZE = 32768.0;
    const uint INSTANCE_COUNT = 9; // 3x3 сетка инстансов

    // —мещение внутри инстанса (вершины квада)
    float2 offset;
    if (localVertex == 0) offset = float2(0, 0);
    else if (localVertex == 1) offset = float2(0, 1);
    else if (localVertex == 2) offset = float2(1, 0);
    else if (localVertex == 3) offset = float2(1, 0);
    else if (localVertex == 4) offset = float2(0, 1);
    else offset = float2(1, 1);

    // ¬ычисл€ем позицию вершины внутри инстанса
    int verticesPerInstance = gridX * gridY * 6;
    int localQuadID = (vID % verticesPerInstance) / 6;

    float2 normalizedPos = float2(
        (localQuadID % gridX + offset.x) / gridX,
        (localQuadID / gridX + offset.y) / gridY
    );

    // Ѕазовые координаты дл€ инстанса 0
    float2 worldXY = normalizedPos * AREA_SIZE;

    // ¬ычисл€ем смещение дл€ текущего инстанса (3x3 сетка)
    int2 instanceOffset = int2(iID % 3 - 1, 1 - iID / 3); // [-1, 0, 1] дл€ X и Y
    worldXY += instanceOffset * AREA_SIZE;

    // UV с учетом смещени€ инстанса
    float2 regionUV = (worldXY- instanceOffset * AREA_SIZE) / (AREA_SIZE);
    if (regionUV.y < 0.00001)regionUV.y = 1;
    if (regionUV.x < 0.00001)regionUV.x = 1;
    // ¬ысота из текстуры
    float height = heightMap.SampleLevel(sampLinear, regionUV, 0).r;

    float heightScale = 1500;
    float worldZ = height * heightScale;

    float4 pos = float4(worldXY, worldZ, 1);

    output.uv = regionUV;
    output.wpos = worldXY;
    output.pos = mul(pos, mul(view[0], proj[0]));
    output.height = worldZ;
    output.instanceID = iID;

    return output;
}