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
    float4 gConst[32];
};


struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;   // глобальные UV для heightMap
    float3 wpos : TEXCOORD1;   // мировые XY + waterLevel Z
};

VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    const float CHUNK_SIZE = 2048.0;
    const int TILE_COUNT = 8;

    float waterLevel = gConst[0].x;
    int centerChunkX = (int)gConst[0].y;
    int centerChunkY = (int)gConst[0].z;

    
    int layerStartID[5] = { 0,1,9,25,49 };
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

    
    float2 positions[6] = {
        float2(0,0), float2(1,0), float2(0,1),
        float2(1,0), float2(1,1), float2(0,1)
    };
    float2 localPos = positions[vID];

    
    float2 chunkOffset = float2(tileX, tileY) * CHUNK_SIZE;
    float2 worldXY = chunkOffset + (localPos - 0.5) * CHUNK_SIZE;

    
    float tileSize = 1.0 / TILE_COUNT;
    int textureTileX = (centerChunkX + tileX + TILE_COUNT) % TILE_COUNT;
    int textureTileY = (centerChunkY + tileY + TILE_COUNT) % TILE_COUNT;
    float2 tileOffset = float2(textureTileX, textureTileY) * tileSize;
    float2 regionUV = localPos * tileSize + tileOffset;

    float4 worldPos = float4(worldXY.x, worldXY.y, waterLevel*300, 1.0);

    output.pos = mul(worldPos, mul(view[0], proj[0]));
    output.uv = regionUV;
    output.wpos = float3(worldXY, waterLevel*40);

    return output;
}
