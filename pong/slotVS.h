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

    // Определяем позиции и высоты для каждой полоски
    float barPositions = -0.35;
    float barHeights = -0.85;
    int slot = gConst[0].z;
    int speed= gConst[0].y;
    // Определяем какую полоску и какой vertex в ней рисуем
    uint barIndex = vID / 6;      // 6 вершин на полоску (2 треугольника)
    uint vertexInBar = vID % 6;   // Номер вершины внутри полоски
    if (gConst[0].x == slot)
        output.vpos = float4(1, 1, 1, 1);
    if(slot==-1)
        output.vpos = float4(-1, -1, -1, -1);
    float2 uvCoords[6] = {
    float2(0, 1), // Нижний левый
    float2(1, 1), // Нижний правый
    float2(0, 0), // Верхний левый

    float2(1, 1), // Нижний правый
    float2(1, 0), // Верхний правый
    float2(0, 0)  // Верхний левый
    };
    float barLeft = barPositions - 0.1+slot*0.1;  // Левая граница полоски
    float barRight = barPositions + slot * 0.1;        // Правая граница полоски
    float barBottom = -1;                           // Нижняя граница
    float barTop = barHeights;            // Верхняя граница

    // Создаем геометрию для одной полоски (2 треугольника)
    float2 vertices[6] = {
        float2(barLeft, barBottom),   // Нижний левый
        float2(barRight, barBottom),      // Верхний левый  
        float2(barLeft, barTop),  // Нижний правый

        float2(barRight, barBottom),  // Нижний правый
        float2(barRight, barTop),      // Верхний левый
        float2(barLeft, barTop)      // Верхний правый
    };

    float2 position = vertices[vertexInBar];
    output.pos = float4(position, 0, 1.0f);
    output.uv = uvCoords[vID];
    return output;
}