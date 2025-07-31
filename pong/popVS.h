cbuffer global : register(b5)
{
    float4 gConst[32];
};

cbuffer frame : register(b4)
{
    float4 time;
    float4 aspect;
};

cbuffer camera : register(b3)
{
    float4x4 world[2];
    float4x4 view[2];
    float4x4 proj[2];
};

cbuffer drawMat : register(b2)
{
    float4x4 model;
    float hilight;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 vpos : POSITION0;
    float4 wpos : POSITION1;
    float4 vnorm : NORMAL1;
    float2 uv : TEXCOORD0;
};

float3 rotY(float3 pos, float a)
{
    float3x3 m =
    {
        cos(a), 0, sin(a),
        0, 1, 0,
        -sin(a), 0, cos(a)
    };
    pos = mul(pos, m);
    return pos;
}

VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    float x1 = -50+gConst[iID].x/10;      // Фиксированная X-координата нижнего левого угла
    int x = -50;
    int y = -51;
    int y1 = -50;

    float x2 = -50 + gConst[iID].y /10;
    int x3 = -50;
    int y3 = -53;
    int y2 = -52;
    // Вершины квада (два треугольника)
    float2 quad[12] = {
        float2(x, y),   // Нижний левый
        float2(x1, y),  // Нижний правый
        float2(x, y1),  // Верхний левый

        float2(x1, y),  // Нижний правый (повтор)
        float2(x1, y1), // Верхний правый
        float2(x, y1),   // Верхний левый (повтор)

        float2(x3, y3),   // Нижний левый
        float2(x2, y3),  // Нижний правый
        float2(x3, y2),  // Верхний левый

        float2(x2, y3),  // Нижний правый (повтор)
        float2(x2, y2), // Верхний правый
        float2(x3, y2)   // Верхний левый (повтор)

    };
    float4 viewPos = mul(float4(quad[vID], 1, 1.0f), view[0]);
    float4 projPos = mul(viewPos, proj[0]);
    output.wpos = float4(quad[vID], 1, 1.0f);
    output.pos = projPos;  // Позиция в clip-пространстве

    return output;
}
