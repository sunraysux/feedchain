Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

cbuffer global : register(b5)
{
    float4 gConst[4096];
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
    uint instanceID : TEXCOORD1;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    return float4(1,1,1,1);

}