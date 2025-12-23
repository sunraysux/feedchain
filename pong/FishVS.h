// Текстура высот (если нужно)
Texture2D heightMap : register(t0);
// StructuredBuffer для данных инстансов рыб
StructuredBuffer<float4> fishInstanceData : register(t1);
SamplerState sampLinear : register(s0);

// Матрицы камеры
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

// Статические данные для куба (36 вершин)
static const float3 cubeVertices[36] = {
    // Передняя грань (Z = 0) - нормаль (0, 0, -1)
    float3(0, 0, 0), float3(1, 0, 0), float3(1, 1, 0),
    float3(0, 0, 0), float3(1, 1, 0), float3(0, 1, 0),

    // Задняя грань (Z = 1) - нормаль (0, 0, 1)
    float3(0, 0, 1), float3(1, 1, 1), float3(1, 0, 1),
    float3(0, 0, 1), float3(0, 1, 1), float3(1, 1, 1),

    // Нижняя грань (Y = 0) - нормаль (0, -1, 0)
    float3(0, 0, 0), float3(1, 0, 1), float3(1, 0, 0),
    float3(0, 0, 0), float3(0, 0, 1), float3(1, 0, 1),

    // Верхняя грань (Y = 1) - нормаль (0, 1, 0)
    float3(0, 1, 0), float3(1, 1, 0), float3(1, 1, 1),
    float3(0, 1, 0), float3(1, 1, 1), float3(0, 1, 1),

    // Левая грань (X = 0) - нормаль (-1, 0, 0)
    float3(0, 0, 0), float3(0, 1, 0), float3(0, 1, 1),
    float3(0, 0, 0), float3(0, 1, 1), float3(0, 0, 1),

    // Правая грань (X = 1) - нормаль (1, 0, 0)
    float3(1, 0, 0), float3(1, 1, 1), float3(1, 1, 0),
    float3(1, 0, 0), float3(1, 0, 1), float3(1, 1, 1)
};

// UV-координаты для куба
static const float2 cubeUVs[36] = {
    // Передняя грань
    float2(0.25, 0.666), float2(0.5, 0.666), float2(0.5, 0.333),
    float2(0.25, 0.666), float2(0.5, 0.333), float2(0.25, 0.333),

    // Задняя грань
    float2(0.75, 0.666), float2(1.0, 0.333), float2(1.0, 0.666),
    float2(0.75, 0.666), float2(0.75, 0.333), float2(1.0, 0.333),

    // Нижняя грань
    float2(0.25, 1.0), float2(0.5, 0.666), float2(0.5, 1.0),
    float2(0.25, 1.0), float2(0.25, 0.666), float2(0.5, 0.666),

    // Верхняя грань
    float2(0.25, 0.333), float2(0.5, 0.333), float2(0.5, 0),
    float2(0.25, 0.333), float2(0.5, 0), float2(0.25, 0),

    // Левая грань
    float2(0.0, 0.666), float2(0.25, 0.666), float2(0.25, 0.333),
    float2(0.0, 0.666), float2(0.25, 0.333), float2(0.0, 0.333),

    // Правая грань
    float2(0.5, 0.666), float2(0.75, 0.333), float2(0.75, 0.666),
    float2(0.5, 0.666), float2(0.5, 0.333), float2(0.75, 0.333)
};

VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    // Получаем данные инстанса из StructuredBuffer
    float4 instance = fishInstanceData[iID];
    float x = instance.x;      // X координата
    float y = instance.y;      // Y координата  
    float z = instance.z;      // Z координата (глубина)
    float sz = instance.w;     // Размер рыбы

    // Если размер в данных = 100 (как в вашем коде), но можно использовать instance.w
    // float sz = instance.w; // или задаем фиксированный размер, если instance.w = 100

    // Получаем базовую позицию
    float3 p = float3(x, y, z);

    // Масштабируем вершины куба размером рыбы и смещаем относительно центра
    // Чтобы куб был центрирован в точке p, нужно сместить на -0.5 * размер
    float3 cubeVertex = cubeVertices[vID] * sz - float3(sz * 0.5, sz * 0.5, sz * 0.5);

    // Преобразуем в мировые координаты
    float3 worldPos = cubeVertex + p;

    // Видовое и проекционное преобразование
    float4 viewPos = mul(float4(worldPos, 1.0f), view[0]);
    float4 projPos = mul(viewPos, proj[0]);

    output.pos = projPos;
    output.uv = cubeUVs[vID];
    output.instanceID = iID;

    return output;
}