
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

cbuffer params : register(b1)
{
    float r, g, b;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 vpos : POSITION0;
    float4 wpos : POSITION1;
    float4 vnorm : NORMAL1;
    float2 uv : TEXCOORD0;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    if (input.wpos.x < -0.99) {
        return float4(1, 0, 0, 1);
        }
    else if (input.wpos.x < -0.98) {
        return float4(0, 1, 0, 1);
        }
    else  if (input.wpos.x < -0.97) {

        return float4(0, 0, 1, 1);
        }
    else  if (input.wpos.x < -0.96) {

        return float4(0, 1, 1, 1);
        }
    else  if (input.wpos.x < -0.95) {

        return float4(1, 0, 1, 1);
        }
    else if (input.wpos.x < -0.94)
        {

        return float4(1, 1, 0, 1);
        }
    else {

        return float4(1, 1, 1, 1);
        }

}