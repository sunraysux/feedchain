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

    // ���������� ������� � ������ ��� ������ �������
    float barPositions = -0.35;
    float barHeights = -0.85;
    int slot = gConst[0].z;
    int speed= gConst[0].y;
    // ���������� ����� ������� � ����� vertex � ��� ������
    uint barIndex = vID / 6;      // 6 ������ �� ������� (2 ������������)
    uint vertexInBar = vID % 6;   // ����� ������� ������ �������
    if (gConst[0].x == slot)
        output.vpos = float4(1, 1, 1, 1);
    if(slot==-1)
        output.vpos = float4(-1, -1, -1, -1);
    float2 uvCoords[6] = {
    float2(0, 1), // ������ �����
    float2(1, 1), // ������ ������
    float2(0, 0), // ������� �����

    float2(1, 1), // ������ ������
    float2(1, 0), // ������� ������
    float2(0, 0)  // ������� �����
    };
    float barLeft = barPositions - 0.1+slot*0.1;  // ����� ������� �������
    float barRight = barPositions + slot * 0.1;        // ������ ������� �������
    float barBottom = -1;                           // ������ �������
    float barTop = barHeights;            // ������� �������

    // ������� ��������� ��� ����� ������� (2 ������������)
    float2 vertices[6] = {
        float2(barLeft, barBottom),   // ������ �����
        float2(barRight, barBottom),      // ������� �����  
        float2(barLeft, barTop),  // ������ ������

        float2(barRight, barBottom),  // ������ ������
        float2(barRight, barTop),      // ������� �����
        float2(barLeft, barTop)      // ������� ������
    };

    float2 position = vertices[vertexInBar];
    output.pos = float4(position, 0, 1.0f);
    output.uv = uvCoords[vID];
    return output;
}