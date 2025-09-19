
struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
};


float4 PS(VS_OUTPUT input) : SV_Target
{
    // Просто белый цвет, полностью непрозрачный
    return float4(1.0, 1.0, 1.0, 1.0);
}