bool heightW(float x, float y) {

    auto& heightMap = Textures::Texture[10];
    float normalizedX = (x + base_rangex) / (2.0f * base_rangex); // [0, 1]
    float normalizedY = (y + base_rangey) / (2.0f * base_rangey); // [0, 1]
    float u = normalizedX / 4.0f;
    float v = normalizedY / 4.0f;

    UINT texX = static_cast<UINT>(u * heightMap.size.x) % static_cast<UINT>(heightMap.size.x);
    UINT texY = static_cast<UINT>(v * heightMap.size.y) % static_cast<UINT>(heightMap.size.y);

    float height = heightMap.cpuData[texY * static_cast<UINT>(heightMap.size.x) + texX];
    return height < waterLevel;
}

class Grass : public Creature {
public:
    Grass() : Creature(type_::grass) {
        // Инициализация параметров растения
        nutritional_value = 200;
        maturity_age = 250;
        age_limit = 500;
        age = 0;
    }

    void reproduce(std::vector<std::shared_ptr<Grass>>& all_grass,
        std::vector<std::shared_ptr<Grass>>& new_creatures) {

        if (!Random::FastChance(0.05f)) return;

        int seeds = Random::Int(1, 5);
        float seedlingx=x;
        float seedlingy=y;
        auto& heightMap = Textures::Texture[10];
        const UINT texW = static_cast<UINT>(heightMap.size.x);
        const UINT texH = static_cast<UINT>(heightMap.size.y);
        for (int s = 0; s < seeds; s++) {
            float a = Random::Float(0.0f, 2.0f * 3.14159265f);
            float dist = Random::Float(5.0f, 20.0f);
            float dx = cosf(a) * dist;
            float dy = sinf(a) * dist;
            float seedlingx = x + dx;
            float seedlingy = y + dy;
             seedlingx = Wrap(seedlingx, base_rangex);
            seedlingy = Wrap(seedlingy, base_rangey);

            // Проверка минимального расстояния (например, 2.0f)

            float normalizedX = (seedlingx + base_rangex) / (2.0f * base_rangex)/4; // [0,1]
            float normalizedY = (seedlingy + base_rangey) / (2.0f * base_rangey)/4; // [0,1]

            // clamp и перевод в индекс (избегаем %)
            UINT texX = static_cast<UINT>(min(max(normalizedX, 0.0f) * (texW - 1), (double)(texW - 1)));
            UINT texY = static_cast<UINT>(min(max(normalizedY, 0.0f) * (texH - 1), (double)(texH - 1)));

            float height = heightMap.cpuData[texY * static_cast<UINT>(heightMap.size.x) + texX];
            if (height < waterLevel + Random::Float(-0.1, 0.1)) continue;
            // Проверка плотности (например, не более 10 растений в радиусе 5)
            int xc = coord_to_chunkx(seedlingx);
            int yc = coord_to_chunky(seedlingy);

            float radius = 15.0f;
            float radius2 = radius * radius;

            int totalGrass = 0;
            int MAX_PLANTS_PER_CHUNK=1;


            // проходим по траве в чанке
            for (auto& gWeak : chunk_grid[xc][yc].Plants) {
                if (auto g = gWeak.lock()) { // проверяем, что объект жив
                    float dx = g->x - seedlingx;
                    float dy = g->y - seedlingy;
                    for (int i = -1;i < 1;i++) {
                        for (int j = -1;j < 1;j++) {
                            xc = coord_to_chunkx(seedlingx + i * CHUNK_SIZE);
                            yc = coord_to_chunky(seedlingy + j * CHUNK_SIZE);
                            int totalGrass1 = static_cast<int>(chunk_grid[xc][yc].Plants.size());
                            
                            totalGrass += totalGrass1;
                            if (totalGrass1 > MAX_PLANTS_PER_CHUNK) break;
                        }
                        if (totalGrass > 1) break;
                    }
                }
            }



            if (totalGrass > 1) continue;
            auto seedling = std::make_shared<Grass>();
            seedling->x = seedlingx;
            seedling->y = seedlingy;
            // установить базовые значения
            seedling->age = 0;
            seedling->birth_tick = tick;
            seedling->updateChunk();
            new_creatures.push_back(std::move(seedling));


        }
    }


    void eat() {} // Растения не едят

    bool shouldDie() const override {
        return dead || age > age_limit;
    }

    void process(std::vector<std::shared_ptr<Grass>>& creatures,
        std::vector<std::shared_ptr<Grass>>& new_grass,
        PopulationManager& pop) {
        if  (Random::FastChance(0.1f)) {
            if (shouldDie()) return;
        }
        age++;
        if (age >= maturity_age) {
            if (tick % 5 == 0) {
                reproduce(creatures, new_grass);
            }
        }
    }
protected:
    std::vector<std::weak_ptr<Creature>>& getChunkContainer(Chunk& chunk) override {
        return chunk.grass;
    }
    std::vector<std::weak_ptr<Creature>>& getChunkContainer2(Chunk& chunk) override {
        return chunk.Plants;
    }
    void addToChunk(Chunk& chunk) override {
        chunk.grass.push_back(weak_from_this());
        chunk.Plants.push_back(weak_from_this());
    }
};

class Tree : public Creature {
public:
    Tree() : Creature(type_::tree) {
        // Инициализация параметров растения
        nutritional_value = 100;
        age = 0;
        maturity_age = 1000;
        age_limit = 5000;
    }

    void reproduce(std::vector<std::shared_ptr<Tree>>& all_trees,
        std::vector<std::shared_ptr<Tree>>& new_creatures) {

        float reproductionChance = min(0.01f * (age - maturity_age), 0.05f);
        if ((Random::Int(0, 100)) >= (reproductionChance * 100))
            return;

        int seeds = Random::Int(1, 5);

        for (int s = 0; s < seeds; s++) {
            auto seedling = std::make_shared<Tree>();
            float a = Random::Float(0, 2 * 3.14159265f);
            float dX = cosf(a); float dY = sinf(a);
            seedling->x = x + dX * 20;
            seedling->y = y + dY * 20;
            // Обрезка по границам
            seedling->x = Wrap(seedling->x, base_rangex);
            seedling->y = Wrap(seedling->y, base_rangey);

            auto& heightMap = Textures::Texture[10];
            float normalizedX = (seedling->x + base_rangex) / (2.0f * base_rangex); // [0, 1]
            float normalizedY = (seedling->y + base_rangey) / (2.0f * base_rangey); // [0, 1]
            float u = normalizedX / 4.0f;
            float v = normalizedY / 4.0f;

            UINT texX = static_cast<UINT>(u * heightMap.size.x) % static_cast<UINT>(heightMap.size.x);
            UINT texY = static_cast<UINT>(v * heightMap.size.y) % static_cast<UINT>(heightMap.size.y);

            float height = heightMap.cpuData[texY * static_cast<UINT>(heightMap.size.x) + texX];
            if (height < waterLevel + Random::Float(-0.1, 0.1)) continue;
            int xc = coord_to_chunkx(seedling->x);
            int yc = coord_to_chunky(seedling->y);

            float radius = 15.0f;
            float radius2 = radius * radius;

            int totalGrass = 0;



            // проходим по траве в чанке
            for (auto& gWeak : chunk_grid[xc][yc].Plants) {
                if (auto g = gWeak.lock()) { // проверяем, что объект жив
                    float dx = g->x - seedling->x;
                    float dy = g->y - seedling->y;
                    for (int i = -1;i < 1;i++) {
                        for (int j = -1;j < 1;j++) {
                            xc = coord_to_chunkx(seedling->x + i * CHUNK_SIZE);
                            yc = coord_to_chunky(seedling->y + j * CHUNK_SIZE);
                            int plant = chunk_grid[xc][yc].countCreatures(chunk_grid[xc][yc].Plants);
                            if (plant > 0)
                                totalGrass += plant;
                        }

                    }
                }
            }



            if (totalGrass > 1) continue;
            updateChunk();
            new_creatures.push_back(seedling);


        }
    }


    void eat(std::vector<std::shared_ptr<Creature>>& creatures) {} // Растения не едят

    bool shouldDie() const override {
        return dead || age > age_limit;
    }

    void process(std::vector<std::shared_ptr<Tree>>& creatures,
        std::vector<std::shared_ptr<Tree>>& new_trees,
        PopulationManager& pop) {
        if (shouldDie()) return;
        age++;
        if (age >= maturity_age && pop.canAddTree(static_cast<int>(new_trees.size()))) {
            reproduce(creatures, new_trees);
        }
    }
protected:
    std::vector<std::weak_ptr<Creature>>& getChunkContainer(Chunk& chunk) override {
        return chunk.trees;
    }
    std::vector<std::weak_ptr<Creature>>& getChunkContainer2(Chunk& chunk) override {
        return chunk.Plants;
    }

    void addToChunk(Chunk& chunk) override {
        chunk.trees.push_back(weak_from_this());
        chunk.Plants.push_back(weak_from_this());
    }
};

class Bush : public Creature {
public:
    Bush() : Creature(type_::bush) {
        // Инициализация параметров растения
        nutritional_value = 200;
        age = 0;
        maturity_age = 500;
        age_limit = 1000;
        berry_count = 0;
        blossoming_age = 80;
        berry_limit = 5;
    }
    
  

    void blossoming() {
        float reproductionChance = min(0.01f * (age - maturity_age), 0.05f);
        if ((Random::Int(0, 100)) >= (reproductionChance * 100))
            return;
        berry_count++;
    }
    void reproduce(std::vector<std::shared_ptr<Bush>>& all_bushs,
        std::vector<std::shared_ptr<Bush>>& new_creatures) {

        float reproductionChance = min(0.01f * (age - maturity_age), 0.05f);
        if ((Random::Int(0, 100)) >= (reproductionChance * 100))
            return;

        int seeds = Random::Int(1, 5);

        for (int s = 0; s < seeds; s++) {
            auto seedling = std::make_shared<Bush>();
            float a = Random::Float(0, 2 * 3.14159265f);
            float dX = cosf(a); float dY = sinf(a);
            seedling->x = x + dX * 20;
            seedling->y = y + dY * 20;
            // Обрезка по границам
            seedling->x = Wrap(seedling->x, base_rangex);
            seedling->y = Wrap(seedling->y, base_rangey);

            auto& heightMap = Textures::Texture[10];
            float normalizedX = (seedling->x + base_rangex) / (2.0f * base_rangex); // [0, 1]
            float normalizedY = (seedling->y + base_rangey) / (2.0f * base_rangey); // [0, 1]
            float u = normalizedX / 4.0f;
            float v = normalizedY / 4.0f;

            UINT texX = static_cast<UINT>(u * heightMap.size.x) % static_cast<UINT>(heightMap.size.x);
            UINT texY = static_cast<UINT>(v * heightMap.size.y) % static_cast<UINT>(heightMap.size.y);

            float height = heightMap.cpuData[texY * static_cast<UINT>(heightMap.size.x) + texX];
            if (height < waterLevel + Random::Float(-0.1, 0.1)) continue;
            int xc = coord_to_chunkx(seedling->x);
            int yc = coord_to_chunky(seedling->y);

            float radius = 15.0f;
            float radius2 = radius * radius;

            int totalGrass = 0;



            // проходим по траве в чанке
            for (auto& gWeak : chunk_grid[xc][yc].Plants) {
                if (auto g = gWeak.lock()) { // проверяем, что объект жив
                    float dx = g->x - seedling->x;
                    float dy = g->y - seedling->y;
                    for (int i = -1;i < 1;i++) {
                        for (int j = -1;j < 1;j++) {
                            xc = coord_to_chunkx(seedling->x + i * CHUNK_SIZE);
                            yc = coord_to_chunky(seedling->y + j * CHUNK_SIZE);
                            int plant = chunk_grid[xc][yc].countCreatures(chunk_grid[xc][yc].Plants);
                            if (plant > 0)
                                totalGrass += plant;
                        }

                    }
                }
            }



            if (totalGrass > 1) continue;
            updateChunk();
            new_creatures.push_back(seedling);


        }
    }



    void eat(std::vector<std::shared_ptr<Creature>>& creatures) {} // Растения не едят

    bool shouldDie() const override {
        return dead || age > age_limit;
    }

    void process(std::vector<std::shared_ptr<Bush>>& creatures,
        std::vector<std::shared_ptr<Bush>>& new_bushes,
        PopulationManager& pop) {
        if (shouldDie()) return;
        age++;
        if (age >= blossoming_age && berry_count < berry_limit) {
            blossoming();
        }
        if(age >= maturity_age && pop.canAddBush(static_cast<int>(new_bushes.size())) && berry_count>0) {
            reproduce(creatures, new_bushes);
        }
    }
protected:
    std::vector<std::weak_ptr<Creature>>& getChunkContainer(Chunk& chunk) override {
        return chunk.bushes;
    }
    std::vector<std::weak_ptr<Creature>>& getChunkContainer2(Chunk& chunk) override {
        return chunk.Plants;
    }
    void addToChunk(Chunk& chunk) override {
        chunk.bushes.push_back(weak_from_this());
        chunk.Plants.push_back(weak_from_this());
    }
};

class Rabbit : public Creature {
public:
    Rabbit() : Creature(type_::rabbit) {
        nutritional_value = 1000;
        gender = (Random::Int(0,1)==0) ? gender_::male : gender_::female;
        eating_range = 2;
        age = 0;
        maturity_age = 500;
        age_limit = 1500;
        hunger_limit = 400;
        hunger = 0;
        birth_tick = tick;

        isDirectionSelect = false;
        step = 0;
        nextPositionX = 0;
        nextPositionY = 0;
        move_range = 2;

    }
    int stepsTick = 0;
    float dirX = 0.0f, dirY = 0.0f;
    int remainingSteps = 0;
    float speed = 1.0f;
    int MATURITY_TICKS = 200;
    void move(std::vector<std::shared_ptr<Rabbit>>& new_rabbits,
        PopulationManager& pop) {
        const float avoidance_radius = 5.0f;
        const float avoidanceStrength = 0.5f;

        bool isHunger = hunger > 200;
        

        bool isMaturity = (age >= maturity_age) &&
            (tick - birth_tick > MATURITY_TICKS) &&
            !isHunger;
        //  избегание соседей 
        float ax = 0.0f, ay = 0.0f;
        int nearbyCount = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                int ncx = coord_to_chunkx(Wrap(x + i * CHUNK_SIZE, base_rangex));
                int ncy = coord_to_chunky(Wrap(y + j * CHUNK_SIZE, base_rangey));
                auto t = chunk_grid[ncx][ncy].nearly_creature_square(chunk_grid[ncx][ncy].rabbits, x, y, avoidance_radius);
                nearbyCount += std::get<0>(t);
                ax += std::get<1>(t);
                ay += std::get<2>(t);
            }
        }

        if (nearbyCount > 0) {
            ax /= nearbyCount;
            ay /= nearbyCount;
            float len = std::sqrt(ax * ax + ay * ay);
            if (len > 1e-6f) {
                ax = (ax / len) * avoidanceStrength;
                ay = (ay / len) * avoidanceStrength;
            }
        }
        if (remainingSteps <= 0||tick-stepsTick>10) {
            if (isHunger) {
                auto bush = searchNearestCreature(x, y, type_::grass, 3, false, gender);
                if (bush.first != -5000.0f) {
                    float dx = torusDelta(x, bush.first, base_rangex);
                    float dy = torusDelta(y, bush.second, base_rangey);
                    float d = std::sqrt(dx * dx + dy * dy);
                    if (d > 1e-6f) {
                        dirX = dx / d;
                        dirY = dy / d;

                        // сколько шагов нужно до куста
                        remainingSteps = static_cast<int>(d / speed);

                        // чтобы не застревал рядом, добавь 1-2 шага про запас
                        remainingSteps = max(1, remainingSteps);
                        stepsTick = tick;
                    }
                }
            }
            else if (isMaturity && pop.canAddRabbit(static_cast<int>(new_rabbits.size()))) {
                auto target = searchNearestCreature(x, y, type_::rabbit, 3, true, gender);
                if (target.first != -5000.0f) {
                    float dx = torusDelta(x, target.first, base_rangex);
                    float dy = torusDelta(y, target.second, base_rangey);
                    float d = std::sqrt(dx * dx + dy * dy);
                    if (d > 1e-6f) {
                        dirX = dx / d;
                        dirY = dy / d;

                        // сколько шагов нужно до куста
                        remainingSteps = static_cast<int>(d / speed);

                        remainingSteps = max(1, remainingSteps);
                        stepsTick = tick;
                    }
                }
            }
            // если всё ещё нет направления — случайный угол
            if (remainingSteps <= 0) {
                float a = Random::Float(0, 2 * 3.14159265f);
                dirX = cosf(a); dirY = sinf(a);
                remainingSteps = 20;
                stepsTick = tick;
            }
        }

        // вычисляем steering: основной dir + небольшое избегание
        float steerX = dirX + ax * 0.7f; // ax,ay — из расчёта избегания соседей
        float steerY = dirY + ay * 0.7f;
        float slen = std::sqrt(steerX * steerX + steerY * steerY);
        if (slen > 1e-6f) { steerX /= slen; steerY /= slen; }

        // пробуем сделать один маленький шаг
        float candX = Wrap(x + steerX * speed, base_rangex);
        float candY = Wrap(y + steerY * speed, base_rangey);

        if (heightW(candX, candY)) {
            // вода прямо впереди — пробуем обход в стороны (+/- 45°)
            float baseAngle = atan2f(dirY, dirX);
            bool found = false;
            const float offsets[4] = { 0.5f, -0.5f, 1.0f, -1.0f }; // в радианах ~30°, -30°, 60°, -60°
            for (float off : offsets) {
                float na = baseAngle + off;
                float nx = cosf(na), ny = sinf(na);
                float tx = Wrap(x + nx * speed, base_rangex);
                float ty = Wrap(y + ny * speed, base_rangey);
                if (!heightW(tx, ty)) { dirX = nx; dirY = ny; found = true; break; }
            }
            if (!found) {
                // застряли — сбрасываем направление, в следующем тике выберется новое
                remainingSteps = 0;
            }
        }
        else {
            // шаг возможен — совершаем его
            x = candX; y = candY;
            remainingSteps--;
            updateChunk();
        }
    }

    void reproduce(std::vector<std::shared_ptr<Rabbit>>& rabbits,
        std::vector<std::shared_ptr<Rabbit>>& new_rabbits) {
        if (age < maturity_age || (tick - birth_tick) < MATURITY_TICKS || dead) return;

        int base_cx = coord_to_chunkx(x);
        int base_cy = coord_to_chunky(y);

        for (auto& w : chunk_grid[base_cx][base_cy].rabbits) {
            if (auto partner = w.lock()) {
                if (partner.get() == this || partner->dead) continue;
                if (partner->gender == gender) continue;
                if (partner->age < maturity_age || (tick - partner->birth_tick) < MATURITY_TICKS) continue;

                // расстояние с учётом тора
                float dx = torusDelta(x, partner->x, base_rangex);
                float dy = torusDelta(y, partner->y, base_rangey);
                float dist2 = dx * dx + dy * dy;

                if (dist2 < 50.0f*50.0f) {
                    auto offspring = std::make_shared<Rabbit>();
                    offspring->x = Wrap(x + Random::Int(-5, 5), base_rangex);
                    offspring->y = Wrap(y + Random::Int(-5, 5), base_rangey);
                    offspring->birth_tick = tick;
                    offspring->gender = (rand() % 2 == 0) ? gender_::male : gender_::female;

                    // Обновляем cooldown родителей
                    birth_tick = tick;
                    partner->birth_tick = tick;

                    // разнесение, чтобы не слипались
                    const float nudge = 5.0f;
                    float nd = std::sqrt(dx * dx + dy * dy);
                    do {
                        nextPositionX = Random::Int(-move_range, move_range);
                        nextPositionY = Random::Int(-move_range, move_range);
                    } while (nextPositionX == 0 && nextPositionY == 0);
                    isDirectionSelect = true;
                    step = Random::Int(5, 15);

                    do {
                        partner->nextPositionX = Random::Int(-move_range, move_range);
                        partner->nextPositionY = Random::Int(-move_range, move_range);
                    } while (partner->nextPositionX == 0 && partner->nextPositionY == 0);
                    partner->isDirectionSelect = true;
                    partner->step = Random::Int(5, 15);

                    new_rabbits.push_back(offspring);
                    break;
                }
            }

        }
    }
        
    

    void eat() {
        if (hunger <= 10 || dead) return;
        int base_cx = coord_to_chunkx(x);
        int base_cy = coord_to_chunky(y);
        Chunk& chunk = chunk_grid[base_cx][base_cy];

        for (auto& w : chunk_grid[base_cx][base_cy].grass) {
            if (auto partner = w.lock()) {
                if (partner->dead) continue;

                // расстояние с учётом тора
                float dx = torusDelta(x, partner->x, base_rangex);
                float dy = torusDelta(y, partner->y, base_rangey);
                float dist2 = dx * dx + dy * dy;

                if (dist2 < eating_range) {
                    hunger -= partner->nutritional_value;
                    partner->dead=true;
                }
            }
        }
    }
            
        

    

    bool shouldDie() const override {
        return dead || age > age_limit || hunger > hunger_limit;
    }

    void process(std::vector<std::shared_ptr<Rabbit>>& rabbits,
        std::vector<std::shared_ptr<Rabbit>>& new_rabbits,
        PopulationManager& pop) {
        if (shouldDie()) return;
        age++; hunger++;
        move_range = Random::Int(1, 5);
        move(new_rabbits,pop);
        eat();
        if (pop.canAddRabbit(static_cast<int>(new_rabbits.size())))
            reproduce(rabbits, new_rabbits);
    }

protected:
    std::vector<std::weak_ptr<Creature>>& getChunkContainer(Chunk& chunk) override {
        return chunk.rabbits;
    }
    std::vector<std::weak_ptr<Creature>>& getChunkContainer2(Chunk& chunk) override {
        return chunk.Animals;
    }

    void addToChunk(Chunk& chunk) override {
        chunk.rabbits.push_back(weak_from_this());
        chunk.Animals.push_back(weak_from_this());
    }
};
class Wolf : public Creature {
public:
    Wolf() : Creature(type_::wolf) {
        gender = (Random::Int(0, 1) == 0) ? gender_::male : gender_::female;
        eating_range = 10;
        age = 0;
        maturity_age = 2000;
        age_limit = 4000;
        hunger_limit = 1000;
        hunger = 0;
    }

    bool isDirectionSelect = false;
    int step = 0;
    float nextPositionX = 0;
    float nextPositionY = 0;
    float birth_time = 0.0f;

    int stepsTick = 0;
    float dirX = 0.0f, dirY = 0.0f;
    int remainingSteps = 0;
    float speed = 2.0f;
    int MATURITY_TICKS = 75;
    void move(std::vector<std::shared_ptr<Wolf>>& new_wolfs,
        PopulationManager& pop) {
        const float avoidance_radius = 5.0f;
        const float avoidanceStrength = 0.5f;

        bool isHunger = hunger > 100;


        bool isMaturity = (age >= maturity_age) &&
            (tick - birth_tick > MATURITY_TICKS) &&
            !isHunger;
        bool needNewDir = (!isDirectionSelect || step <= 0);
        //  избегание соседей 
        float ax = 0.0f, ay = 0.0f;
        int nearbyCount = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                int ncx = coord_to_chunkx(Wrap(x + i * CHUNK_SIZE, base_rangex));
                int ncy = coord_to_chunky(Wrap(y + j * CHUNK_SIZE, base_rangey));
                auto t = chunk_grid[ncx][ncy].nearly_creature_square(chunk_grid[ncx][ncy].wolves, x, y, avoidance_radius);
                nearbyCount += std::get<0>(t);
                ax += std::get<1>(t);
                ay += std::get<2>(t);
            }
        }

        if (nearbyCount > 0) {
            ax /= nearbyCount;
            ay /= nearbyCount;
            float len = std::sqrt(ax * ax + ay * ay);
            if (len > 1e-6f) {
                ax = (ax / len) * avoidanceStrength;
                ay = (ay / len) * avoidanceStrength;
            }
        }
        if (remainingSteps <= 0 || tick - stepsTick > 10) {
            if (isHunger) {
                auto bush = searchNearestCreature(x, y, type_::rabbit, 3, false, gender);
                if (bush.first != -5000.0f) {
                    float dx = torusDelta(x, bush.first, base_rangex);
                    float dy = torusDelta(y, bush.second, base_rangey);
                    float d = std::sqrt(dx * dx + dy * dy);
                    if (d > 1e-6f) {
                        dirX = dx / d;
                        dirY = dy / d;

                        // сколько шагов нужно до куста
                        remainingSteps = static_cast<int>(d / speed);

                        remainingSteps = max(1, remainingSteps);
                        stepsTick = tick;
                    }
                }
            }
            else if (isMaturity && pop.canAddWolf(static_cast<int>(new_wolfs.size()))) {
                auto target = searchNearestCreature(x, y, type_::wolf, 3, true, gender);
                if (target.first != -5000.0f) {
                    float dx = torusDelta(x, target.first, base_rangex);
                    float dy = torusDelta(y, target.second, base_rangey);
                    float d = std::sqrt(dx * dx + dy * dy);
                    if (d > 1e-6f) {
                        dirX = dx / d;
                        dirY = dy / d;

                        remainingSteps = static_cast<int>(d / speed);

                        remainingSteps = max(1, remainingSteps);
                        stepsTick = tick;
                    }
                }
            }
            // если всё ещё нет направления — случайный угол
            if (remainingSteps <= 0) {
                float a = Random::Float(0, 2 * 3.14159265f);
                dirX = cosf(a); dirY = sinf(a);
                remainingSteps = 10;
                stepsTick = tick;
            }
        }

        // вычисляем steering: основной dir + небольшое избегание
        float steerX = dirX + ax * 0.7f; // ax,ay — из расчёта избегания соседей
        float steerY = dirY + ay * 0.7f;
        float slen = std::sqrt(steerX * steerX + steerY * steerY);
        if (slen > 1e-6f) { steerX /= slen; steerY /= slen; }

        // пробуем сделать один маленький шаг
        float candX = Wrap(x + steerX * speed, base_rangex);
        float candY = Wrap(y + steerY * speed, base_rangey);

        if (heightW(candX, candY)) {
            // вода прямо впереди — пробуем обход в стороны (+/- 45°)
            float baseAngle = atan2f(dirY, dirX);
            bool found = false;
            const float offsets[4] = { 0.5f, -0.5f, 1.0f, -1.0f }; // в радианах ~30°, -30°, 60°, -60°
            for (float off : offsets) {
                float na = baseAngle + off;
                float nx = cosf(na), ny = sinf(na);
                float tx = Wrap(x + nx * speed, base_rangex);
                float ty = Wrap(y + ny * speed, base_rangey);
                if (!heightW(tx, ty)) { dirX = nx; dirY = ny; found = true; break; }
            }
            if (!found) {
                // застряли — сбрасываем направление, в следующем тике выберется новое
                remainingSteps = 0;
            }
        }
        else {
            // шаг возможен — совершаем его
            x = candX; y = candY;
            remainingSteps--;
            updateChunk();
        }
    }

    void reproduce(std::vector<std::shared_ptr<Wolf>>& wolves,
        std::vector<std::shared_ptr<Wolf>>& new_wolfs) {
        const float mateCooldown = 200.0f;
        if (age < maturity_age || (tick - birth_time) < mateCooldown || dead) return;

        int base_cx = coord_to_chunkx(x);
        int base_cy = coord_to_chunky(y);

        for (auto& w : chunk_grid[base_cx][base_cy].wolves) {
            if (auto partner = w.lock()) {
                if (partner.get() == this || partner->dead) continue;
                if (partner->gender == gender) continue;
                if (partner->age < maturity_age || (tick - partner->birth_tick) < mateCooldown) continue;

                // расстояние с учётом тора
                float dx = torusDelta(x, partner->x, base_rangex);
                float dy = torusDelta(y, partner->y, base_rangey);
                float dist2 = dx * dx + dy * dy;

                if (dist2 < 200.0f) {
                    auto offspring = std::make_shared<Wolf>();
                    offspring->x = Wrap(x + Random::Int(-5, 5), base_rangex);
                    offspring->y = Wrap(y + Random::Int(-5, 5), base_rangey);
                    offspring->birth_tick = tick;
                    offspring->gender = (rand() % 2 == 0) ? gender_::male : gender_::female;

                    // Обновляем cooldown родителей
                    birth_tick = tick;
                    partner->birth_tick = tick;

                    // разнесение, чтобы не слипались
                    const float nudge = 5.0f;
                    float nd = std::sqrt(dx * dx + dy * dy);
                    do {
                        nextPositionX = Random::Int(-move_range, move_range);
                        nextPositionY = Random::Int(-move_range, move_range);
                    } while (nextPositionX == 0 && nextPositionY == 0);
                    isDirectionSelect = true;
                    step = Random::Int(5, 15);

                    do {
                        partner->nextPositionX = Random::Int(-move_range, move_range);
                        partner->nextPositionY = Random::Int(-move_range, move_range);
                    } while (partner->nextPositionX == 0 && partner->nextPositionY == 0);
                    partner->isDirectionSelect = true;
                    partner->step = Random::Int(5, 15);

                    new_wolfs.push_back(offspring);
                    break;
                }
            }

        }
    }

    void eat(std::vector<std::shared_ptr<Rabbit>>& rabbits) {
        if (hunger <= 10 || dead) return;
        int base_cx = coord_to_chunkx(x);
        int base_cy = coord_to_chunky(y);
        for (auto& w : chunk_grid[base_cx][base_cy].rabbits) {
            if (auto partner = w.lock()) {
                if (partner->dead) continue;

                // расстояние с учётом тора
                float dx = torusDelta(x, partner->x, base_rangex);
                float dy = torusDelta(y, partner->y, base_rangey);
                float dist2 = dx * dx + dy * dy;

                if (dist2 < eating_range) {
                    hunger -= partner->nutritional_value;
                    partner->dead = true;
                    break;
                }
            }
        }
    }

    bool shouldDie() const override {
        return dead || age > age_limit || hunger > hunger_limit;
    }

    void process(std::vector<std::shared_ptr<Wolf>>& wolves,
        std::vector<std::shared_ptr<Wolf>>& new_wolves,
        std::vector<std::shared_ptr<Rabbit>>& rabbits,
        PopulationManager& pop) {
        if (shouldDie()) return;
        age++; hunger++;
        move_range = Random::Int(1, 15);
        move(new_wolves,pop);
        eat(rabbits);
        if (!pop.canAddWolf(static_cast<int>(new_wolves.size()))) return;
        reproduce(wolves, new_wolves);
    }

protected:
    std::vector<std::weak_ptr<Creature>>& getChunkContainer(Chunk& chunk) override {
        return chunk.wolves;
    }
    std::vector<std::weak_ptr<Creature>>& getChunkContainer2(Chunk& chunk) override {
        return chunk.Animals;
    }

    void addToChunk(Chunk& chunk) override {
        chunk.wolves.push_back(weak_from_this());
        chunk.Animals.push_back(weak_from_this());
    }
};

class Rat : public Creature {
public:
    Rat() : Creature(type_::rat) {
        nutritional_value = 1000;
        gender = (Random::Int(0, 1) == 0) ? gender_::male : gender_::female;
        eating_range = 2;
        age = 0;
        maturity_age = 250;
        age_limit = 500;
        hunger_limit = 200;
        hunger = 0;
        birth_tick = tick;

        isDirectionSelect = false;
        step = 0;
        nextPositionX = 0;
        nextPositionY = 0;
        move_range = 2;

    }
    
    bool isUsedInfection = false;

    int stepsTick = 0;
    float dirX = 0.0f, dirY = 0.0f;
    int remainingSteps = 0;
    float speed = 1.0f;
    int MATURITY_TICKS = 25;
    void move(std::vector<std::shared_ptr<Rat>>& new_rats,
        PopulationManager& pop) {
        const float avoidance_radius = 5.0f;
        const float avoidanceStrength = 0.5f;

        bool isHunger = hunger > 100;


        bool isMaturity = (age >= maturity_age) &&
            (tick - birth_tick > MATURITY_TICKS) &&
            !isHunger;
        bool needNewDir = (!isDirectionSelect || step <= 0);
        //  избегание соседей 
        float ax = 0.0f, ay = 0.0f;
        int nearbyCount = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                int ncx = coord_to_chunkx(Wrap(x + i * CHUNK_SIZE, base_rangex));
                int ncy = coord_to_chunky(Wrap(y + j * CHUNK_SIZE, base_rangey));
                auto t = chunk_grid[ncx][ncy].nearly_creature_square(chunk_grid[ncx][ncy].rabbits, x, y, avoidance_radius);
                nearbyCount += std::get<0>(t);
                ax += std::get<1>(t);
                ay += std::get<2>(t);
            }
        }

        if (nearbyCount > 0) {
            ax /= nearbyCount;
            ay /= nearbyCount;
            float len = std::sqrt(ax * ax + ay * ay);
            if (len > 1e-6f) {
                ax = (ax / len) * avoidanceStrength;
                ay = (ay / len) * avoidanceStrength;
            }
        }
        if (remainingSteps <= 0 || tick - stepsTick > 10) {
            if (isHunger) {
                auto bush = searchNearestCreature(x, y, type_::berry, 3, false, gender);
                if (bush.first != -5000.0f) {
                    float dx = torusDelta(x, bush.first, base_rangex);
                    float dy = torusDelta(y, bush.second, base_rangey);
                    float d = std::sqrt(dx * dx + dy * dy);
                    if (d > 1e-6f) {
                        dirX = dx / d;
                        dirY = dy / d;

                        // сколько шагов нужно до куста
                        remainingSteps = static_cast<int>(d / speed);

                        remainingSteps = max(1, remainingSteps);
                        stepsTick = tick;
                    }
                }
            }
            else if (isMaturity && pop.canAddRat(static_cast<int>(new_rats.size()))) {
                auto target = searchNearestCreature(x, y, type_::rat, 3, true, gender);
                if (target.first != -5000.0f) {
                    float dx = torusDelta(x, target.first, base_rangex);
                    float dy = torusDelta(y, target.second, base_rangey);
                    float d = std::sqrt(dx * dx + dy * dy);
                    if (d > 1e-6f) {
                        dirX = dx / d;
                        dirY = dy / d;

                        // сколько шагов нужно до куста
                        remainingSteps = static_cast<int>(d / speed);

                        remainingSteps = max(1, remainingSteps);
                        stepsTick = tick;
                    }
                }
            }
            // если всё ещё нет направления — случайный угол
            if (remainingSteps <= 0) {
                float a = Random::Float(0, 2 * 3.14159265f);
                dirX = cosf(a); dirY = sinf(a);
                remainingSteps = 5;
                stepsTick = tick;
            }
        }

        // вычисляем steering: основной dir + небольшое избегание
        float steerX = dirX + ax * 0.7f; // ax,ay — из расчёта избегания соседей
        float steerY = dirY + ay * 0.7f;
        float slen = std::sqrt(steerX * steerX + steerY * steerY);
        if (slen > 1e-6f) { steerX /= slen; steerY /= slen; }

        // пробуем сделать один маленький шаг
        float candX = Wrap(x + steerX * speed, base_rangex);
        float candY = Wrap(y + steerY * speed, base_rangey);

        if (heightW(candX, candY)) {
            // вода прямо впереди — пробуем обход в стороны (+/- 45°)
            float baseAngle = atan2f(dirY, dirX);
            bool found = false;
            const float offsets[4] = { 0.5f, -0.5f, 1.0f, -1.0f }; // в радианах ~30°, -30°, 60°, -60°
            for (float off : offsets) {
                float na = baseAngle + off;
                float nx = cosf(na), ny = sinf(na);
                float tx = Wrap(x + nx * speed, base_rangex);
                float ty = Wrap(y + ny * speed, base_rangey);
                if (!heightW(tx, ty)) { dirX = nx; dirY = ny; found = true; break; }
            }
            if (!found) {
                // застряли — сбрасываем направление, в следующем тике выберется новое
                remainingSteps = 0;
            }
        }
        else {
            // шаг возможен — совершаем его
            x = candX; y = candY;
            remainingSteps--;
            updateChunk();
        }
    }
    void infection() {
        if (Random::Int(1, 100000) == 1) {
            infect = true;
        }
    }
    void usedInfection() {
        age_limit *= 0.8;
        isUsedInfection = true;
    }

    void reproduce(std::vector<std::shared_ptr<Rat>>& rats,
        std::vector<std::shared_ptr<Rat>>& new_rats) {
        if (age < maturity_age || (tick - birth_tick) < MATURITY_TICKS || dead) return;

        int base_cx = coord_to_chunkx(x);
        int base_cy = coord_to_chunky(y);

        for (auto& w : chunk_grid[base_cx][base_cy].rats) {
            if (auto partner = w.lock()) {
                if (partner.get() == this || partner->dead) continue;
                if (partner->gender == gender) continue;
                if (partner->age < maturity_age || (tick - partner->birth_tick) < MATURITY_TICKS) continue;

                // расстояние с учётом тора
                float dx = torusDelta(x, partner->x, base_rangex);
                float dy = torusDelta(y, partner->y, base_rangey);
                float dist2 = dx * dx + dy * dy;

                if (dist2 < 50.0f * 50.0f) {
                    auto offspring = std::make_shared<Rat>();
                    offspring->x = Wrap(x + Random::Int(-5, 5), base_rangex);
                    offspring->y = Wrap(y + Random::Int(-5, 5), base_rangey);
                    offspring->birth_tick = tick;
                    offspring->gender = (rand() % 2 == 0) ? gender_::male : gender_::female;

                    // Обновляем cooldown родителей
                    birth_tick = tick;
                    partner->birth_tick = tick;

                    // разнесение, чтобы не слипались
                    const float nudge = 5.0f;
                    float nd = std::sqrt(dx * dx + dy * dy);
                    do {
                        nextPositionX = Random::Int(-move_range, move_range);
                        nextPositionY = Random::Int(-move_range, move_range);
                    } while (nextPositionX == 0 && nextPositionY == 0);
                    isDirectionSelect = true;
                    step = Random::Int(5, 15);

                    do {
                        partner->nextPositionX = Random::Int(-move_range, move_range);
                        partner->nextPositionY = Random::Int(-move_range, move_range);
                    } while (partner->nextPositionX == 0 && partner->nextPositionY == 0);
                    partner->isDirectionSelect = true;
                    partner->step = Random::Int(5, 15);

                    new_rats.push_back(offspring);
                    break;
                }
            }

        }
    }

    void eat(std::vector<std::shared_ptr<Bush>>& bushes) {
        if (hunger <= 10 || dead) return;
        int base_cx = coord_to_chunkx(x);
        int base_cy = coord_to_chunky(y);
        Chunk& chunk = chunk_grid[base_cx][base_cy];

        for (auto& w : chunk_grid[base_cx][base_cy].bushes) {
            if (auto partner = w.lock()) {
                if (partner->dead || partner->berry_count == 0) continue;

                // расстояние с учётом тора
                float dx = torusDelta(x, partner->x, base_rangex);
                float dy = torusDelta(y, partner->y, base_rangey);
                float dist2 = dx * dx + dy * dy;

                if (dist2 < eating_range) {
                    hunger -= partner->nutritional_value;
                    partner->berry_count--;
                }
            }
        }
    }

    bool shouldDie() const override {
        return dead  || hunger > hunger_limit|| age > age_limit;
    }

    void process(std::vector<std::shared_ptr<Rat>>& rats,
        std::vector<std::shared_ptr<Rat>>& new_rats,
        std::vector<std::shared_ptr<Bush>>& bushes,
        PopulationManager& pop) {
        if (shouldDie()) return;
        if (!infect) {
            infection();
        }
        if (infect && !isUsedInfection) {
            usedInfection();
        }
        age++; hunger++;
        move_range = Random::Int(1, 5);
        move(new_rats,pop);
        eat(bushes);
        if (!pop.canAddRat(static_cast<int>(new_rats.size()))) return;
        reproduce(rats, new_rats);
    }

protected:
    std::vector<std::weak_ptr<Creature>>& getChunkContainer(Chunk& chunk) override {
        return chunk.rats;
    }
    std::vector<std::weak_ptr<Creature>>& getChunkContainer2(Chunk& chunk) override {
        return chunk.Animals;
    }

    void addToChunk(Chunk& chunk) override {
        chunk.rats.push_back(weak_from_this());
        chunk.Animals.push_back(weak_from_this());
    }
};

class Eagle : public Creature {
public:
    Eagle() : Creature(type_::eagle) {
        gender = (Random::Int(0, 1) == 0) ? gender_::male : gender_::female;
        eating_range = 10;
        age = 0;
        maturity_age = 2000;
        age_limit = 4000;
        hunger_limit = 1000;
        hunger = 0;
    }

    bool isDirectionSelect = false;
    int step = 0;
    float nextPositionX = 0;
    float nextPositionY = 0;
    float birth_tick = 0.0f;

    void move(std::vector<std::shared_ptr<Eagle>>& new_eagles,
        PopulationManager& pop) {
        //разная скорость
        const float avoidance_radius = 5.0f;

        bool isHunger = hunger > 500;
        const float MATURITY_TICKS = 200;
        bool isMaturity = (age >= maturity_age) &&
            (tick - birth_tick > MATURITY_TICKS) &&
            !isHunger;

        // --- избегаем других волков
        float ax = 0, ay = 0;
        // int Wcount = 0;
        // for (int i = -1; i <= 1; ++i) {
        //     for (int j = -1; j <= 1; ++j) {
        //         int ncx = coord_to_chunkx(Wrap(x + i * CHUNK_SIZE, base_rangex));
        //         int ncy = coord_to_chunky(Wrap(y + j * CHUNK_SIZE, base_rangey));
        //         auto result = chunk_grid[ncx][ncy].nearly_creature_square(chunk_grid[ncx][ncy].wolves, x, y, avoidance_radius);
        //         int count = std::get<0>(result);
        //         float dx = std::get<1>(result);
        //         float dy = std::get<2>(result);
        //         Wcount += count;
        //         ax += dx;
        //         ay += dy;
        //     }
        // }
        // if (Wcount > 0) {
        //     ax /= Wcount;
        //     ay /= Wcount;
        //     float len = std::sqrt(ax * ax + ay * ay);
        //     if (len > 1e-6f) { ax /= len; ay /= len; }
        // }

        bool needNewDir = (!isDirectionSelect || step <= 0);

        // --- выбираем цель
        if (isHunger) {
            std::pair<float, float> targetRabbit = searchNearestCreature(x, y, type_::rat, 3, false, gender);
            float rabbitX = targetRabbit.first;
            float rabbitY = targetRabbit.second;
            if (rabbitX != -5000.0f) {
                float dx = torusDelta(x, rabbitX, base_rangex);
                float dy = torusDelta(y, rabbitY, base_rangey);
                float d = std::sqrt(dx * dx + dy * dy);
                if (d > 1e-6f) {
                    nextPositionX = (dx / d) * move_range;
                    nextPositionY = (dy / d) * move_range;
                    isDirectionSelect = true;
                    step = Random::Int(10, 30);
                    needNewDir = false;
                }
            }
        }
        else if (isMaturity && pop.canAddEagle(static_cast<int>(new_eagles.size()))) {
            std::pair<float, float> target = searchNearestCreature(x, y, type_::eagle, 3, true, gender);
            float targetX = target.first;
            float targetY = target.second;
            if (targetX != -5000.0f) {
                float dx = torusDelta(x, targetX, base_rangex);
                float dy = torusDelta(y, targetY, base_rangey);
                float d = std::sqrt(dx * dx + dy * dy);
                if (d > 1e-6f) {
                    nextPositionX = (dx / d) * move_range;
                    nextPositionY = (dy / d) * move_range;
                    isDirectionSelect = true;
                    step = Random::Int(10, 30);
                    needNewDir = false;
                }
            }
        }

        if (needNewDir) {
            do {
                nextPositionX = Random::Int(-move_range, move_range);
                nextPositionY = Random::Int(-move_range, move_range);
            } while (nextPositionX == 0 && nextPositionY == 0);
            isDirectionSelect = true;
            step = Random::Int(10, 30);
        }
        else {
            --step;
        }

        // --- комбинируем цель и отталкивание
        const float avoidanceWeight = 0.1f;
        float moveX = nextPositionX + ax * (move_range * avoidanceWeight);
        float moveY = nextPositionY + ay * (move_range * avoidanceWeight);

        float mlen = std::sqrt(moveX * moveX + moveY * moveY);
        if (mlen > move_range && mlen > 1e-6f) {
            moveX = (moveX / mlen) * move_range;
            moveY = (moveY / mlen) * move_range;
        }

        x = Wrap(x + moveX, base_rangex);
        y = Wrap(y + moveY, base_rangey);

        updateChunk();
    }

    void reproduce(std::vector<std::shared_ptr<Eagle>>& eagles,
        std::vector<std::shared_ptr<Eagle>>& new_eagles) {
        const float mateCooldown = 200.0f;
        if (age < maturity_age || (tick - birth_tick) < mateCooldown || dead) return;

        int base_cx = coord_to_chunkx(x);
        int base_cy = coord_to_chunky(y);

        for (auto& w : chunk_grid[base_cx][base_cy].eagles) {
            if (auto partner = w.lock()) {
                if (partner.get() == this || partner->dead) continue;
                if (partner->gender == gender) continue;
                if (partner->age < maturity_age || (tick - partner->birth_tick) < mateCooldown) continue;

                // расстояние с учётом тора
                float dx = torusDelta(x, partner->x, base_rangex);
                float dy = torusDelta(y, partner->y, base_rangey);
                float dist2 = dx * dx + dy * dy;

                if (dist2 < 200.0f) {
                    auto offspring = std::make_shared<Eagle>();
                    offspring->x = Wrap(x + Random::Int(-5, 5), base_rangex);
                    offspring->y = Wrap(y + Random::Int(-5, 5), base_rangey);
                    offspring->birth_tick = tick;
                    offspring->gender = (rand() % 2 == 0) ? gender_::male : gender_::female;

                    // Обновляем cooldown родителей
                    birth_tick = tick;
                    partner->birth_tick = tick;

                    // разнесение, чтобы не слипались
                    const float nudge = 5.0f;
                    float nd = std::sqrt(dx * dx + dy * dy);
                    do {
                        nextPositionX = Random::Int(-move_range, move_range);
                        nextPositionY = Random::Int(-move_range, move_range);
                    } while (nextPositionX == 0 && nextPositionY == 0);
                    isDirectionSelect = true;
                    step = Random::Int(5, 15);

                    do {
                        partner->nextPositionX = Random::Int(-move_range, move_range);
                        partner->nextPositionY = Random::Int(-move_range, move_range);
                    } while (partner->nextPositionX == 0 && partner->nextPositionY == 0);
                    partner->isDirectionSelect = true;
                    partner->step = Random::Int(5, 15);

                    new_eagles.push_back(offspring);
                    break;
                }
            }

        }
    }

    void eat(std::vector<std::shared_ptr<Rat>>& rats) {
        if (hunger <= 10 || dead) return;
        int base_cx = coord_to_chunkx(x);
        int base_cy = coord_to_chunky(y);
        for (auto& w : chunk_grid[base_cx][base_cy].rats) {
            if (auto partner = w.lock()) {
                if (partner->dead) continue;

                // расстояние с учётом тора
                float dx = torusDelta(x, partner->x, base_rangex);
                float dy = torusDelta(y, partner->y, base_rangey);
                float dist2 = dx * dx + dy * dy;

                if (dist2 < eating_range) {
                    hunger -= partner->nutritional_value;
                    partner->dead = true;
                    break;
                }
            }
        }
    }

    bool shouldDie() const override {
        return dead || age > age_limit || hunger > hunger_limit;
    }

    void process(std::vector<std::shared_ptr<Eagle>>& eagles,
        std::vector<std::shared_ptr<Eagle>>& new_eagles,
        std::vector<std::shared_ptr<Rat>>& rats,
        PopulationManager& pop) {
        if (shouldDie()) return;
        age++; hunger++;
        move_range = Random::Int(1, 15);
        move(new_eagles, pop);
        eat(rats);
        if (!pop.canAddEagle(static_cast<int>(new_eagles.size()))) return;
        reproduce(eagles, new_eagles);
    }

protected:
    std::vector<std::weak_ptr<Creature>>& getChunkContainer(Chunk& chunk) override {
        return chunk.eagles;
    }
    std::vector<std::weak_ptr<Creature>>& getChunkContainer2(Chunk& chunk) override {
        return chunk.Animals;
    }

    void addToChunk(Chunk& chunk) override {
        chunk.eagles.push_back(weak_from_this());
        chunk.Animals.push_back(weak_from_this());
    }
};


// Глобальный контейнер существ


std::vector<std::shared_ptr<Rabbit>> rabbits;
std::vector<std::shared_ptr<Tree>> trees;
std::vector<std::shared_ptr<Wolf>> wolves;
std::vector<std::shared_ptr<Bush>> bushes;
std::vector<std::shared_ptr<Eagle>> eagles;
std::vector<std::shared_ptr<Rat>> rats;
std::vector<std::shared_ptr<Grass>> grass;

