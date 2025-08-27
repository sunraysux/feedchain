struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

static const float2 quad[6] = {
    float2(-1.0, -1.0),
    float2(-1.0,  1.0),
    float2(1.0, -1.0),

    float2(1.0, -1.0),
    float2(-1.0,  1.0),
    float2(1.0,  1.0)
};

VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT o;
    float2 p = quad[vID];
    o.pos = float4(p, 0.0, 1.0);        // уже в clip-space (NDC)
    o.uv = p * 0.5f + 0.5f;           // [0..1]
    return o;
}