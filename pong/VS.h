Texture2D heightMap : register(t0);
SamplerState sampLinear : register(s0);


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
};



VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    float x = gConst[iID].x;      // Фиксированная X-координата нижнего левого угла
    float y = gConst[iID].y;      // Фиксированная Y-координата нижнего левого угла
    float sz = gConst[iID].z;     // Ширина (размер по X)


    float base_rangex = 1024.0f;
    float base_rangey = 1024.0f;
    // Вершины квада (два треугольника)

    float3 p = float3(x, y, 0);
    float2 uv = (p.xy + float2(base_rangex, base_rangey)) / float2(2.0f * base_rangex, 2.0f * base_rangey);

    // высота
    float height = heightMap.SampleLevel(sampLinear, uv / 4, 0).r;
    float heightScale = height * 7;
    p.z = height * heightScale * heightScale * heightScale;
    //p.z = height * heightScale ;

    float3 cameraRight = float3(view[0]._m00, view[0]._m10, view[0]._m20);
    float3 cameraUp = float3(0,0,gConst[iID].w); // Z - это высота

    float3 bottomLeft = p - cameraRight * sz ;
    float3 bottomRight = p + cameraRight * sz ;
    float3 topLeft = p - cameraRight * sz + cameraUp * sz;
    float3 topRight = p + cameraRight * sz + cameraUp * sz;

    float3 quad[6] = {
        bottomLeft, bottomRight, topLeft,
        bottomRight, topRight, topLeft
    };
    float2 uvCoords[6] = {
        float2(0, 1), // Нижний левый
        float2(1, 1), // Нижний правый
        float2(0, 0), // Верхний левый

        float2(1, 1), // Нижний правый
        float2(1, 0), // Верхний правый
        float2(0, 0)  // Верхний левый
    };
    float3 wpos = float3(quad[vID].xy, quad[vID].z);
    float4 viewPos = mul(float4(wpos, 1.0f), view[0]);
    float4 projPos = mul(viewPos, proj[0]);

    output.pos = projPos;  // Позиция в clip-пространстве
    output.uv = uvCoords[vID];            // UV-координаты

    return output;
}
