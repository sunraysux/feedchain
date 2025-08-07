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

    // Получаем параметры экземпляра
    float x = gConst[iID].x;      // X-координата
    float y = gConst[iID].y;      // Y-координата
    float x1 = gConst[iID].z;     // Ширина
    float y1 = gConst[iID].w;     // Высота

    // Вершины квада (два треугольника)
    float2 quad[6] = {
        float2(x, y),   // Нижний левый
        float2(x, y1),  // Верхний левый
        float2(x1, y),  // Нижний правый
        float2(x1, y),  // Нижний правый (повтор)
        float2(x, y1),  // Верхний левый (повтор)
        float2(x1, y1)  // Верхний правый
    };

    // UV-координаты
    float2 uvCoords[6] = {
        float2(0, 1), float2(0, 0), float2(1, 1),
        float2(1, 1), float2(0, 0), float2(1, 0)
    };

    // Текущая позиция вершины в 2D
    float2 pos2D = quad[vID];

    // Центр текущего объекта
    float2 objCenter = float2((x + x1) * 0.5, (y + y1) * 0.5);

    // Вектор от центра сцены к центру объекта
    float2 toObjCenter = objCenter - float2(0, 0);
    float distFromSceneCenter = length(toObjCenter);

    // Нормализованное расстояние от центра сцены (0-1)
    float normalizedDist = saturate(distFromSceneCenter / 100.0); // 100 - максимальный радиус

    // Z-координата изменяется от 50 в центре до 25 на краю
    float zPos = lerp(50.0, 25.0, normalizedDist * normalizedDist);

    // Если нужно сферическое распределение внутри каждого объекта:
    float2 vertexOffset = pos2D - objCenter;
    float vertexDist = length(vertexOffset) / length(float2(x1 - x, y1 - y));
    zPos -= 10.0 * vertexDist; // Добавляем небольшую кривизну к каждому объекту

    // Итоговая позиция в мировых координатах
    float4 worldPos = float4(pos2D.x, pos2D.y, -zPos, 1.0);

    // Преобразование в пространство камеры
    float4 viewPos = mul(worldPos, view[0]);
    float4 projPos = mul(viewPos, proj[0]);

    output.pos = projPos;
    output.uv = uvCoords[vID];

    return output;
}