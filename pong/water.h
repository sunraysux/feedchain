struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;

};

float4 PS(VS_OUTPUT input) : SV_Target
{
    return float4(0.83 ,0.94,0.95 ,1);
}