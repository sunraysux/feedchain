Texture2D heightMap : register(t0);
SamplerState sampLinear : register(s0);

cbuffer global : register(b5)
{
    float4 gConst[4096];
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
    float2 uv : TEXCOORD0;
    uint instanceID : TEXCOORD1;
};

VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    // Данные инстанса
    float x = gConst[iID + 8].x;      // X координата
    float y = gConst[iID + 8].y;      // Y координата
    float z = gConst[iID + 8].z;     // Размер билборда
    float sz = 100.0f; // Высота билборда

    int gridX = 64;
    int gridY = 64;

    const int TILE_COUNT = 8;
    const float CHUNK_SIZE = 32768.0f;

    float2 regionUV = (float2(x, y)) / CHUNK_SIZE;
    if (regionUV.y < 0.00001)regionUV.y = 1;
    if (regionUV.x < 0.00001)regionUV.x = 1;
    // 6) sample высоты (как в terrain)

    // Базовая позиция билборда
    float3 p = float3(x, y, z);


    float3 quad[36] = { 
        // передняя сторона (Z = 0)
        float3(0, 0, 0), float3(1, 0, 0), float3(0, 1, 0),
        float3(1, 0, 0), float3(1, 1, 0), float3(0, 1, 0),

        float3(0, 1, 1), float3(1, 0, 1), float3(0, 0, 1),
        float3(1, 0, 1), float3(0, 1, 1), float3(1, 1, 1),


        float3(0, 0, 1), float3(1, 0, 0), float3(0, 0, 0),
        float3(1, 0, 0), float3(0, 0, 1), float3(1, 0, 1),

        float3(1, 1, 0), float3(1, 1, 1), float3(0, 1, 1),
        float3(0, 1, 0), float3(1, 1, 0), float3(0, 1, 1),
        

        float3(0, 0, 0), float3(0, 1, 0), float3(0, 0, 1),
        float3(0, 0, 1), float3(0, 1, 0), float3(0, 1, 1),

        float3(1, 0, 0), float3(1, 0, 1), float3(1, 1, 0),
        float3(1, 0, 1), float3(1, 1, 1), float3(1, 1, 0)
    };
    float3 wpos = float3(quad[vID])*sz+p;
    float4 viewPos = mul(float4(wpos, 1.0f), view[0]);
    float4 projPos = mul(viewPos, proj[0]);

    output.pos = projPos;  // Позиция в clip-пространстве
    output.uv = float2(wpos.x, wpos.z);          // UV-координаты
    output.instanceID = iID;

    return output;
}
