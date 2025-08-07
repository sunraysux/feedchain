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
    float2 uv : TEXCOORD0;
};

float3 rotY(float3 pos, float a)
{
    float3x3 m = float3x3(
        cos(a), 0, sin(a),
        0, 1, 0,
        -sin(a), 0, cos(a)
    );
    return mul(pos, m);
}

float3 rotX(float3 pos, float a)
{
    float3x3 m = float3x3(
        1, 0, 0,
        0, cos(a), -sin(a),
        0, sin(a), cos(a)
    );
    return mul(pos, m);
}

float3 rotZ(float3 pos, float a)
{
    float3x3 m = float3x3(
        cos(a), sin(a), 0,
        -sin(a), cos(a), 0,
        0, 0, 1
    );
    return mul(pos, m);
}

float3 cubeToSphere(float3 p)
{
    float x2 = p.x * p.x;
    float y2 = p.y * p.y;
    float z2 = p.z * p.z;

    float3 result;
    result.x = p.x * sqrt(1.0 - (y2 + z2) * 0.5 + (y2 * z2) / 3.0);
    result.y = p.y * sqrt(1.0 - (z2 + x2) * 0.5 + (z2 * x2) / 3.0);
    result.z = p.z * sqrt(1.0 - (x2 + y2) * 0.5 + (x2 * y2) / 3.0);

    return result;
}

float3 calcGeom(float2 uv, int faceID)
{
    float2 p = uv * 2.0 - 1.0;
    float3 cubePos;
    if (faceID == 0)      cubePos = float3(-1, p.y, p.x);
    else if (faceID == 1) cubePos = float3(1, p.y, -p.x);
    else if (faceID == 2) cubePos = float3(-p.x, -1, -p.y);
    else if (faceID == 3) cubePos = float3(-p.x, 1, p.y);
    else if (faceID == 4) cubePos = float3(-p.x, p.y, -1);
    else if (faceID == 5) cubePos = float3(p.x, p.y, 1);
    else                  cubePos = float3(0, 0, 0); // fallback
    return cubeToSphere(cubePos);
    cubePos = rotX(cubeToSphere(cubePos), time.x * 0.05);
    return rotY(cubePos, time.x * 0.05);
}


VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    // Получаем позицию существа на грани
    float2 bottomLeft = gConst[iID].xy; // (x0, y0)
    float2 topRight = gConst[iID].zw; // (x1, y1)

    // Локальные вершины квада (в пределах существа)
    float2 quadVerts[6] = {
    float2(0, 0), float2(1, 0), float2(0, 1),
    float2(0, 1), float2(1, 0), float2(1, 1)
    };

    float2 localUV = quadVerts[vID];

    // Преобразуем в глобальные координаты на грани
    float2 gridPos = lerp( topRight, bottomLeft, localUV); // внутри существа
    float2 uv = gridPos / float2(100, 100); // нормализуем в UV

    // Теперь получаем позицию на сфере
    int faceID =4; // Пока фиксированная грань. Потом можешь передавать как gConst[iID].a
    float3 worldPos = calcGeom(uv, faceID) * 60.0; // радиус

    // Преобразуем в clip space
    output.pos = mul(float4(worldPos, 1.0), mul(view[0], proj[0]));

    // UV для текстуры существа
    float2 uvCoords[6] = {
    float2(0, 0), // Нижний левый
    float2(1, 0), // Нижний правый
    float2(0, 1), // Верхний левый

    float2(0, 1), // Верхний левый
    float2(1, 0), // Нижний правый
    float2(1, 1)  // Верхний правый
    };
    output.uv = uvCoords[vID];

    return output;
}