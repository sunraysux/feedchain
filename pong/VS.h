Texture2D heightMap : register(t0);
SamplerState sampLinear : register(s0);

cbuffer global : register(b5)
{
    float4 gConst[4010]; // ����������� ������ ������
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

    // ������ ��������
    float x = gConst[iID+8].x;      // X ����������
    float y = gConst[iID+8].y;      // Y ����������
    float sz = gConst[iID+8].z;     // ������ ��������
    float billboardHeight = gConst[iID].w; // ������ ��������

    int gridX = 64;
    int gridY = 64;

    const int TILE_COUNT = 8;
    const float CHUNK_SIZE = 32768.0f;
    float halfChunk = CHUNK_SIZE * 0.5;

    float2 regionUV = (float2(x, y) + halfChunk) / CHUNK_SIZE;

    // 6) sample ������ (��� � terrain)
    float height = heightMap.SampleLevel(sampLinear, regionUV, 0).r;
    float depth = heightMap.SampleLevel(sampLinear, regionUV, 0).g;

    float heightScale = 200;
    float depthScale = 80;
    float worldZ = exp(height * 2) * heightScale - exp(depth * 2) * depthScale;

    // ������� ������� ��������
    float3 p = float3(x, y, worldZ);

    float3 cameraRight = float3(view[0]._m00, view[0]._m10, view[0]._m20);
    float3 cameraUp = float3(0, 0, gConst[iID+8].w); // Z - ��� ������

    float3 bottomLeft = p + cameraRight * sz;
    float3 bottomRight = p - cameraRight * sz;
    float3 topLeft = p + cameraRight * sz + cameraUp * sz;
    float3 topRight = p - cameraRight * sz + cameraUp * sz;

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

    output.pos = projPos;  // ������� � clip-������������
    output.uv = uvCoords[vID];            // UV-����������

    return output;
}
