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

VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    float x1 = -0.99;      // Фиксированная X-координата нижнего левого угла
    float x = -1;
    float y = -1;
    float y1 = gConst[0].x-1;

    float x2 = -0.98 ;
    float x3 = -0.99;
    float y3 = -1;
    float y2 = gConst[0].y-1;

    float x4 = -0.97;
    float x5 = -0.98;
    float y5 = -1;
    float y4 = gConst[0].z-1;
    // Вершины квада (два треугольника)
    float2 quad[18] = {
        float2(x, y),   // Нижний левый
        float2(x, y1),  // Верхний левый
        float2(x1, y),  // Нижний правый
        

        float2(x1, y),  // Нижний правый (повтор)
        float2(x, y1),   // Верхний левый (повтор)
        float2(x1, y1), // Верхний правый
        

        float2(x3, y3),   // Нижний левый
        float2(x3, y2),  // Верхний левый
        float2(x2, y3),  // Нижний правый
        

        float2(x2, y3),  // Нижний правый (повтор)
        float2(x3, y2),   // Верхний левый (повтор)
        float2(x2, y2), // Верхний правый

        float2(x5, y5),   // Нижний левый
        float2(x5, y4),  // Верхний левый
        float2(x4, y5),  // Нижний правый


        float2(x4, y5),  // Нижний правый (повтор)
        float2(x5, y4),   // Верхний левый (повтор)
        float2(x4, y4) // Верхний правый
        

    };
    float4 viewPos = mul(float4(quad[vID], 0, 1.0f), view[0]);
    float4 projPos = mul(viewPos, proj[0]);
    output.wpos = float4(quad[vID], 0, 1.0f);
    output.pos = float4(quad[vID], 0, 1.0f);  

    return output;
}
