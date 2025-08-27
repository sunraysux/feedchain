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
    float x = gConst[iID].x;      // ������������� X-���������� ������� ������ ����
    float y = gConst[iID].y;      // ������������� Y-���������� ������� ������ ����
    float age = gConst[iID].z;     // ������ (������ �� X)
    float scale = gConst[iID].w;   // ������ (������ �� Y)

    float base_rangex = 1024.0f;
    float base_rangey = 512.0f;
    // ������� ����� (��� ������������)

    float3 p =float3( x,y,0);
    float2 uv = frac(p.xy / float2(base_rangex, base_rangey) * 0.5 + 0.5);


    // ������
    float height = heightMap.SampleLevel(sampLinear, uv, 0).r;
    p.z = height;
    float heightScale = 100;
    p.z += height * heightScale ;
    float sz = age / scale;
    float3 quad[6] = {
    float3(p.x - sz, p.y,p.z ),   // ������ �����
    float3(p.x + sz, p.y,p.z ),  // ������� �����
    float3(p.x - sz, p.y,p.z + sz),  // ������ ������
                           
                           
    float3(p.x + sz, p.y,p.z ),  // ������ ������ (������)
    float3(p.x + sz, p.y,p.z + sz),   // ������� ����� (������)
    float3(p.x - sz, p.y,p.z + sz) // ������� ������


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
