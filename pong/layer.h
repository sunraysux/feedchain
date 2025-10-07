Texture2D heightMap : register(t0);
SamplerState sampLinear : register(s0);

cbuffer frameBuffer : register(b4)
{
    float4 time;
    float4 aspect;
};

cbuffer global : register(b5)
{
    float4 gConst[32];
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
    float height : TEXCOORD1;
    float3 wpos : TEXCOORD2;
    float3 normal : TEXCOORD3;
};


VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    // Вершины куба без дна (5 граней ? 6 вершин = 30 вершин)
    float3 positions[6] = {
        float3(-1, -1, 1), float3(1, -1, 1), float3(-1, 1, 1),
        float3(1, -1, 1), float3(1, 1, 1), float3(-1, 1, 1),
    };
    // Нормали для каждой грани
    float3 normals[5] = {
        float3(0, 0, 1),    // Верх
        float3(0, -1, 0),   // Перед
        float3(0, 1, 0),    // Зад
        float3(-1, 0, 0),   // Лево
        float3(1, 0, 0)     // Право
    };

    float scale = 1024.0*3;
    float waterWorldHeight = gConst[0].x;
    float heightScale = waterWorldHeight * 7;
    float waterZ = waterWorldHeight * heightScale * heightScale * heightScale;
    // Масштабируем и позиционируем вершины
    float3 vertexPos = positions[vID];
    float3 worldPos = float3(vertexPos.xy * scale, vertexPos.z * waterZ);
    output.wpos = worldPos;
    output.pos = mul(float4(worldPos, 1.0), mul(view[0], proj[0]));
    output.uv = vertexPos.xy * 0.5 + 0.5;
    output.height = waterWorldHeight;

    return output;
}