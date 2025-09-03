
class Tree : public Creature {
public:
    Tree() : Creature(type_::tree) {
        // Инициализация параметров растения
        nutritional_value = 100;
        age = 0;
        maturity_age = 100;
        age_limit = 170;
    }

    void reproduce(std::vector<std::shared_ptr<Tree>>& all_trees,
        std::vector<std::shared_ptr<Tree>>& new_creatures) {

        float reproductionChance = min(0.01f * (age - maturity_age), 0.05f);
        if ((Random::Float(0, 100)) >= (reproductionChance * 100))
            return;

        int seeds = Random::Int(1, 5);

        for (int s = 0; s < seeds; s++) {
            auto seedling = std::make_shared<Tree>(*this);
            seedling->age = 0;
            seedling->dead = false;
            seedling->maturity_age+= Random::Int(-10, 10);
            seedling->age_limit += Random::Int(-10, 10);
            float distance = Random::Int(10, 100); // 3–50
            float angle = Random::Float(0, 3.14 * 2);
            seedling->x += distance * cos(angle);
            seedling->y += distance * sin(angle);

            // Обрезка по границам
            seedling->x = Wrap(seedling->x, base_rangex);
            seedling->y = Wrap(seedling->y, base_rangey);

            // Проверка минимального расстояния (например, 2.0f)
            bool tooClose = false;

            // Проверка плотности (например, не более 10 растений в радиусе 5)
            int nearbyCount = 0;
            int xc=coord_to_chunkx(seedling->x);
            int yc=coord_to_chunky(seedling->y);
            if (chunk_grid[xc][yc].countCreatures(chunk_grid[xc][yc].trees) > 50) continue;
            updateChunk();
            new_creatures.push_back(seedling);
        }
    }


    void move() override {} // Растения не двигаются

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

    void addToChunk(Chunk& chunk) override {
        chunk.trees.push_back(weak_from_this());
    }
};

class Bush : public Creature {
public:
    Bush() : Creature(type_::bush) {
        // Инициализация параметров растения
        nutritional_value = 100;
        age = 0;
        maturity_age = 100;
        age_limit = 170;
        berry_count = 0;
        blossoming_age = 80;
        berry_limit = 5;
    }
    
  

    void blossoming() {
        float reproductionChance = min(0.01f * (age - maturity_age), 0.05f);
        if ((Random::Float(0, 100)) >= (reproductionChance * 100))
            return;
        berry_count++;
    }
    void reproduce(std::vector<std::shared_ptr<Bush>>& all_bushes,
        std::vector<std::shared_ptr<Bush>>& new_creatures) {
        float reproductionChance = min(0.01f * (age - maturity_age), 0.05f);
        if ((Random::Float(0, 100)) >= (reproductionChance * 100))
            return;

        int seeds = Random::Int(1, 5);

        for (int s = 0; s < seeds; s++) {
            auto seedling = std::make_shared<Bush>(*this);
            seedling->age = 0;
            seedling->dead = false;
            seedling->maturity_age += Random::Int(-10, 10);
            seedling->age_limit += Random::Int(-10, 10);
            float distance = Random::Int(10, 100); // 3–50
            float angle = Random::Float(0, 3.14 * 2);
            seedling->x += distance * cos(angle);
            seedling->y += distance * sin(angle);

            // Обрезка по границам
            seedling->x = Wrap(seedling->x, base_rangex);
            seedling->y = Wrap(seedling->y, base_rangey);

            // Проверка минимального расстояния (например, 2.0f)
            bool tooClose = false;

            // Проверка плотности (например, не более 10 растений в радиусе 5)
            int nearbyCount = 0;
            int xc = coord_to_chunkx(seedling->x);
            int yc = coord_to_chunky(seedling->y);
            if (chunk_grid[xc][yc].countCreatures(chunk_grid[xc][yc].bushes) > 50) continue;
            updateChunk();
            new_creatures.push_back(seedling);
        }
    }


    void move() override {} // Растения не двигаются

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

    void addToChunk(Chunk& chunk) override {
        chunk.bushes.push_back(weak_from_this());
    }
};

class Rabbit : public Creature {
public:
    Rabbit() : Creature(type_::rabbit) {
        nutritional_value = 100;
        gender = (rand() % 2 == 0) ? gender_::male : gender_::female;
        eating_range = 2;
        age = 0;
        maturity_age = 100;
        age_limit = 2000;
        hunger_limit = 50;
        hunger = 0;
        birth_time = currentTime;

        isDirectionSelect = false;
        step = 0;
        nextPositionX = 0;
        nextPositionY = 0;
        move_range = 2;
    }

    void move() override {
        const float avoidance_radius = 5.0f;
        const float avoidanceStrength = 0.5f;

        bool isHunger = hunger > 100;
        bool isMaturity = (age >= maturity_age) && (currentTime - birth_time > 200) && !isHunger;

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
            if (len > 1e-6f) { ax = (ax / len) * avoidanceStrength;
            ay = (ay / len) * avoidanceStrength; }
        }

        //  Выбор цели для размножения 
        if (isMaturity && (!isDirectionSelect || step <= 0)) {
            auto target = searchNearestCreature(x, y, type_::rabbit, 3, true);
            if (target.first != -5000.0f) {
                float dx = torusDelta(x, target.first, base_rangex);
                float dy = torusDelta(y, target.second, base_rangey);
                float dist2 = dx * dx + dy * dy;
                if (dist2 > 1.0f) {
                    float d = std::sqrt(dist2);
                    nextPositionX = (dx / d) * move_range;
                    nextPositionY = (dy / d) * move_range;
                    isDirectionSelect = true;
                    step = Random::Int(5, 15);
                }
            }
        }

        //  Случайное движение если нет цели 
        if (!isDirectionSelect || step <= 0) {
            do {
                nextPositionX = Random::Int(-move_range, move_range);
                nextPositionY = Random::Int(-move_range, move_range);
            } while (nextPositionX == 0 && nextPositionY == 0);
            isDirectionSelect = true;
            step = Random::Int(5, 15);
        }
        else {
            --step;
        }

        //  Комбинируем цель и слабое избегание 
        float moveX = nextPositionX + ax;
        float moveY = nextPositionY + ay;
        float mlen = std::sqrt(moveX * moveX + moveY * moveY);
        if (mlen > move_range && mlen > 1e-6f) {
            moveX = (moveX / mlen) * move_range;
            moveY = (moveY / mlen) * move_range;
        }

        x = Wrap(x + moveX, base_rangex);
        y = Wrap(y + moveY, base_rangey);

        updateChunk();
    }

    void reproduce(std::vector<std::shared_ptr<Rabbit>>& rabbits,
        std::vector<std::shared_ptr<Rabbit>>& new_rabbits) {
        const float mateCooldown = 200.0f;
        if (age < maturity_age || (currentTime - birth_time) < mateCooldown || dead) return;

        int base_cx = coord_to_chunkx(x);
        int base_cy = coord_to_chunky(y);

        for (auto& w : chunk_grid[base_cx][base_cy].rabbits) {
            if (auto partner = w.lock()) {
                if (partner.get() == this || partner->dead) continue;
                if (partner->gender == gender) continue;
                if (partner->age < maturity_age || (currentTime - partner->birth_time) < mateCooldown) continue;

                // расстояние с учётом тора
                float dx = torusDelta(x, partner->x, base_rangex);
                float dy = torusDelta(y, partner->y, base_rangey);
                float dist2 = dx * dx + dy * dy;

                if (dist2 < 50.0f) {
                    auto offspring = std::make_shared<Rabbit>();
                    offspring->x = Wrap(x + Random::Int(-5, 5), base_rangex);
                    offspring->y = Wrap(y + Random::Int(-5, 5), base_rangey);
                    offspring->birth_time = currentTime;
                    offspring->gender = (rand() % 2 == 0) ? gender_::male : gender_::female;

                    // Обновляем cooldown родителей
                    birth_time = currentTime;
                    partner->birth_time = currentTime;

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
        int cx = coord_to_chunkx(x);
        int cy = coord_to_chunky(y);
        if (cx < 0 || cy < 0 || cx >= CHUNKS_PER_SIDEX || cy >= CHUNKS_PER_SIDEY) return;

        Chunk& chunk = chunk_grid[cx][cy];
        if (chunk.grass.growth >= 10) {
            float hungerReduction = min(hunger, chunk.grass.growth) * 1.0f;

            hunger -= hungerReduction;
            chunk.grass.growth -= hungerReduction;
            if (hunger < 0) hunger = 0;

            if (chunk.grass.growth < 0) chunk.grass.growth = 0;
        }
    }

    bool shouldDie() const override {
        return dead || age > age_limit || hunger > hunger_limit;
    }

    void process(std::vector<std::shared_ptr<Rabbit>>& rabbits,
        std::vector<std::shared_ptr<Rabbit>>& new_rabbits,
        std::vector<std::shared_ptr<Tree>>& trees,
        PopulationManager& pop) {
        if (shouldDie()) return;
        age++; hunger++;
        move_range = Random::Int(1, 5);
        move();
        eat();
        if (pop.canAddRabbit(static_cast<int>(new_rabbits.size())))
            reproduce(rabbits, new_rabbits);
    }

protected:
    std::vector<std::weak_ptr<Creature>>& getChunkContainer(Chunk& chunk) override {
        return chunk.rabbits;
    }
    void addToChunk(Chunk& chunk) override {
        chunk.rabbits.push_back(weak_from_this());
    }
};
class Wolf : public Creature {
public:
    Wolf() : Creature(type_::wolf) {
        gender = (rand() % 2 == 0) ? gender_::male : gender_::female;
        eating_range = 2;
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

    void move() override {
        //разная скорость
        const float avoidance_radius = 5.0f;

        bool isHunger = hunger > 500;
        bool isMaturity = age >= maturity_age && birth_time == 0.0f;

        // --- избегаем других волков
        float ax = 0, ay = 0;
        int Wcount = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                int ncx = coord_to_chunkx(Wrap(x + i * CHUNK_SIZE, base_rangex));
                int ncy = coord_to_chunky(Wrap(y + j * CHUNK_SIZE, base_rangey));
                auto result = chunk_grid[ncx][ncy].nearly_creature_square(chunk_grid[ncx][ncy].wolves, x, y, avoidance_radius);
                int count = std::get<0>(result);
                float dx = std::get<1>(result);
                float dy = std::get<2>(result);
                Wcount += count;
                ax += dx;
                ay += dy;
            }
        }
        if (Wcount > 0) {
            ax /= Wcount;
            ay /= Wcount;
            float len = std::sqrt(ax * ax + ay * ay);
            if (len > 1e-6f) { ax /= len; ay /= len; }
        }

        bool needNewDir = (!isDirectionSelect || step <= 0);

        // --- выбираем цель
        if (isHunger) {
            std::pair<float, float> targetRabbit = searchNearestCreature(x, y, type_::rabbit, 3, false);
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
        else if (isMaturity) {
            std::pair<float, float> target = searchNearestCreature(x, y, type_::wolf, 3, true);
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

    void reproduce(std::vector<std::shared_ptr<Wolf>>& wolves,
        std::vector<std::shared_ptr<Wolf>>& new_wolfs) {
        const float mateCooldown = 200.0f;
        if (age < maturity_age || (currentTime - birth_time) < mateCooldown || dead) return;

        int base_cx = coord_to_chunkx(x);
        int base_cy = coord_to_chunky(y);

        for (auto& w : chunk_grid[base_cx][base_cy].wolves) {
            if (auto partner = w.lock()) {
                if (partner.get() == this || partner->dead) continue;
                if (partner->gender == gender) continue;
                if (partner->age < maturity_age || (currentTime - partner->birth_time) < mateCooldown) continue;

                // расстояние с учётом тора
                float dx = torusDelta(x, partner->x, base_rangex);
                float dy = torusDelta(y, partner->y, base_rangey);
                float dist2 = dx * dx + dy * dy;

                if (dist2 < 200.0f) {
                    auto offspring = std::make_shared<Wolf>();
                    offspring->x = Wrap(x + Random::Int(-5, 5), base_rangex);
                    offspring->y = Wrap(y + Random::Int(-5, 5), base_rangey);
                    offspring->birth_time = currentTime;
                    offspring->gender = (rand() % 2 == 0) ? gender_::male : gender_::female;

                    // Обновляем cooldown родителей
                    birth_time = currentTime;
                    partner->birth_time = currentTime;

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
        if (birth_time != 0.0f && currentTime - birth_time > 2000.0f)
            birth_time = 0.0f;
        move_range = Random::Int(1, 15);
        move();
        eat(rabbits);
        if (!pop.canAddWolf(static_cast<int>(new_wolves.size()))) return;
        reproduce(wolves, new_wolves);
    }

protected:
    std::vector<std::weak_ptr<Creature>>& getChunkContainer(Chunk& chunk) override {
        return chunk.wolves;
    }

    void addToChunk(Chunk& chunk) override {
        chunk.wolves.push_back(weak_from_this());
    }
};

class Rat : public Creature {
public:
    Rat() : Creature(type_::rat) {
        gender = (rand() % 2 == 0) ? gender_::male : gender_::female;
        eating_range = 2;
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

    void move() override {
        //разная скорость
        const float avoidance_radius = 5.0f;

        bool isHunger = hunger > 500;
        bool isMaturity = age >= maturity_age && birth_time == 0.0f;

        // --- избегаем других волков
        float ax = 0, ay = 0;
        int Rcount = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                int ncx = coord_to_chunkx(Wrap(x + i * CHUNK_SIZE, base_rangex));
                int ncy = coord_to_chunky(Wrap(y + j * CHUNK_SIZE, base_rangey));
                auto result = chunk_grid[ncx][ncy].nearly_creature_square(chunk_grid[ncx][ncy].rats, x, y, avoidance_radius);
                int count = std::get<0>(result);
                float dx = std::get<1>(result);
                float dy = std::get<2>(result);
                Rcount += count;
                ax += dx;
                ay += dy;
            }
        }
        if (Rcount > 0) {
            ax /= Rcount;
            ay /= Rcount;
            float len = std::sqrt(ax * ax + ay * ay);
            if (len > 1e-6f) { ax /= len; ay /= len; }
        }

        bool needNewDir = (!isDirectionSelect || step <= 0);

        // --- выбираем цель
        if (isHunger) {
            std::pair<float, float> targetBush = searchNearestCreature(x, y, type_::bush, 3, false);
            float BushX = targetBush.first;
            float BushY = targetBush.second;
            if (BushX != -5000.0f) {
                float dx = torusDelta(x, BushX, base_rangex);
                float dy = torusDelta(y, BushY, base_rangey);
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
        else if (isMaturity) {
            std::pair<float, float> target = searchNearestCreature(x, y, type_::rat, 3, true);
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

    void reproduce(std::vector<std::shared_ptr<Rat>>& rats,
        std::vector<std::shared_ptr<Rat>>& new_rats) {
        const float mateCooldown = 200.0f;
        if (age < maturity_age || (currentTime - birth_time) < mateCooldown || dead) return;

        int base_cx = coord_to_chunkx(x);
        int base_cy = coord_to_chunky(y);

        for (auto& w : chunk_grid[base_cx][base_cy].rats) {
            if (auto partner = w.lock()) {
                if (partner.get() == this || partner->dead) continue;
                if (partner->gender == gender) continue;
                if (partner->age < maturity_age || (currentTime - partner->birth_time) < mateCooldown) continue;

                // расстояние с учётом тора
                float dx = torusDelta(x, partner->x, base_rangex);
                float dy = torusDelta(y, partner->y, base_rangey);
                float dist2 = dx * dx + dy * dy;

                if (dist2 < 100.0f) {
                    auto offspring = std::make_shared<Rat>();
                    offspring->x = Wrap(x + Random::Int(-5, 5), base_rangex);
                    offspring->y = Wrap(y + Random::Int(-5, 5), base_rangey);
                    offspring->birth_time = currentTime;
                    offspring->gender = (rand() % 2 == 0) ? gender_::male : gender_::female;

                    // Обновляем cooldown родителей
                    birth_time = currentTime;
                    partner->birth_time = currentTime;

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
        return dead || age > age_limit || hunger > hunger_limit;
    }

    void process(std::vector<std::shared_ptr<Rat>>& rats,
        std::vector<std::shared_ptr<Rat>>& new_rats,
        std::vector<std::shared_ptr<Bush>>& bushes,
        PopulationManager& pop) {
        if (shouldDie()) return;
        age++; hunger++;
        if (birth_time != 0.0f && currentTime - birth_time > 2000.0f)
            birth_time = 0.0f;
        move_range = Random::Int(1, 5);
        move();
        eat(bushes);
        if (!pop.canAddRats(static_cast<int>(new_rats.size()))) return;
        reproduce(rats, new_rats);
    }

protected:
    std::vector<std::weak_ptr<Creature>>& getChunkContainer(Chunk& chunk) override {
        return chunk.rats;
    }

    void addToChunk(Chunk& chunk) override {
        chunk.rats.push_back(weak_from_this());
    }
};

class Eagle : public Creature {
public:
    Eagle() : Creature(type_::eagle) {
        gender = (rand() % 2 == 0) ? gender_::male : gender_::female;
        eating_range = 2;
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

    void move() override {
        //разная скорость
        const float avoidance_radius = 5.0f;

        bool isHunger = hunger > 500;
        bool isMaturity = age >= maturity_age && birth_time == 0.0f;

        // --- избегаем других волков
        float ax = 0, ay = 0;
        int Wcount = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                int ncx = coord_to_chunkx(Wrap(x + i * CHUNK_SIZE, base_rangex));
                int ncy = coord_to_chunky(Wrap(y + j * CHUNK_SIZE, base_rangey));
                auto result = chunk_grid[ncx][ncy].nearly_creature_square(chunk_grid[ncx][ncy].eagles, x, y, avoidance_radius);
                int count = std::get<0>(result);
                float dx = std::get<1>(result);
                float dy = std::get<2>(result);
                Wcount += count;
                ax += dx;
                ay += dy;
            }
        }
        if (Wcount > 0) {
            ax /= Wcount;
            ay /= Wcount;
            float len = std::sqrt(ax * ax + ay * ay);
            if (len > 1e-6f) { ax /= len; ay /= len; }
        }

        bool needNewDir = (!isDirectionSelect || step <= 0);

        // --- выбираем цель
        if (isHunger) {
            std::pair<float, float> targetRat = searchNearestCreature(x, y, type_::rat, 3, false);
            float RatX = targetRat.first;
            float RatY = targetRat.second;
            if (RatX != -5000.0f) {
                float dx = torusDelta(x, RatX, base_rangex);
                float dy = torusDelta(y, RatY, base_rangey);
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
        else if (isMaturity) {
            std::pair<float, float> target = searchNearestCreature(x, y, type_::eagle, 3, true);
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
        if (age < maturity_age || (currentTime - birth_time) < mateCooldown || dead) return;

        int base_cx = coord_to_chunkx(x);
        int base_cy = coord_to_chunky(y);

        for (auto& w : chunk_grid[base_cx][base_cy].eagles) {
            if (auto partner = w.lock()) {
                if (partner.get() == this || partner->dead) continue;
                if (partner->gender == gender) continue;
                if (partner->age < maturity_age || (currentTime - partner->birth_time) < mateCooldown) continue;

                // расстояние с учётом тора
                float dx = torusDelta(x, partner->x, base_rangex);
                float dy = torusDelta(y, partner->y, base_rangey);
                float dist2 = dx * dx + dy * dy;

                if (dist2 < 100.0f) {
                    auto offspring = std::make_shared<Eagle>();
                    offspring->x = Wrap(x + Random::Int(-5, 5), base_rangex);
                    offspring->y = Wrap(y + Random::Int(-5, 5), base_rangey);
                    offspring->birth_time = currentTime;
                    offspring->gender = (rand() % 2 == 0) ? gender_::male : gender_::female;

                    // Обновляем cooldown родителей
                    birth_time = currentTime;
                    partner->birth_time = currentTime;

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
        if (birth_time != 0.0f && currentTime - birth_time > 2000.0f)
            birth_time = 0.0f;
        move_range = Random::Int(1, 5);
        move();
        eat(rats);
        if (!pop.canAddEagle(static_cast<int>(new_eagles.size()))) return;
        reproduce(eagles, new_eagles);
    }

protected:
    std::vector<std::weak_ptr<Creature>>& getChunkContainer(Chunk& chunk) override {
        return chunk.eagles;
    }

    void addToChunk(Chunk& chunk) override {
        chunk.eagles.push_back(weak_from_this());
    }
};

// Глобальный контейнер существ


std::vector<std::shared_ptr<Rabbit>> rabbits;
std::vector<std::shared_ptr<Tree>> trees;
std::vector<std::shared_ptr<Wolf>> wolves;
std::vector<std::shared_ptr<Bush>> bushes;
std::vector<std::shared_ptr<Eagle>> eagles;
std::vector<std::shared_ptr<Rat>> rats;

