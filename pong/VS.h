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
    float x = gConst[iID].x;      // Фиксированная X-координата нижнего левого угла
    float y = gConst[iID].y;      // Фиксированная Y-координата нижнего левого угла
    float x1 = gConst[iID].z;     // Ширина (размер по X)
    float y1 = gConst[iID].w;   // Высота (размер по Y)


    // Вершины квада (два треугольника)
    float2 quad[6] = {
        float2(x, y),   // Нижний левый
        float2(x, y1),  // Верхний левый
        float2(x1, y),  // Нижний правый
        

        float2(x1, y),  // Нижний правый (повтор)
        float2(x, y1),   // Верхний левый (повтор)
        float2(x1, y1) // Верхний правый
        

    };

    float2 uvCoords[6] = {
    float2(0, 1), // Нижний левый
    float2(0, 0), // Нижний правый
    float2(1, 1), // Верхний правый
    
    float2(1, 1), // Нижний правый
    float2(0, 0), // Верхний левый
    float2(1, 0) // Верхний правый
    };
    float4 viewPos = mul(float4(quad[vID], 1, 1.0f), view[0]);
    float4 projPos = mul(viewPos, proj[0]);

    output.pos = projPos;  // Позиция в clip-пространстве
    output.uv = uvCoords[vID];            // UV-координаты

    return output;
}
