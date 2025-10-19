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
    float4 rect = float4(0.505, 0.924, 0.126, 0.639);

    int POINTCOUNT = gConst[0].w;
    float count = vID / 6;

    float maxY = gConst[0].y;
    float h = 0.004;


    //    // �������� ������� � ��������� �������� ������
    //    float currentValue = gConst[pointIndex].x;
    //    float nextValue = gConst[pointIndex + 1].x;
    //
    int stat= gConst[count].x;
    float statNext = gConst[count + 1].x;

    //    // ����������� �������� Y � �������� [rect.z, rect.w]
    //    float normalizedCurrentY = rect.z + (currentValue / maxDataValue) * (rect.w - rect.z);
    //    float normalizedNextY = rect.z + (nextValue / maxDataValue) * (rect.w - rect.z);
    //
    float normalizedY = rect.z + (stat / maxY) * (rect.w - rect.z);
    float normalizedNextY = rect.z + (statNext / maxY) * (rect.w - rect.z);


    //    // ��������� ������� X ��� �������� ��������
    //    float segmentWidth = (rect.y - rect.x) / (totalPoints - 1);
    //    float currentX = rect.x + pointIndex * segmentWidth;
    //    float nextX = rect.x + (pointIndex + 1) * segmentWidth;
    //
    float rcd = rect.y - rect.x;
    float drsd = rcd / (POINTCOUNT - 1);
    float rc = rect.x + count * drsd;
    float rc2 = rect.x + (count + 1) * drsd;

    float4 rect2 = float4(rect.x, rect.y, rect.w, rect.w+h);
    rect = float4(rect.x, rect.y, rect.z, rect.z+h);
    
    // ���������� ���������� ������ ��� �������������� �����
    float2 posc[6] = {
        float2(rc,normalizedY+h), // ����-����
        float2(rc,normalizedY), // ����-���
        float2(rc2, normalizedNextY+h), // �����-����

        float2(rc, normalizedY), // ����-���  
        float2(rc2, normalizedNextY), // �����-���
        float2(rc2, normalizedNextY+h)  // �����-����
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
    output.pos = float4(posc[vID%6], 0.0, 1.0);  // ������� � clip-������������
    output.uv = uvCoords[vID%6];                // UV-����������

    return output;
}

//    // ������� ������������� ��� ����� ������� (������� �����)
//    float lineThickness = 0.002;
//
//    // ���������� ������ ��� ����� ������� (��� ������������)
//    float2 posc[6] = {
//        // ������ �����������
//        float2(currentX, normalizedCurrentY + lineThickness), // ����-���
//        float2(currentX, normalizedCurrentY - lineThickness), // ����-����  
//        float2(nextX, normalizedNextY + lineThickness),       // �����-���
//
//        // ������ �����������
//        float2(currentX, normalizedCurrentY - lineThickness), // ����-����
//        float2(nextX, normalizedNextY - lineThickness),       // �����-����
//        float2(nextX, normalizedNextY + lineThickness)        // �����-���
//    };
//
//    // UV ����������
//    float2 uvCoords[6] = {
//        float2(0, 0),
//        float2(0, 1),
//        float2(1, 0),
//        float2(0, 1),
//        float2(1, 1),
//        float2(1, 0)
//    };
//
//    output.pos = float4(posc[vID % 6], 0.0, 1.0);
//    output.uv = uvCoords[vID % 6];
//
//    return output;
//}