struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

cbuffer global : register(b5)
{
    float4 gConst[4096];
};

float GetHeight(int x, int y)
{
    int linearIndex = y * 65 + x;
    int bufferIndex = linearIndex / 4;
    int component = linearIndex % 4;

    float4 element = gConst[bufferIndex];

    if (component == 0) return element.x;
    else if (component == 1) return element.y;
    else if (component == 2) return element.z;
    else return element.w;
}

float SampleHeight(float2 uv)
{
    float2 scaledUV = uv * 64.0f;

    int x0 = floor(scaledUV.x);
    int y0 = floor(scaledUV.y);
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    x0 = x0 % 65;
    x1 = x1 % 65;
    y0 = y0 % 65;
    y1 = y1 % 65;

    float h00 = GetHeight(x0, y0);
    float h10 = GetHeight(x1, y0);
    float h01 = GetHeight(x0, y1);
    float h11 = GetHeight(x1, y1);

    float fracX = frac(scaledUV.x);
    float fracY = frac(scaledUV.y);

    float top = lerp(h00, h10, fracX);
    float bottom = lerp(h01, h11, fracX);
    return lerp(top, bottom, fracY);
}

float4 PS(VS_OUTPUT IN) : SV_Target
{
    float height = SampleHeight(IN.uv);

    return float4(height, height, height, 1.0f);
}