Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 color = tex.Sample(samplerState, input.uv);
    float4 color1 =float4(1,1,1,1);
    return color;

}