XMFLOAT2 sampleHeightDepthBilinear(const std::vector<XMFLOAT4>& data,
    int width, int height,
    float u, float v)
{
    float x = u * (width - 1);
    float y = v * (height - 1);

    int x0 = floor(x);
    int y0 = floor(y);
    int x1 = min(x0 + 1, width - 1);
    int y1 = min(y0 + 1, height - 1);

    float tx = x - x0;
    float ty = y - y0;

    auto get = [&](int X, int Y) { return data[Y * width + X]; };

    XMFLOAT4 h00 = get(x0, y0);
    XMFLOAT4 h10 = get(x1, y0);
    XMFLOAT4 h01 = get(x0, y1);
    XMFLOAT4 h11 = get(x1, y1);

    float h0 = h00.x * (1 - tx) + h10.x * tx;
    float h1 = h01.x * (1 - tx) + h11.x * tx;
    float h = h0 * (1 - ty) + h1 * ty;

    float d0 = h00.y * (1 - tx) + h10.y * tx;
    float d1 = h01.y * (1 - tx) + h11.y * tx;
    float d = d0 * (1 - ty) + d1 * ty;

    return { h, d };
}
float heightH(float x, float y)
{
    auto& heightMap = Textures::Texture[1];

    // 1. Нормализация
    float normalizedX = (x) / 32768.0f;
    float normalizedY = (y) / 32768.0f;

    // 2. Жёсткое ограничение
    normalizedX = clamp(normalizedX, 0.0f, 1.0f);
    normalizedY = clamp(normalizedY, 0.0f, 1.0f);

    // 3. Текстурные координаты
    UINT texX = static_cast<UINT>(normalizedX * (heightMap.size.x - 1));
    UINT texY = static_cast<UINT>(normalizedY * (heightMap.size.y - 1));

    // 4. ОДИНАКОВОЕ ограничение
    texX = min(texX, static_cast<UINT>(heightMap.size.x - 1));
    texY = min(texY, static_cast<UINT>(heightMap.size.y - 1));
    XMFLOAT2 hd = sampleHeightDepthBilinear(heightMap.cpuData, heightMap.size.x, heightMap.size.y, normalizedX, normalizedY);

    float height = hd.x;
    float depth = hd.y;

    float heightScale = 1500;
    height = height * heightScale;
    return height;
}
bool heightW(float worldX, float worldY) {
    auto& heightMap = Textures::Texture[1];

    // 1. Нормализация
    float normalizedX = (worldX) / 32768.0f;
    float normalizedY = (worldY) / 32768.0f;

    // 2. Жёсткое ограничение
    normalizedX = clamp(normalizedX, 0.0f, 1.0f);
    normalizedY = clamp(normalizedY, 0.0f, 1.0f);

    // 3. Текстурные координаты
    UINT texX = static_cast<UINT>(normalizedX * (heightMap.size.x - 1));
    UINT texY = static_cast<UINT>(normalizedY * (heightMap.size.y - 1));

    // 4. ОДИНАКОВОЕ ограничение
    texX = min(texX, static_cast<UINT>(heightMap.size.x - 1));
    texY = min(texY, static_cast<UINT>(heightMap.size.y - 1));
    XMFLOAT2 hd = sampleHeightDepthBilinear(heightMap.cpuData, heightMap.size.x, heightMap.size.y, normalizedX, normalizedY);

    float height = hd.x;
    float depth = hd.y;

    float heightScale = 1500;
    height = height * heightScale;

    return height < waterLevel;
}


// ==================== OPTIMIZED FISH SYSTEM (SoA) ====================
class FishSystem {
private:
    // Structure of Arrays (SoA) - для лучшей производительности
    std::vector<float> fishX;       // Позиция X
    std::vector<float> fishY;       // Позиция Y  
    std::vector<float> fishZ;       // Позиция Z (глубина)
    std::vector<float> fishSizes;   // Размер рыб
    std::vector<bool> fishActive;   // Активна ли рыба
    std::vector<uint32_t> freeIndices; // Свободные индексы для повторного использования
    std::vector<uint32_t> activeList;  // Список активных индексов
    std::vector<uint32_t> idtexture;
    std::vector<uint32_t> age;
    std::vector<uint32_t> ww;

public:
    // Добавить рыбу - возвращает ID
    uint32_t AddFish(float x, float y, float z, int tex, int years, float size = 100.0f) {
        uint32_t id;

        if (!freeIndices.empty()) {
            // Используем освобождённый индекс
            id = freeIndices.back();
            freeIndices.pop_back();
            age[id] = years;
            fishX[id] = x;
            fishY[id] = y;
            fishZ[id] = z;
            fishSizes[id] = size;
            idtexture[id] = tex;
            ww[id] = 0;
            fishActive[id] = true;
        }
        else {
            // Добавляем новый элемент
            id = fishX.size();
            age.push_back(years);
            fishX.push_back(x);
            fishY.push_back(y);
            fishZ.push_back(z);
            ww.push_back(0);
            idtexture.push_back(tex);
            fishSizes.push_back(size);
            fishActive.push_back(true);
        }

        activeList.push_back(id);
        return id;
    }

    // Удалить рыбу
    void RemoveFish(uint32_t id) {
        if (id >= fishActive.size() || !fishActive[id]) return;

        fishActive[id] = false;
        freeIndices.push_back(id);

        // Удаляем из активного списка
        auto it = std::find(activeList.begin(), activeList.end(), id);
        if (it != activeList.end()) {
            *it = activeList.back();
            activeList.pop_back();
        }
    }

    // Геттеры
    bool IsActive(uint32_t id) const {
        return id < fishActive.size() ? fishActive[id] : false;
    }

    float GetX(uint32_t id) const { return fishX[id]; }
    float GetY(uint32_t id) const { return fishY[id]; }
    float GetZ(uint32_t id) const { return fishZ[id]; }
    float GetSize(uint32_t id) const { return fishSizes[id]; }

    // Сеттеры
    void SetPosition(uint32_t id, float x, float y, float z) {
        fishX[id] = x;
        fishY[id] = y;
        fishZ[id] = z;
    }

    // Получить количество активных рыб
    size_t GetActiveCount() const {
        return activeList.size();
    }
    // Преобразование мировых координат в экранные
    XMFLOAT2 WorldToScreen(const XMVECTOR& worldPos)
    {
        XMMATRIX viewProj = Camera::state.viewMatrix * Camera::state.projMatrix;
        XMVECTOR clipPos = XMVector3TransformCoord(worldPos, viewProj);

        float x = XMVectorGetX(clipPos);
        float y = XMVectorGetY(clipPos);

        // Преобразование в экранные координаты
        float screenX = (x + 1.0f) * 0.5f * Camera::state.width;
        float screenY = (1.0f - y) * 0.5f * Camera::state.height;

        return XMFLOAT2(screenX, screenY);
    }

    // Проверка видимости объекта с учетом размера
    bool IsObjectVisible(const XMVECTOR& objectPos, float objectSize)
    {
        // Преобразуем позицию объекта в экранные координаты
        XMFLOAT2 screenPos = WorldToScreen(objectPos);

        // Проверяем, находится ли в пределах экрана с запасом
        float margin = objectSize*10 ; // Коэффициент можно настроить

        return (screenPos.x >= -margin && screenPos.x <= Camera::state.width + margin &&
            screenPos.y >= -margin && screenPos.y <= Camera::state.height + margin &&
            XMVectorGetZ(objectPos) > 0); // Проверка глубины (перед камерой)
    }
    // Собрать данные для рендеринга (только активных рыб)
    void GatherRenderData(std::vector<XMFLOAT4>& outData,int i) {
        outData.clear();
        outData.reserve(activeList.size());

        for (uint32_t id : activeList) {

            if (int(idtexture[id]) == i && IsObjectVisible(XMVectorSet( fishX[id],fishY[id], fishZ[id],0),100 ))
            {
                outData.emplace_back(
                    fishX[id],
                    fishY[id],
                    fishZ[id],
                    fishSizes[id]
                );
            }
        }
    }
    void process()
    {
        
        for (uint32_t id : activeList) {
            age[id]+=Random::Int(0,1000);
            if (!heightW(fishX[id], fishY[id])|| fishZ[id]>500) {
                fishX[id] += Random::Float(-10, 10);
                fishY[id] += Random::Float(-10, 10);
                fishZ[id] = heightH(fishX[id], fishY[id]);
                ww[id]+=Random::Int(-1, 2);
            }
            if (age[id] > 1000000||ww[id]>1000)
                RemoveFish(id);
        }
        


    }

    // Очистить все данные
    void Clear() {
        fishX.clear();
        fishY.clear();
        fishZ.clear();
        fishSizes.clear();
        fishActive.clear();
        freeIndices.clear();
        activeList.clear();
    }
};

// Глобальный экземпляр
FishSystem g_fishSystem;