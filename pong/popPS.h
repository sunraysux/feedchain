cbuffer global : register(b5)
{
    float4 gConst[32];
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    float R = gConst[0].z;
float4 color2 = float4(R, 0.5 , 0.5, 1);
return color2;

}



