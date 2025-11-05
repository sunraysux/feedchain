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

// Ôóíêöèÿ äëÿ çàöèêëèâàíèÿ êîîðäèíàò
inline float Wrap(float x, float range) {
    float size = range * 2.0f; // ïîëíûé ðàçìåð ìèðà
    while (x >= range) x -= size;
    while (x < -range) x += size;
    return x;
}

VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    int gridX = gConst[0].x;
    int gridY = gConst[0].y;
    int base_rangex = gConst[1].x;
    int base_rangey = gConst[1].y;
    float2 cameraPos = float2(gConst[0].z, gConst[0].w);

    int quadID = vID / 6;
    int localVertex = vID % 6;

    const float AREA_SIZE = 32768.0;
    const float CHUNK_SIZE = AREA_SIZE / 8.0;

    int quadsPerChunk = gridX * gridY;
    int chunkID = quadID / quadsPerChunk;
    int localQuadID = quadID % quadsPerChunk;

    int tileX = chunkID % 8;
    int tileY = chunkID / 8;

    // ÔÈÊÑ: ÂÛÐÀÂÍÈÂÀÅÌ ÊÀÌÅÐÓ È ÑÌÅÙÀÅÌ ÄËß ÖÅÍÒÐÈÐÎÂÀÍÈß
    float2 alignedCameraPos;
    alignedCameraPos.x = floor(cameraPos.x / CHUNK_SIZE) * CHUNK_SIZE;
    alignedCameraPos.y = floor(cameraPos.y / CHUNK_SIZE) * CHUNK_SIZE;

    // ÔÈÊÑ: ÑÌÅÙÀÅÌ ÒÀÊ, ×ÒÎÁÛ ÖÅÍÒÐÀËÜÍÛÉ ×ÀÍÊ ÁÛË (0,0)
    // tileX îò 0 äî 7, tileY îò 0 äî 7
    // Õîòèì ÷òîáû êàìåðà áûëà â öåíòðå îáëàñòè (÷àíêè -3.5 äî +3.5)
    float2 chunkOffset = float2(tileX - 3.5, tileY - 3.5) * CHUNK_SIZE;

    // Ñìåùåíèå âíóòðè ÷àíêà
    float2 offset;
    if (localVertex == 0) offset = float2(0, 0);
    else if (localVertex == 1) offset = float2(0, 1);
    else if (localVertex == 2) offset = float2(1, 0);
    else if (localVertex == 3) offset = float2(1, 0);
    else if (localVertex == 4) offset = float2(0, 1);
    else offset = float2(1, 1);

    // Ïîçèöèÿ âíóòðè ÷àíêà
    float2 normalizedPos = float2(
        (localQuadID % gridX + offset.x) / gridX,
        (localQuadID / gridX + offset.y) / gridY
    );

    // Ìèðîâàÿ ïîçèöèÿ
    float2 localXY = chunkOffset + (normalizedPos - 0.5) * CHUNK_SIZE;
    float2 worldXY = alignedCameraPos + localXY;

    // ÇÀÖÈÊËÈÂÀÅÌ ÊÎÎÐÄÈÍÀÒÛ ÄËß UV
    float2 wrappedWorldXY;
    wrappedWorldXY.x = Wrap(worldXY.x, 16384.0);
    wrappedWorldXY.y = Wrap(worldXY.y, 16384.0);

    // Ïðåîáðàçóåì çàöèêëåííûå êîîðäèíàòû â UV
    float2 regionUV = (wrappedWorldXY + 16384.0) / 32768.0;

    // Âûñîòà
    float4 pos = float4(worldXY, 0, 1);
    float height = heightMap.SampleLevel(sampLinear, regionUV, 0).r;
    float depth = heightMap.SampleLevel(sampLinear, regionUV, 0).g;

    float heightScale = 200;
    float depthScale = 80;
    float worldZ = exp(height * 2) * heightScale - exp(depth * 2) * depthScale;
    pos.z = worldZ;

    output.wpos = pos.xy;
    output.pos = mul(pos, mul(view[0], proj[0]));
    output.height = (height * 100 - depth * 40) / 90;

    return output;
}