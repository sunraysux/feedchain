Texture2D heightMap : register(t0);
SamplerState sampLinear : register(s0);

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
    float x =0;      // ������������� X-���������� ������� ������ ����
    float y = 0;      // ������������� Y-���������� ������� ������ ����

    float base_rangex = 1024.0f;
    float base_rangey = 1024.0f;
    // ������� ����� (��� ������������)

    float3 p = float3(x, y, 0);


    // ������
    float height = 0.50;
    p.z = height;
    float heightScale = 100;
    p.z = height * heightScale;
    float sz = 1024;
    float3 quad[6] = {
    float3(p.x - sz, p.y-sz,p.z),   // ������ �����
    float3(p.x + sz, p.y-sz,p.z),  // ������� �����
    float3(p.x - sz, p.y+sz,p.z),  // ������ ������


    float3(p.x + sz, p.y-sz,p.z),  // ������ ������ (������)
    float3(p.x + sz, p.y+sz,p.z),   // ������� ����� (������)
    float3(p.x - sz, p.y+sz,p.z) // ������� ������


    };
    float2 uvCoords[6] = {
        float2(0, 1), // ������ �����
        float2(1, 1), // ������ ������
        float2(0, 0), // ������� �����

        float2(1, 1), // ������ ������
        float2(1, 0), // ������� ������
        float2(0, 0)  // ������� �����
    };
    float4 viewPos = mul(float4(quad[vID], 1.0f), view[0]);
    float4 projPos = mul(viewPos, proj[0]);

    output.pos = projPos;  // ������� � clip-������������
    output.uv = uvCoords[vID];            // UV-����������

    return output;
}
