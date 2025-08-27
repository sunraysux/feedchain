struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

#define PI 3.14159265359

// --- UTILS ---
float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }

float hash33_to_1(int3 p)
{
    uint x = (uint)p.x * 73856093u;
    uint y = (uint)p.y * 19349663u;
    uint z = (uint)p.z * 83492791u;
    uint n = x ^ y ^ z;
    n = (n << 13) ^ n;
    return frac((float)(n * (n * n * 15731u + 789221u) + 1376312589u) * 1.1920928955078125e-7f);
}

float valueNoise3D(float3 p)
{
    int3 i0 = (int3)floor(p);
    float3 f = frac(p);
    float3 u = float3(fade(f.x), fade(f.y), fade(f.z));

    float c000 = hash33_to_1(i0 + int3(0, 0, 0));
    float c100 = hash33_to_1(i0 + int3(1, 0, 0));
    float c010 = hash33_to_1(i0 + int3(0, 1, 0));
    float c110 = hash33_to_1(i0 + int3(1, 1, 0));
    float c001 = hash33_to_1(i0 + int3(0, 0, 1));
    float c101 = hash33_to_1(i0 + int3(1, 0, 1));
    float c011 = hash33_to_1(i0 + int3(0, 1, 1));
    float c111 = hash33_to_1(i0 + int3(1, 1, 1));

    float cx00 = lerp(c000, c100, u.x);
    float cx10 = lerp(c010, c110, u.x);
    float cx01 = lerp(c001, c101, u.x);
    float cx11 = lerp(c011, c111, u.x);

    float cxy0 = lerp(cx00, cx10, u.y);
    float cxy1 = lerp(cx01, cx11, u.y);

    float c = lerp(cxy0, cxy1, u.z);
    return c * 2.0 - 1.0;
}

float fbm3D(float3 p, int octaves, float lacunarity, float gain)
{
    float sum = 0.0;
    float amp = 1.0;
    float freq = 1.0;
    float ampSum = 0.0;

    [unroll]
        for (int i = 0; i < octaves; ++i)
        {
            sum += valueNoise3D(p * freq) * amp;
            ampSum += amp;
            freq *= lacunarity;
            amp *= gain;
        }

    return sum / max(0.00001, ampSum);
}

// --- MAPPING ---
float3 uvToTorus(float2 uv, float R, float r)
{
    float theta = 2.0 * PI * uv.x;
    float phi = 2.0 * PI * uv.y;
    float ct = cos(theta), st = sin(theta);
    float cp = cos(phi), sp = sin(phi);
    return float3((R + r * cp) * ct, (R + r * cp) * st, r * sp);
}

// --- COLOR PALETTE ---
float3 heightPalette(float h)
{
    float3 low = float3(0.05, 0.12, 0.03);
    float3 mid = float3(0.45, 0.30, 0.18);
    float3 high = float3(0.9, 0.95, 0.98);
    return (h < 0.4) ? lerp(low, mid, h / 0.4) : lerp(mid, high, (h - 0.4) / 0.6);
}

// --- DOMAIN WARP ---
float3 domainWarp(float3 p)
{
    float3 w;
    w.x = fbm3D(p * 4.0, 3, 2.0, 0.6);
    w.y = fbm3D(p * 8.0 + 13.7, 3, 2.0, 0.5);
    w.z = fbm3D(p * 4.0 + 7.9, 3, 2.0, 0.6);
    return w * 0.35;
}

// --- MAIN PIXEL SHADER ---
float4 PS(VS_OUTPUT IN) : SV_Target
{
    const float R = 2.0;
    const float r = 0.9;
    const float baseScale = 0.8;

    float2 uv = frac(IN.uv);
    float3 p = uvToTorus(uv, R, r) * baseScale;

    // Warp
    p += domainWarp(p);

    // Terrain base and details
    float low = fbm3D(p * 0.4, 5, 2.0, 0.5);
    float detail = fbm3D(p * 2.0, 4, 2.0, 0.5);
    float ridged = pow(saturate(1.0 - abs(detail)), 1.5);

    float h_raw = low * 0.75 + ridged * 0.25;
    float h = saturate(smoothstep(-0.3, 0.9, h_raw));

    float3 color = heightPalette(h);
    return float4(color, 1.0);
}
