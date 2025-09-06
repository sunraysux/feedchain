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
};


VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    float2 positions[6] = {
        float2(-1, -1),  // нижний левый
        float2(1, -1),   // нижний правый  
        float2(-1, 1),   // верхний левый
        float2(1, -1),   // нижний правый (повтор)
        float2(1, 1),    // верхний правый
        float2(-1, 1)    // верхний левый (повтор)
    };

    // ћасштабируем квад чтобы покрыть всю область рельефа
    float scale = 1024.0; // такой же как base_rangex
    float2 worldPos = positions[vID] * scale;
    float x = gConst[0].x;
    // ¬ысота воды
    float waterWorldHeight = x;
    float heightScale = waterWorldHeight * 6;
    float waterZ = waterWorldHeight * heightScale * heightScale * heightScale;

    float4 pos = float4(worldPos, waterZ, 1.0);

    output.wpos = pos.xyz;
    output.pos = mul(pos, mul(view[0], proj[0]));
    output.uv = positions[vID] * 0.5 + 0.5; // UV от [0,0] до [1,1]
    output.height = waterWorldHeight;

    return output;
}