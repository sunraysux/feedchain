Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

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
    
    float4 color = tex.Sample(samplerState, input.uv);

    // если фон чЄрный Ч отбросить пиксель
    if (color.r < 0.0001 && color.g < 0.0001 && color.b < 0.0001)
        discard;

    return color;

}