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

    // ���������� ������� � ������ ��� ������ �������
    float barPositions[7] = { -0.99, -0.98, -0.97, -0.96, -0.95, -0.94,-0.93 };
    float barHeights[7] = {
        gConst[0].x - 1,  // rabbitRatio
        gConst[0].y - 1,  // treeRatio
        gConst[0].z - 1,  // wolfRatio
        gConst[0].w - 1,  // bushRatio
        gConst[1].x - 1,  // ratRatio
        gConst[1].y - 1,
        gConst[1].z - 1 // eagleRatio
    };

    // ���������� ����� ������� � ����� vertex � ��� ������
    uint barIndex = vID / 6;      // 6 ������ �� ������� (2 ������������)
    uint vertexInBar = vID % 6;   // ����� ������� ������ �������

    if (barIndex >= 7) {
        output.pos = float4(0, 0, 0, 1);
        return output;
    }

    float barLeft = barPositions[barIndex] - 0.01;  // ����� ������� �������
    float barRight = barPositions[barIndex];        // ������ ������� �������
    float barBottom = -1;                           // ������ �������
    float barTop = barHeights[barIndex];            // ������� �������

    // ������� ��������� ��� ����� ������� (2 ������������)
    float2 vertices[6] = {
        float2(barLeft, barBottom),   // ������ �����
        float2(barLeft, barTop),      // ������� �����  
        float2(barRight, barBottom),  // ������ ������

        float2(barRight, barBottom),  // ������ ������
        float2(barLeft, barTop),      // ������� �����
        float2(barRight, barTop)      // ������� ������
    };

    float2 position = vertices[vertexInBar];
    output.wpos = float4(position, 0, 1.0f);
    output.pos = float4(position, 0, 1.0f);

    return output;
}