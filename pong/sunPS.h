

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

float4 PS(VS_OUTPUT input) : SV_Target
{
    return float4(1.0,1.0,0.0,1.0);

}
