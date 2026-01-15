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
    float normalizedX = (x) / 30000.0;
    float normalizedY = (y) / 30000.0;

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
    float normalizedX = (worldX) / 30000.0f;
    float normalizedY = (worldY) / 30000.0f;

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



class Chank {
private:
    std::vector<uint32_t> activeList;
    std::vector<uint32_t> typeList;

public:
    // Добавить рыбу
    void AddFish(uint32_t fishId, uint32_t Tt) {
        // Проверяем, нет ли уже этой рыбы в чанке
        if (!ContainsFish(fishId)) {
            activeList.push_back(fishId);
            typeList.push_back(Tt);
        }
    }

    // Удалить рыбу
    void RemoveFish(uint32_t fishId) {
        auto it = std::find(activeList.begin(), activeList.end(), fishId);
        if (it != activeList.end()) {
            *it = activeList.back();
            activeList.pop_back();
        }
    }

    // Проверить наличие рыбы
    bool ContainsFish(uint32_t fishId) const {
        return std::find(activeList.begin(), activeList.end(), fishId) != activeList.end();
    }

    // Получить количество рыб
    size_t GetFishCount() const {
        return activeList.size();
    }

    // Получить список рыб
    const std::vector<uint32_t>& GetFishList() const {
        return activeList;
    }
    std::vector<uint32_t> GetObjectsByType(int type) const {
        std::vector<uint32_t> result;

        // Собираем все ID, у которых нужный тип
        for (size_t i = 0; i < activeList.size(); ++i) {
            if (typeList[i] == type) {  // Используем индекс i, а не значение id
                result.push_back(activeList[i]);
            }
        }

        return result;
    }

    // Очистить чанк
    void Clear() {
        activeList.clear();
    }
};

Chank chank[300][300][15];
class FishSystem {
private:
    std::vector<float> x;       // Позиция X
    std::vector<float> y;       // Позиция Y  
    std::vector<float> z;       // Позиция Z (глубина)
    std::vector<float> sz;  
    std::vector<bool> gender;
    std::vector<bool> Active;
    std::vector<bool> dead;   
    std::vector<uint32_t> freeIndices;
    std::vector<uint32_t> activeList; 
    std::vector<uint32_t> idtexture;
    std::vector<uint32_t> age;
    std::vector<uint32_t> hunger;
    std::vector<uint32_t> nutritional_value;
    std::vector<uint32_t> ww;
    std::vector<uint32_t> CchunkX;
    std::vector<uint32_t> CchunkY;
    std::vector<uint32_t> CchunkZ;
    std::vector<uint32_t> maturity_age;
    std::vector<uint32_t> birth_tick;
    std::vector<uint32_t> MATURITY_TICKS;
    std::vector<uint32_t> deadlist;
    struct NewFish {
        float x, y, z;
        int tex;
        int years;
        float size;
    };
    std::vector<NewFish> newFishes;

public:
    // Добавить рыбу - возвращает ID
    uint32_t AddFish(float X, float Y, float Z, int tex, int years,int nut, float size = 100.0f) {
        uint32_t id;

        if (!freeIndices.empty()) {
            // Используем освобождённый индекс
            id = freeIndices.back();
            freeIndices.pop_back();
            gender[id] = Random::Int(0, 1);
            age[id] = years;
            x[id] = X;
            y[id] = Y;
            z[id] = Z;
            sz[id] = size;
            idtexture[id] = tex;
            maturity_age[id] = 100;
            birth_tick[id] = tick;
            MATURITY_TICKS[id] = 100;
            nutritional_value[id] = nut;
            hunger[id] = 0;
            dead[id] = 0;
            ww[id] = 0;
            Active[id] = true;
            CchunkX[id] = coord_to_chunk(X);
            CchunkY[id] = coord_to_chunk(Y);
            CchunkZ[id] = coord_to_chunk(Z);
        }
        else {
            // Добавляем новый элемент
            id = x.size();
            age.push_back(years);
            x.push_back(X);
            y.push_back(Y);
            z.push_back(Z);
            ww.push_back(0);
            gender.push_back(Random::Int(0, 1));
            idtexture.push_back(tex);
            hunger.push_back(0);
            sz.push_back(size);
            nutritional_value.push_back(nut);
            dead.push_back(0);
            maturity_age.push_back(100);
            birth_tick.push_back(tick);
            MATURITY_TICKS.push_back(100);
            Active.push_back(true);
            CchunkX.push_back(coord_to_chunk(X));
            CchunkY.push_back(coord_to_chunk(Y));
            CchunkZ.push_back(coord_to_chunk(Z));
        }
        
        
        activeList.push_back(id);
        chank[CchunkX[id]][CchunkY[id]][CchunkZ[id]].AddFish(id, idtexture[id]);
        return id;
    }

    // Удалить рыбу
    void RemoveFish(uint32_t id) {
        if (id >= Active.size() || !Active[id]) return;

        Active[id] = false;
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
        return id < Active.size() ? Active[id] : false;
    }

    float GetX(uint32_t id) const { return x[id]; }
    float GetY(uint32_t id) const { return y[id]; }
    float GetZ(uint32_t id) const { return z[id]; }
    float GetSize(uint32_t id) const { return sz[id]; }

    // Сеттеры
    void SetPosition(uint32_t id, float X, float Y, float Z) {
        x[id] = X;
        y[id] = Y;
        z[id] = Z;
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
        float margin = objectSize*1 ; // Коэффициент можно настроить

        return (screenPos.x >= -margin && screenPos.x <= Camera::state.width + margin &&
            screenPos.y >= -margin && screenPos.y <= Camera::state.height + margin &&
            XMVectorGetZ(objectPos) > 0); // Проверка глубины (перед камерой)
    }
    // Собрать данные для рендеринга (только активных рыб)
    void GatherRenderData(std::vector<XMFLOAT4>& outData, int textureType) {
        outData.clear();
        outData.reserve(activeList.size());

        const float WORLD_SIZE = 30000.0f; // Размер вашего мира

        for (uint32_t id : activeList) {
            if (int(idtexture[id]) != textureType) continue;

            float baseX = x[id];
            float baseY = y[id];
            float baseZ = z[id];
            float baseSize = sz[id];


            for (int dx = -1; dx <= 1 ; dx++) {
                for (int dy = -1; dy <= 1 ; dy++) {
                    float offsetX = baseX + dx * WORLD_SIZE;
                    float offsetY = baseY + dy * WORLD_SIZE;

                    XMVECTOR pos = XMVectorSet(offsetX, offsetY, baseZ, 0);

                    if (IsObjectVisible(pos, baseSize)) {
                        outData.emplace_back(
                            offsetX,
                            offsetY,
                            baseZ,
                            baseSize
                        );
                    }
                }
            }
        }
    }


    void process()
    {
        
        for (uint32_t id : activeList) {
            age[id]++;
            hunger[id]++;
            if (idtexture[id] == 55) {

                x[id] += Random::Float(-10, 10);
                y[id] += Random::Float(-10, 10);
                int zB = heightH(x[id], y[id]) + 50;
                CchunkX[id] = coord_to_chunk(x[id]);
                CchunkY[id] = coord_to_chunk(y[id]);
                CchunkZ[id] = coord_to_chunk(z[id]);

                if (age[id] < maturity_age[id] || (tick - birth_tick[id]) < MATURITY_TICKS[id] || dead[id]) continue;

                int base_cx = CchunkX[id];
                int base_cy = CchunkY[id];
                int base_cz = CchunkZ[id];
                for (auto& w : chank[base_cx][base_cy][base_cz].GetObjectsByType(idtexture[id]))
                {
                    if (w == id)continue;
                    if (dead[w] == 1) continue;
                    if (gender[w] == gender[id]) continue;
                    if (age[w] < maturity_age[w] || (tick - birth_tick[w]) < MATURITY_TICKS[w]) continue;

                    // расстояние с учётом тора
                    float dx = torusDeltaSigned(x[id], x[w], base_rangex);
                    float dy = torusDeltaSigned(y[id], y[w], base_rangey);
                    float dist2 = dx * dx + dy * dy;

                    if (dist2 < 500.0f * 500.0f) {

                        int xB = Wrap(x[id] + Random::Int(-500, 500), 30000);
                        int yB = Wrap(y[id] + Random::Int(-500, 500), 30000);
                        int zB = heightH(xB, yB) + 50;
                        bool gender = Random::Int(0, 1);

                        // Обновляем cooldown родителей
                        birth_tick[id] = tick;
                        birth_tick[w] = tick;

                        newFishes.emplace_back(NewFish{ (float)xB, (float)yB,(float)zB,(int)idtexture[id],0,100.0f });
                    }
                }
            }


            if (hunger[id] > 500)
                deadlist.push_back(id);

        }
        for (uint32_t id : deadlist) 
            RemoveFish(id);
        deadlist.clear();
        for (auto& newFish : newFishes)
            AddFish(newFish.x, newFish.y, newFish.z, newFish.tex, newFish.years, newFish.size);
        newFishes.clear();
    }
    // Очистить все данные
    void Clear() {
        x.clear();
        y.clear();
        z.clear();
        sz.clear();
        Active.clear();
        freeIndices.clear();
        activeList.clear();
    }
};

// Глобальный экземпляр
FishSystem g_fishSystem;