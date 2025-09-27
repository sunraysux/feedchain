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
    
    template <typename T>
    void reproduce(std::vector<std::shared_ptr<T>>& new_plants) {
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

                float normalizedX = (seedlingx + base_rangex) / (2.0f * base_rangex) / 4; // [0,1]
                float normalizedY = (seedlingy + base_rangey) / (2.0f * base_rangey) / 4; // [0,1]

                UINT texX = static_cast<UINT>(min(max(normalizedX, 0.0f) * (texW - 1), (double)(texW - 1)));
                UINT texY = static_cast<UINT>(min(max(normalizedY, 0.0f) * (texH - 1), (double)(texH - 1)));

                float height = heightMap.cpuData[texY * static_cast<UINT>(heightMap.size.x) + texX];
                if (height < waterLevel ) continue;

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


                auto seedling = std::dynamic_pointer_cast<T>(offspring);
                seedling->x = seedlingx;
                seedling->y = seedlingy;
                // установить базовые значени€
                seedling->age = 0;
                seedling->birth_tick = tick;
                seedling->updateChunk();
                new_plants.push_back(std::move(seedling));
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

                float normalizedX = (seedlingx + base_rangex) / (2.0f * base_rangex) / 4; // [0,1]
                float normalizedY = (seedlingy + base_rangey) / (2.0f * base_rangey) / 4; // [0,1]

                UINT texX = static_cast<UINT>(min(max(normalizedX, 0.0f) * (texW - 1), (double)(texW - 1)));
                UINT texY = static_cast<UINT>(min(max(normalizedY, 0.0f) * (texH - 1), (double)(texH - 1)));

                float height = heightMap.cpuData[texY * static_cast<UINT>(heightMap.size.x) + texX];
                if (height < waterLevel) continue;

                int xc = coord_to_chunkx(seedlingx);
                int yc = coord_to_chunky(seedlingy);

                float radius = 15.0f;
                float radius2 = radius * radius;

                int totalGrass = 0;
                int MAX_PLANTS_PER_CHUNK = 1;
                for (int i = -1; i < 1; i++) {
                    for (int j = -1; j < 1; j++) {
                        xc = coord_to_chunkx(seedlingx + i * CHUNK_SIZE);
                        yc = coord_to_chunky(seedlingy + j * CHUNK_SIZE);
                        int totalGrass1 = static_cast<int>(chunk_grid[xc][yc].Plants.size());

                        totalGrass += totalGrass1;
                        if (totalGrass1 > MAX_PLANTS_PER_CHUNK) break;
                    }
                    if (totalGrass > 1) break;
                }





                if (totalGrass > 1) continue;
                auto offspring = createOffspring();


                auto seedling = std::dynamic_pointer_cast<T>(offspring);
                seedling->x = seedlingx;
                seedling->y = seedlingy;
                // установить базовые значени€
                seedling->age = 0;
                seedling->birth_tick = tick;
                seedling->updateChunk();
                new_plants.push_back(std::move(seedling));
            }

        }
    }
    template <typename T>
    void process(
        std::vector<std::shared_ptr<T>>& new_plants,
        PopulationManager& pop) {
        if (shouldDie()) return;
        age++;

        if (age >= maturity_age && canAdd(pop, 0)) {
            if (tick % 5 == 0) {
                reproduce(new_plants);
            }
        }
    }
};
class Animal : public Creature {
public:
    Animal(type_ t) : Creature(t) {}

    virtual type_ getFoodType() const = 0;
    virtual std::shared_ptr<Animal> createOffspring() = 0;
    virtual bool canAdd(PopulationManager& pop, size_t newSize) = 0;
    virtual std::vector<std::weak_ptr<Creature>>& getMateContainer(Chunk& chunk) = 0;
    virtual void eat() = 0;
    virtual std::vector<std::weak_ptr<Creature>>& getFoodContainer(Chunk& chunk) = 0;
    int stepsTick = 0;
    float dirX = 0.0f, dirY = 0.0f;
    int remainingSteps = 0;
    float speed = 1.0f;
    int MATURITY_TICKS = 200;
    bool shouldDie() const override {
        return dead || age > age_limit || hunger > hunger_limit;
    }
    bool isUsedInfection = false;



protected:

    
    template <typename T>

    void process(
        std::vector<std::shared_ptr<T>>& creatures,
        std::vector<std::shared_ptr<T>>& new_creatures,
        PopulationManager& pop) {
        if (shouldDie()) return;
        age++; hunger++;
        if (type == type_::rat) {
            if (!infect) {
                infection();
            }
            if (infect && !isUsedInfection) {
                usedInfection();
            }
        }
        move_range = Random::Int(1, 5);
        move(pop);
        eat();
        if (canAdd(pop,0))
            reproduce(creatures, new_creatures);
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
protected:
    void move(PopulationManager& pop) {
        const float avoidance_radius = 10.0f;
        const float avoidanceStrength = 0.5f;

        bool isHunger = hunger > hunger_limit/2;

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
        if (remainingSteps <= 0 || tick - stepsTick > 10) {
            if (isHunger) {
                auto bush = searchNearestCreature(x, y, getFoodType(), 10, false, gender);
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
            else if (isMaturity && canAdd(pop,0)) {
                auto target = searchNearestCreature(x, y, type, 10, true, gender);
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
            // если всЄ ещЄ нет направлени€ Ч случайный угол
            if (remainingSteps <= 0) {
                float a = Random::Float(0, 2 * 3.14159265f);
                dirX = cosf(a); dirY = sinf(a);
                remainingSteps = 20;
                stepsTick = tick;
            }
        }

        // вычисл€ем steering: основной dir + небольшое избегание
        float steerX = dirX + ax * 0.7f; // ax,ay Ч из расчЄта избегани€ соседей
        float steerY = dirY + ay * 0.7f;
        float slen = std::sqrt(steerX * steerX + steerY * steerY);
        if (slen > 1e-6f) { steerX /= slen; steerY /= slen; }

        // пробуем сделать один маленький шаг
        float candX = Wrap(x + steerX * speed, base_rangex);
        float candY = Wrap(y + steerY * speed, base_rangey);

        if (heightW(candX, candY)) {
            // вода пр€мо впереди Ч пробуем обход в стороны (+/- 45∞)
            float baseAngle = atan2f(dirY, dirX);
            bool found = false;
            const float offsets[4] = { 0.5f, -0.5f, 1.0f, -1.0f }; // в радианах ~30∞, -30∞, 60∞, -60∞
            for (float off : offsets) {
                float na = baseAngle + off;
                float nx = cosf(na), ny = sinf(na);
                float tx = Wrap(x + nx * speed, base_rangex);
                float ty = Wrap(y + ny * speed, base_rangey);
                if (!heightW(tx, ty)) { dirX = nx; dirY = ny; found = true; break; }
            }
            if (!found) {
                // застр€ли Ч сбрасываем направление, в следующем тике выберетс€ новое
                remainingSteps = 0;
            }
        }
        else {
            // шаг возможен Ч совершаем его
            x = candX; y = candY;
            remainingSteps--;
            updateChunk();
        }
    }

    template <typename T>
    void reproduce(std::vector<std::shared_ptr<T>>& creatures
        ,std::vector<std::shared_ptr<T>>& new_creatures) {
        if (age < maturity_age || (tick - birth_tick) < MATURITY_TICKS || dead) return;

        int base_cx = coord_to_chunkx(x);
        int base_cy = coord_to_chunky(y);

        for (auto& w : getMateContainer(chunk_grid[base_cx][base_cy])) {
            if (auto partner = w.lock()) {
                if (partner.get() == this || partner->dead) continue;
                if (partner->gender == gender) continue;
                if (partner->age < maturity_age || (tick - partner->birth_tick) < MATURITY_TICKS) continue;

                // рассто€ние с учЄтом тора
                float dx = torusDelta(x, partner->x, base_rangex);
                float dy = torusDelta(y, partner->y, base_rangey);
                float dist2 = dx * dx + dy * dy;

                if (dist2 < 50.0f * 50.0f) {
                    auto spring = createOffspring();
                    auto offspring = std::dynamic_pointer_cast<T>(spring);
                    offspring->x = Wrap(x + Random::Int(-5, 5), base_rangex);
                    offspring->y = Wrap(y + Random::Int(-5, 5), base_rangey);
                    offspring->birth_tick = tick;
                    offspring->gender = (rand() % 2 == 0) ? gender_::male : gender_::female;

                    // ќбновл€ем cooldown родителей
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

                    new_creatures.push_back(offspring);
                    break;
                }
            }

        }
    }
};

class Grass : public Plnt {
public:
    Grass() : Plnt(type_::grass) {
        // »нициализаци€ параметров растени€
        nutritional_value = 200;
        maturity_age = 250;
        age_limit = 500;
        age = 0;
    }
    std::shared_ptr<Plnt> createOffspring() override { return std::make_shared<Grass>(); }
    bool canAdd(PopulationManager& pop, size_t newSize) override {
        return pop.canAddGrass(static_cast<int>(newSize));
    }

    void process(std::vector<std::shared_ptr<Grass>>& new_plants,
        PopulationManager& pop) {
        Plnt::process<Grass>(new_plants, pop);
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


class Tree : public Plnt {
public:
    Tree() : Plnt(type_::tree) {
        // »нициализаци€ параметров растени€
        nutritional_value = 100;
        age = 0;
        maturity_age = 1000;
        age_limit = 5000;
    }

    std::shared_ptr<Plnt> createOffspring() override { return std::make_shared<Tree>(); }
    bool canAdd(PopulationManager& pop, size_t newSize) override {
        return pop.canAddTree(static_cast<int>(newSize));
    }

    void process(std::vector<std::shared_ptr<Tree>>& new_plants,
        PopulationManager& pop) {
        Plnt::process<Tree>(new_plants, pop);
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

class Bush : public Plnt {
public:
    Bush() : Plnt(type_::bush) {
        // »нициализаци€ параметров растени€
        nutritional_value = 200;
        age = 0;
        maturity_age = 500;
        age_limit = 1000;
        /*berry_count = 0;*/
        blossoming_age = 80;
        /*berry_limit = 5;*/
    }
    
    std::shared_ptr<Plnt> createOffspring() override { return std::make_shared<Bush>(); }
    bool canAdd(PopulationManager& pop, size_t newSize) override {
        return pop.canAddBush(static_cast<int>(newSize));
    }
    /*void blossoming() {
        float reproductionChance = min(0.01f * (age - maturity_age), 0.05f);
        if ((Random::Int(0, 100)) >= (reproductionChance * 100))
            return;
        berry_count++;
    }*/
    
    void process(std::vector<std::shared_ptr<Bush>>& new_plants,
        PopulationManager& pop) {
        Plnt::process<Bush>(new_plants, pop);
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

class Rabbit : public Animal {
public:
    Rabbit() : Animal(type_::rabbit) {
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

    std::vector<std::weak_ptr<Creature>>& getFoodContainer(Chunk& c) override { return c.grass; }
    type_ getFoodType() const override { return type_::grass; }
    bool canAdd(PopulationManager& pop, size_t newSize) override {
        return pop.canAddRabbit(static_cast<int>(newSize));
    }
    std::shared_ptr<Animal> createOffspring() override {
        return std::make_shared<Rabbit>();
    }
    std::vector<std::weak_ptr<Creature>>& getMateContainer(Chunk& chunk) override {
        return chunk.rabbits;
    }
    

    void eat() {
        if (hunger <= 10 || dead) return;
        int base_cx = coord_to_chunkx(x);
        int base_cy = coord_to_chunky(y);
        Chunk& chunk = chunk_grid[base_cx][base_cy];

        for (auto& w : getFoodContainer(chunk_grid[base_cx][base_cy])) {
            if (auto partner = w.lock()) {
                if (partner->dead) continue;

                // рассто€ние с учЄтом тора
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
    void process(std::vector<std::shared_ptr<Rabbit>>& creature,
        std::vector<std::shared_ptr<Rabbit>>& new_creature,
        PopulationManager& pop) {
        Animal::process<Rabbit>(creature,new_creature, pop);
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
class Wolf : public Animal {
public:
    Wolf() : Animal(type_::wolf) {
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
    std::vector<std::weak_ptr<Creature>>& getFoodContainer(Chunk& c) override { return c.rabbits; }
    type_ getFoodType() const override { return type_::rabbit; }
    bool canAdd(PopulationManager& pop, size_t newSize) override {
        return pop.canAddWolf(static_cast<int>(newSize));
    }
    std::shared_ptr<Animal> createOffspring() override {
        return std::make_shared<Wolf>();
    }
    std::vector<std::weak_ptr<Creature>>& getMateContainer(Chunk& chunk) override {
        return chunk.wolves;
    }

    void eat() {
        if (hunger <= 10 || dead) return;
        int base_cx = coord_to_chunkx(x);
        int base_cy = coord_to_chunky(y);
        for (auto& w : getFoodContainer(chunk_grid[base_cx][base_cy])) {
            if (auto partner = w.lock()) {
                if (partner->dead) continue;

                // рассто€ние с учЄтом тора
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
    
    void process(std::vector<std::shared_ptr<Wolf>>& creature,
        std::vector<std::shared_ptr<Wolf>>& new_creature,
        PopulationManager& pop) {
        Animal::process<Wolf>(creature, new_creature, pop);
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

class Rat : public Animal {
public:
    Rat() : Animal(type_::rat) {
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
    
    bool isUsedInfection = false;

    int stepsTick = 0;
    float dirX = 0.0f, dirY = 0.0f;
    int remainingSteps = 0;
    float speed = 1.0f;
    int MATURITY_TICKS = 25;
    std::vector<std::weak_ptr<Creature>>& getFoodContainer(Chunk& c) override { return c.bushes; }
    type_ getFoodType() const override { return type_::bush; }
    bool canAdd(PopulationManager& pop, size_t newSize) override {
        return pop.canAddRat(static_cast<int>(newSize));
    }
    std::shared_ptr<Animal> createOffspring() override {
        return std::make_shared<Rat>();
    }
    std::vector<std::weak_ptr<Creature>>& getMateContainer(Chunk& chunk) override {
        return chunk.rats;
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


    void eat() {
        if (hunger <= 10 || dead) return;
        int base_cx = coord_to_chunkx(x);
        int base_cy = coord_to_chunky(y);
        Chunk& chunk = chunk_grid[base_cx][base_cy];

        for (auto& w : getFoodContainer(chunk)) {
            if (auto partner = w.lock()) {
                if (partner->dead) continue;

                // рассто€ние с учЄтом тора
                float dx = torusDelta(x, partner->x, base_rangex);
                float dy = torusDelta(y, partner->y, base_rangey);
                float dist2 = dx * dx + dy * dy;

                if (dist2 < eating_range) {
                    hunger -= partner->nutritional_value;
                    partner->dead = true;
                }
            }
        }
    }

    

    void process(std::vector<std::shared_ptr<Rat>>& creature,
        std::vector<std::shared_ptr<Rat>>& new_creature,
        PopulationManager& pop) {
        Animal::process<Rat>(creature, new_creature, pop);
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


class Eagle : public Animal {
public:
    Eagle() : Animal(type_::eagle) {
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
    std::vector<std::weak_ptr<Creature>>& getFoodContainer(Chunk& c) override { return c.rats; }
    type_ getFoodType() const override { return type_::rat; }
    bool canAdd(PopulationManager& pop, size_t newSize) override {
        return pop.canAddEagle(static_cast<int>(newSize));
    }
    std::shared_ptr<Animal> createOffspring() override {
        return std::make_shared<Eagle>();
    }
    std::vector<std::weak_ptr<Creature>>& getMateContainer(Chunk& chunk) override {
        return chunk.eagles;
    }

    
    

    void eat() {
        if (hunger <= 10 || dead) return;
        int base_cx = coord_to_chunkx(x);
        int base_cy = coord_to_chunky(y);
        for (auto& w : getFoodContainer(chunk_grid[base_cx][base_cy])) {
            if (auto partner = w.lock()) {
                if (partner->dead) continue;

                // рассто€ние с учЄтом тора
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

    void process(std::vector < std::shared_ptr <Eagle>> &creature,
        std::vector<std::shared_ptr<Eagle>>& new_creature,
        PopulationManager& pop) {
        Animal::process<Eagle>(creature, new_creature, pop);
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


// √лобальный контейнер существ


std::vector<std::shared_ptr<Rabbit>> rabbits;
std::vector<std::shared_ptr<Tree>> trees;
std::vector<std::shared_ptr<Wolf>> wolves;
std::vector<std::shared_ptr<Bush>> bushes;
std::vector<std::shared_ptr<Eagle>> eagles;
std::vector<std::shared_ptr<Rat>> rats;
std::vector<std::shared_ptr<Grass>> grass;


