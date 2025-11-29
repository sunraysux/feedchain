class Plnt : public Creature {
public:
    Plnt(type_ t) : Creature(t) {}


    virtual std::shared_ptr<Plnt> createOffspring() = 0;
    virtual bool canAdd(PopulationManager& pop, size_t newSize) = 0;
    void eat(std::vector<std::shared_ptr<Creature>>& creatures) {}
    bool shouldDie() const override {
        return dead || age > age_limit;
    }


protected:

    void reproduce() {
        if (!Random::FastChance(0.05f)) return;

        int seeds = Random::Int(1, 5);
        float seedlingx = x;
        float seedlingy = y;
        auto& heightMap = Textures::Texture[10];
        const UINT texW = static_cast<UINT>(heightMap.size.x);
        const UINT texH = static_cast<UINT>(heightMap.size.y);
        for (int s = 0; s < seeds; s++) {
            if (type == type_::tree) {
                float a = Random::Float(0.0f, 2.0f * 3.14159265f);
                float dist = Random::Float(5.0f, 40.0f);
                float dx = cosf(a) * dist;
                float dy = sinf(a) * dist;
                float seedlingx = x + dx;
                float seedlingy = y + dy;
                seedlingx = Wrap(seedlingx, base_rangex);
                seedlingy = Wrap(seedlingy, base_rangey);
                if (heightW(seedlingx, seedlingy)) continue;

                int xc = coord_to_chunkx(seedlingx);
                int yc = coord_to_chunky(seedlingy);

                float radius = 15.0f;
                float radius2 = radius * radius;

                int totalGrass = 0;
                int MAX_PLANTS_PER_CHUNK = 1;
                for (int i = -5; i < 5; i++) {
                    for (int j = -5; j < 5; j++) {
                        xc = coord_to_chunkx(seedlingx + i * CHUNK_SIZE);
                        yc = coord_to_chunky(seedlingy + j * CHUNK_SIZE);
                        int totalGrass1 = static_cast<int>(chunk_grid[xc][yc].trees.size());

                        totalGrass += totalGrass1;
                        if (totalGrass1 > MAX_PLANTS_PER_CHUNK) break;
                    }
                    if (totalGrass > 1) break;
                }

                if (totalGrass > 1) continue;
                auto offspring = createOffspring();


                auto seedling = offspring;
                seedling->x = seedlingx;
                seedling->y = seedlingy;
                // установить базовые значения
                seedling->age = 0;
                seedling->birth_tick = tick;
                seedling->updateChunk();
                seedling->id = plant_id + s + 1;
                new_creature.push_back(std::move(seedling));
            }

            else {
                float a = Random::Float(0.0f, 2.0f * 3.14159265f);
                float dist = Random::Float(5.0f, 20.0f);
                float dx = cosf(a) * dist;
                float dy = sinf(a) * dist;
                float seedlingx = x + dx;
                float seedlingy = y + dy;
                seedlingx = Wrap(seedlingx, base_rangex);
                seedlingy = Wrap(seedlingy, base_rangey);

                if (heightW(seedlingx, seedlingy)) continue;

                int xc = coord_to_chunkx(seedlingx);
                int yc = coord_to_chunky(seedlingy);

                float radius = 15.0f;
                float radius2 = radius * radius;

                int totalGrass = 0;
                int MAX_PLANTS_PER_CHUNK = 2;
                for (int i = -1; i < 2; i++) {
                    for (int j = -1; j < 2; j++) {
                        xc = coord_to_chunkx(seedlingx + i * CHUNK_SIZE);
                        yc = coord_to_chunky(seedlingy + j * CHUNK_SIZE);
                        int totalGrass1 = static_cast<int>(chunk_grid[xc][yc].Plants.size());

                        totalGrass += totalGrass1;
                        if (totalGrass1 > MAX_PLANTS_PER_CHUNK) break;
                    }
                    if (totalGrass > 2) break;
                }





                if (totalGrass > 2) continue;
                auto offspring = createOffspring();


                auto seedling =offspring;
                seedling->x = seedlingx;
                seedling->y = seedlingy;
                // установить базовые значения
                seedling->age = 0;
                seedling->birth_tick = tick;
                seedling->id = plant_id + s + 1;
                seedling->updateChunk();
                new_creature.push_back(std::move(seedling));
            }

        }
        plant_id += seeds;
    }

    template <typename T>
    void process(PopulationManager& pop) {
        if (shouldDie()) return;
        if (tick+id % 10 == 0)
            updateChunk();
        age++;
        if (type == type_::berry && !isRotten && age > age_limit * 0.5) {
            isRotten = true;
        }
        if (type != type_::berry) {
            if (age >= maturity_age && canAdd(pop, 0)) {
                reproduce();
                
            }
        }
    }
};

class Animal : public Creature {
public:
    Animal(type_ t) : Creature(t) {}

    virtual std::vector<type_> getFoodTypes() const = 0;
    virtual std::shared_ptr<Animal> createOffspring() = 0;
    virtual bool canAdd(PopulationManager& pop, size_t newSize) = 0;
    virtual std::vector<std::weak_ptr<Creature>>& getMateContainer(Chunk& chunk) = 0;
    virtual void getFoodContainers(Chunk& c, std::vector<std::weak_ptr<Creature>>& output) = 0;
    int stepsTick = 0;
    float dirX = 0.0f, dirY = 0.0f;
    int remainingSteps = 0;
    float speed = 1.0f;
    int MATURITY_TICKS = 100;
    bool shouldDie() const override {
        return dead || age > age_limit || hunger > hunger_limit;
    }



protected:



    void process(PopulationManager& pop) {
        if (shouldDie()) return;
        age++;
        hunger++;
        if (!infect) {
            infection();
        }
        if (infect && !isUsedInfection) {
            usedInfection();
        }
        move_range = Random::Int(1, 5);
        move(pop);
        eat();
        if (canAdd(pop, 0))
            reproduce();
    }

protected:

    bool findRichChunk(bool rich) {
        int currentChunkX = coord_to_large_chunkx(x);
        int currentChunkY = coord_to_large_chunky(y);

        ChunkWorld& currentChunk = population.getChunkByIndex(currentChunkX, currentChunkY);
        // Для травоядных ищем траву, для хищников - добычу
        int currentResource = 0;
        int targetResource = 0;

        switch (type) {
        case type_::rabbit:
            currentResource = currentChunk.grass_sum + currentChunk.berry_sum - currentChunk.wolf_sum * 1000;
            if (maturity_age < age)
                currentResource += currentChunk.rabbit_sum * 10 ;
            break;
        case type_::rat:
            currentResource = currentChunk.grass_sum + currentChunk.berry_sum - currentChunk.wolf_sum * 100;
            if (currentChunk.rat_sum > 10)
                currentResource = -10000;
            if (maturity_age < age)
                currentResource += currentChunk.rat_sum - currentChunk.wolf_sum;
            break;
        case type_::wolf:
            currentResource = currentChunk.rabbit_sum + currentChunk.rat_sum;
            break;
        case type_::bear:
            currentResource = currentChunk.rabbit_sum + currentChunk.bush_sum + currentChunk.berry_sum + currentChunk.wolf_sum + currentChunk.rat_sum;
            break;
        case type_::eagle:
            currentResource = currentChunk.rat_sum;
            break;
        default:
            return false;
        }
        if (rich) {
            if (currentResource >= 50) {
                
                //float a = Random::Float(0, 2 * 3.14159265f);
                //dirX = cosf(a);
                //dirY = sinf(a);
                return true;
            }
            else
                return false;
        }
        // Если в текущем чанке достаточно ресурсов
        if (currentResource >= 50) {
            float a = Random::Float(0, 2 * 3.14159265f);
            dirX = cosf(a);
            dirY = sinf(a);
            return true;
        }

        // Поиск лучшего чанка
        int bestChunkX = -1, bestChunkY = -1;
        int maxResource = currentResource;

        for (int dx = -3; dx <= 3; dx++) {
            for (int dy = -3; dy <= 3; dy++) {
                if (dx == 0 && dy == 0) continue;

                int checkX = currentChunkX + dx;
                int checkY = currentChunkY + dy;

                if (checkX >= 0 && checkX < CHUNKS_PER_SIDE_LARGE &&
                    checkY >= 0 && checkY < CHUNKS_PER_SIDE_LARGE) {

                    ChunkWorld& neighbor = population.getChunkByIndex(checkX, checkY);
                    int neighborResource = 0;
                    switch (type) {
                    case type_::rabbit:
                        neighborResource = neighbor.grass_sum + neighbor.berry_sum - neighbor.wolf_sum * 1000;
                        if (maturity_age < age)
                            neighborResource += neighbor.rabbit_sum*100 ;
                        break;
                    case type_::rat:
                        neighborResource = neighbor.grass_sum + neighbor.berry_sum - neighbor.wolf_sum * 1000;
                        if (neighbor.rat_sum > 10)
                            neighborResource = -10000;
                        if (maturity_age < age)
                            neighborResource += neighbor.rat_sum ;
                        break;
                    case type_::wolf:
                        neighborResource = neighbor.rabbit_sum + neighbor.rat_sum;
                        if (maturity_age < age)
                            neighborResource = neighbor.wolf_sum - neighbor.rabbit_sum * 100 - neighbor.rat_sum * 100;
                        break;
                    case type_::bear:
                        neighborResource = neighbor.berry_sum + neighbor.rabbit_sum;
                        break;
                    case type_::eagle:
                        neighborResource = neighbor.rabbit_sum + neighbor.rat_sum;
                        break;
                    }

                    if (neighborResource > maxResource) {
                        maxResource = neighborResource;
                        bestChunkX = checkX;
                        bestChunkY = checkY;
                    }
                }
            }
        }

        if (bestChunkX != -1 && bestChunkY != -1) {
            float targetX = (bestChunkX + 0.5f) * LARGE_CHUNK_SIZE;
            float targetY = (bestChunkY + 0.5f) * LARGE_CHUNK_SIZE;
            float dx = torusDeltaSigned(x, targetX, base_rangex);
            float dy = torusDeltaSigned(y, targetY, base_rangey);
            float d = std::sqrt(dx * dx + dy * dy);
            if (d > 1e-6f) {
                dirX = dx / d;
                dirY = dy / d;
                return true;
            }
        }

        return false;
    }

    void move(PopulationManager& pop) {
        const float avoidance_radius = 10.0f;
        const float avoidanceStrength = 0.5f;

        bool isHunger = hunger > hunger_limit / 2;

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
                auto t = chunk_grid[ncx][ncy].nearlyUnity_creature_square(type, x, y, avoidance_radius);
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
        if (remainingSteps <= 0 || tick - stepsTick > 60) {
            if (isHunger || eating) {
                eating = true;
                auto bush = searchNearestCreature(x, y, type, false, gender, findRichChunk(true));
                if (bush.first != -5000.0f) {
                    float dx = torusDeltaSigned(x, bush.first, base_rangex);
                    float dy = torusDeltaSigned(y, bush.second, base_rangey);
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
            else if (isMaturity && canAdd(pop, 0)) {
                auto target = searchNearestCreature(x, y, type, true, gender, findRichChunk(true));
                if (target.first != -5000.0f) {
                    float dx = torusDeltaSigned(x, target.first, base_rangex);
                    float dy = torusDeltaSigned(y, target.second, base_rangey);
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
                if (findRichChunk(false)) {
                    // Нашли направление к чанку с травой
                    remainingSteps = Random::Int(30, 60); // Долгий переход
                    stepsTick = tick;
                }
                else {
                    // Не нашли чанк с травой — случайное направление
                    float a = Random::Float(0, 2 * 3.14159265f);
                    dirX = cosf(a); dirY = sinf(a);
                    remainingSteps = 20;
                    stepsTick = tick;
                }
            }
        }

        // вычисляем steering: основной dir + небольшое избегание

        float steerX = dirX + ax * 3; // ax,ay — из расчёта избегания соседей
        float steerY = dirY + ay * 3;
        if (isMaturity && canAdd(pop, 0))
        {
            steerX = dirX;
            steerY = dirY;
        }
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

    void reproduce() {
        if (age < maturity_age || (tick - birth_tick) < MATURITY_TICKS || dead) return;

        int base_cx = coord_to_chunkx(x);
        int base_cy = coord_to_chunky(y);

        for (auto& w : getMateContainer(chunk_grid[base_cx][base_cy])) {
            if (auto partner = w.lock()) {
                if (partner.get() == this || partner->dead) continue;
                if (partner->gender == gender) continue;
                if (partner->age < maturity_age || (tick - partner->birth_tick) < MATURITY_TICKS) continue;

                // расстояние с учётом тора
                float dx = torusDeltaSigned(x, partner->x, base_rangex);
                float dy = torusDeltaSigned(y, partner->y, base_rangey);
                float dist2 = dx * dx + dy * dy;

                if (dist2 < 50.0f * 50.0f) {
                    auto spring = createOffspring();
                    auto offspring = spring;
                    offspring->x = Wrap(x + Random::Int(-5, 5), base_rangex);
                    offspring->y = Wrap(y + Random::Int(-5, 5), base_rangey);
                    offspring->birth_tick = tick;
                    offspring->infect = false;
                    offspring->isUsedInfection = false;
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
                    new_creature.push_back(std::move(offspring));
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
        std::vector<std::weak_ptr<Creature>> foodPool;
        getFoodContainers(chunk, foodPool);
        for (auto& w : foodPool) {
            if (auto partner = w.lock()) {
                if (partner->dead) continue;

                // расстояние с учётом тора
                float dx = torusDeltaSigned(x, partner->x, base_rangex);
                float dy = torusDeltaSigned(y, partner->y, base_rangey);
                float dist2 = dx * dx + dy * dy;

                if (dist2 < eating_range) {
                    if (partner->nutritional_value > nutritional_value)
                    {
                        dead = true;
                    }
                    else {
                        hunger -= partner->nutritional_value;
                        if (hunger < hunger_limit / 10)
                            eating = false;
                        if (partner->isRotten == true) {
                            dead = true;
                        }

                        partner->dead = true;
                    }
                }
            }
        }
    }
    void infection() {
        int xc = 0;
        int yc = 0;
        virus = 0;
        for (int i = -1; i < 1; i++) {
            for (int j = -1; j < 1; j++) {
                xc = coord_to_chunkx(x + i * CHUNK_SIZE);
                yc = coord_to_chunky(y + j * CHUNK_SIZE);
                for (auto& w : getMateContainer(chunk_grid[xc][yc])) {
                    if (auto partner = w.lock()) {
                        virus += 1;

                    }
                }
            }
        }

        if (virus > 5) {

            infect = true;
        }
    }
    void usedInfection() {
        age_limit *= 0.1;
        isUsedInfection = true;
    }
};