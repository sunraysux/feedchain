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
};

inline float torusDeltaA(float from, float to, float size) {
    float diff = to - from;
    if (diff < 0)
        diff = diff * (-1);

    return diff;
}

float base_rangey = 30000.0;
float base_rangex = 30000.0;

VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    // Данные инстанса
    float x = gConst[0].x;      // X координата
    float y = gConst[0].y;      // Y координата
    float z =0;    
    float sz = 50;
    int gridX = 64;
    int gridY = 64;
    float dx = torusDeltaA((gConst[1].z), x, base_rangex);
    float dy = torusDeltaA((gConst[0].w), y, base_rangey);
    float Dd = dx * dx + dy * dy;
    float DD = sqrt(Dd) / gConst[0].z*1000;
     z = 7800 - DD;
    const int TILE_COUNT = 8;
    const float CHUNK_SIZE = 30000.0f;

    float2 regionUV = (float2(x, y)) / CHUNK_SIZE;
    if (regionUV.y < 0.00001)regionUV.y = 1;
    if (regionUV.x < 0.00001)regionUV.x = 1;
    // 6) sample высоты (как в terrain)
    float height = heightMap.SampleLevel(sampLinear, regionUV, 0).r;

    float heightScale = 1500;
    float worldZ = height * heightScale+z;

    // Базовая позиция билборда
    float3 p = float3(x, y, worldZ);

    float3 cameraRight = float3(view[0]._m00, view[0]._m10, view[0]._m20);
    float3 cameraUp = float3(view[0]._m01, view[0]._m11, view[0]._m21);
    float3 Up = float3(0, 0, 2); 

    float3 bottomLeft = p + cameraRight * sz - cameraUp * sz ;
    float3 bottomRight = p - cameraRight * sz - cameraUp * sz ;
    float3 topLeft = p + cameraRight * sz + cameraUp * sz ;
    float3 topRight = p - cameraRight * sz + cameraUp * sz ;

    float3 quad[6] = {
        bottomLeft, topLeft,bottomRight,
        bottomRight, topLeft,topRight
    };
    float2 uvCoords[6] = {
        float2(0, 1),
        float2(0, 0),
        float2(1, 1),

        float2(1, 1),
        float2(0, 0),
        float2(1, 0)
    };
    float3 wpos = float3(quad[vID].xy, quad[vID].z);
    float4 viewPos = mul(float4(wpos, 1.0f), view[0]);
    float4 projPos = mul(viewPos, proj[0]);

    output.pos = projPos;  // Позиция в clip-пространстве
    output.uv = uvCoords[vID];            // UV-координаты

    return output;
}
