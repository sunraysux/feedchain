Texture2D heightMap : register(t0);
SamplerState sampLinear : register(s0);

cbuffer camera : register(b3)
{
    float4x4 world[2];
    float4x4 view[2];
    float4x4 proj[2];
};

cbuffer drawer : register(b5)
{
    float4 gConst[4002];
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float height : TEXCOORD1;
    float2 wpos : TEXCOORD2;
};

VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    int gridX = gConst[0].x;
    int gridY = gConst[0].y;
    int base_rangex = gConst[1].x;
    int base_rangey = gConst[1].y;
    int centerChunkX = gConst[0].z;
    int centerChunkY = gConst[0].w;

    int quadID = vID / 6;
    int localVertex = vID % 6;

    int layerStartID[5] = { 0, 1, 9, 25, 49 }; 
    int layer = 0;

    for (int i = 4; i >= 0; i--)
    {
        if (iID >= layerStartID[i])
        {
            layer = i;
            break;
        }
    }

    int size = layer * 2 + 1;
    int half = size / 2;
    int localID = iID - layerStartID[layer];

    int tileX, tileY;

    if (layer == 0)
    {
        
        tileX = 0;
        tileY = 0;
    }
    else
    {
        
        int sideLength = size - 1; 
        int totalSide = size * 4 - 4; 

        if (localID < sideLength)
        {
            
            tileX = localID - half;
            tileY = -half;
        }
        else if (localID < sideLength * 2)
        {
            
            tileX = half;
            tileY = (localID - sideLength) - half;
        }
        else if (localID < sideLength * 3)
        {
            
            tileX = half - (localID - sideLength * 2);
            tileY = half;
        }
        else
        {
            
            tileX = -half;
            tileY = half - (localID - sideLength * 3);
        }
    }

    
    float2 offset;
    if (localVertex == 0) offset = float2(0, 0);
    else if (localVertex == 1) offset = float2(1, 0);
    else if (localVertex == 2) offset = float2(0, 1);
    else if (localVertex == 3) offset = float2(1, 0);
    else if (localVertex == 4) offset = float2(1, 1);
    else offset = float2(0, 1);

    // Смещение чанка относительно центра 
    float2 chunkWorldOffset = float2(tileX * 2048.0, tileY * 2048.0);

    float2 normalizedPos = float2(
        (quadID % gridX + offset.x) / gridX,
        (quadID / gridX + offset.y) / gridY
    );

    float2 p = float2(
        (normalizedPos.x - 0.5) * 2.0 * base_rangex + chunkWorldOffset.x,
        (normalizedPos.y - 0.5) * 2.0 * base_rangey + chunkWorldOffset.y
    );

    // UV текстуры 
    int textureTileX = (centerChunkX + tileX + 8) % 8;
    int textureTileY = (centerChunkY + tileY + 8) % 8;
    float2 tileOffset = float2(textureTileX, textureTileY);
    float tileSize = 1.0 / 8;

    float2 regionUV = (normalizedPos * tileSize) + tileOffset * tileSize;

    // Высота 
    float4 pos = float4(p, 0, 1);
    float height = heightMap.SampleLevel(sampLinear, regionUV, 0).r;
    float depth = heightMap.SampleLevel(sampLinear, regionUV, 0).g;

    float heightScale = 100;
    float depthScale = 40;
    pos.z += exp(height * 1.5) * heightScale;
    pos.z -= exp(depth * 1.5) * depthScale;

    output.wpos = pos.xy;
    output.pos = mul(pos, mul(view[0], proj[0]));
    output.height = (height * 100 - depth * 40) / 90;

    return output;
}