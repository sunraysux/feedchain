cbuffer global : register(b5)
{
    float4 gConst[32];
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    float4 rect = gConst[0];
    // ���������� ���������� ������ ��� �������������� �����
    float2 posc[6] = {
        float2(rect.x, rect.w), // ����-����
        float2(rect.x, rect.z), // ����-���
        float2(rect.y, rect.w), // �����-����

        float2(rect.x, rect.z), // ����-���  
        float2(rect.y, rect.z), // �����-���
        float2(rect.y, rect.w)  // �����-����
    };

    // ��������������� UV-���������� ��� CCW �������
    float2 uvCoords[6] = {
        float2(0, 0),
        float2(0, 1),
        float2(1, 0),

        float2(0, 1),
        float2(1, 1),
        float2(1, 0)
    };
    output.pos = float4(posc[vID], 0.0, 1.0);  // ������� � clip-������������
    output.uv = uvCoords[vID];                // UV-����������

    return output;
}