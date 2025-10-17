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
    float4 vpos : POSITION0;
    float2 uv : TEXCOORD0;
};

float4 PS(VS_OUTPUT input) : SV_Target
{

    float4 color = tex.Sample(samplerState, input.uv);

// если фон чЄрный Ч отбросить пиксель
if (color.r < 0.0001 && color.g < 0.0001 && color.b < 0.0001)
{
    if (input.vpos.x == 1)
        return float4(0.8, gConst[0].w+0.4, 0.8, 1);
    if (input.vpos.x == -1)
        discard;
    return float4(0.5, gConst[0].w+0.5, 0.5, 0.7);
}
float4 color2 = float4(0.01, 0.01, 0.01, 0.01);
return color + color2;

}