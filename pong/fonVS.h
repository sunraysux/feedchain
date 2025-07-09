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

VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    float x = gConst[0].x;      // ������������� X-���������� ������� ������ ����
    float y = gConst[0].y;      // ������������� Y-���������� ������� ������ ����
    float sz = gConst[0].z;     // ������ (������ �� X)
    float years = gConst[0].w;   // ������ (������ �� Y)

    // ������������ ���� ��������������
    float x1 = x + sz;          // ������ ������� (X)
    float y1 = y + years;       // ������� ������� (Y)

    // ������� ����� (��� ������������)
    float2 quad[6] = {
        float2(-100, -100),
        float2(100, -100), 
        float2(-100, 100), 

        float2(100, -100), 
        float2(100,100), 
        float2(-100, 100)  

    };

    // UV-���������� (���� ����� ��� ��������)
    float2 uv_coords[6] = {
        float2(x, y1),   // ������������� (x,y)
        float2(x1, y1),   // ������������� (x1,y)
        float2(x, y),   // ������������� (x,y1)

        float2(x1, y1),   // ������
        float2(x1, y),   // ������������� (x1,y1)
        float2(y, y)    // ������
    };
    float4 viewPos = mul(float4(quad[vID], 1, 1.0f), view[0]);
    float4 projPos = mul(viewPos, proj[0]);

    output.pos = projPos;  // ������� � clip-������������
    output.uv = uv_coords[vID];            // UV-����������

    return output;
}
