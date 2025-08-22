

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

class Rabbit : public Creature {
public:
    Rabbit() : Creature(type_::rabbit) {
        nutritional_value = 100;
        gender = (rand() % 2 == 0) ? gender_::male : gender_::female;
        eating_range = 2;
        age = 0;
        maturity_age = 1000;
        age_limit = 2000;
        hunger_limit = 500;
        hunger = 0;
    }

    float birth_time = 0.0f;
    bool isDirectionSelect = false;
    int step = 0;
    float nextPositionX = 0;
    float nextPositionY = 0;

    void move() override {
        const int move_range = 2;
        const float avoidance_radius = 5.0f;

        bool isHunger = hunger > 10;
        bool isMaturity = (age >= maturity_age) && (birth_time == 0.0f) && !isHunger;

        // --- Избегание соседей ---
        float ax = 0.0f, ay = 0.0f;
        int nearbyCount = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                int ncx = coord_to_chunkx(Wrap(x + i * CHUNK_SIZE, base_rangex));
                int ncy = coord_to_chunky(Wrap(y + j * CHUNK_SIZE, base_rangey));
                int nearbyCount;
                float ax, ay;
                auto t = chunk_grid[ncx][ncy].nearly_creature(chunk_grid[ncx][ncy].rabbits, x, y, avoidance_radius);
                 nearbyCount = std::get<0>(t);
                 ax = std::get<1>(t);
                 ay = std::get<2>(t);
            }
        }

        if (nearbyCount > 0) {
            ax /= nearbyCount;
            ay /= nearbyCount;
            float len = std::sqrt(ax * ax + ay * ay);
            if (len > 1e-6f) { ax /= len; ay /= len; }
        }

        // --- Выбор цели ---
        bool needNewDir = (!isDirectionSelect || step <= 0);

        if (isMaturity) {
            std::pair<float, float> target = searchNearestCreature(x, y, type_::rabbit, 3, true);
            float targetX = target.first;
            float targetY = target.second;
            if (targetX != -5000.0f) {
                float dx = torusDelta(x, targetX, base_rangex);
                float dy = torusDelta(y, targetY, base_rangey);
                float d = std::sqrt(dx * dx + dy * dy);
                if (d > 1e-3f) {
                    nextPositionX = (dx / d) * move_range;
                    nextPositionY = (dy / d) * move_range;
                    isDirectionSelect = true;
                    step = Random::Int(5, 15);
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
            step = Random::Int(5, 15);
        }
        else {
            --step;
        }

        // --- Комбинируем цель и отталкивание ---
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

    void reproduce(std::vector<std::shared_ptr<Rabbit>>& creatures,
        std::vector<std::shared_ptr<Rabbit>>& new_creatures) {

        if (age < maturity_age || birth_time != 0.0f || dead) return;

        for (auto& other : creatures) {
            if (other->dead || other->type != type_::rabbit || other.get() == this) continue;

            Rabbit* partner = dynamic_cast<Rabbit*>(other.get());
            if (partner && partner->age >= maturity_age && partner->birth_time == 0.0f &&
                partner->gender != gender &&
                distanceSquared(x, y, partner->x, partner->y) < 20.0f) {

                auto offspring = std::make_shared<Rabbit>(*this);
                offspring->age = 0;
                offspring->hunger = 0;
                offspring->dead = false;
                offspring->gender = (rand() % 2 == 0) ? gender_::male : gender_::female;
                offspring->x += Random::Int(-5, 5);
                offspring->y += Random::Int(-5, 5);
                new_creatures.push_back(offspring);
                birth_time = currentTime;
                break;
            }
        }
    }


    void eat() {
        if (hunger <= 10 || dead) return;

        // Определяем, в каком чанке находится существо
        int cx = coord_to_chunkx(x);
        int cy = coord_to_chunky(y);

        if (cx < 0 || cx >= CHUNKS_PER_SIDEX || cy < 0 || cy >= CHUNKS_PER_SIDEY) return;

        Chunk& chunk = chunk_grid[cx][cy];

        // Чем больше трава, тем больше уменьшается голод
        // Например, hunger уменьшается пропорционально growthLevel (0..1)
        float hungerReduction = chunk.grass.growth * 1.0f; // 10 — можно настроить

        hunger -= hungerReduction;

        if (hunger < -100) hunger = 0;

        // Можно дополнительно уменьшить рост травы в чанке из-за поедания
        chunk.grass.growth -= 5;  // немного съели травы

        if (chunk.grass.growth < 0) chunk.grass.growth = 0;
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
        if (birth_time != 0.0f) {
            if (currentTime - birth_time > 2000.0f) {
                birth_time = 0.0f;
            }
        }
        move();
        eat();
        if (!pop.canAddRabbit(static_cast<int>(new_rabbits.size())))
            return;
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
        const int move_range = Random::Int(1, 60);//разная скорость
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
                auto result = chunk_grid[ncx][ncy].nearly_creature(chunk_grid[ncx][ncy].wolfs, x, y, avoidance_radius);
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
                    step = Random::Int(10, 60);
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
                    step = Random::Int(10, 60);
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
            step = Random::Int(10, 60);
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

    void reproduce(std::vector<std::shared_ptr<Wolf>>& creatures,
        std::vector<std::shared_ptr<Wolf>>& new_creatures) {
        if (age < maturity_age || birth_time != 0.0f || dead) return;

        for (auto& other : creatures) {
            if (other->dead || other->type != type_::wolf || other.get() == this) continue;

            Wolf* partner = dynamic_cast<Wolf*>(other.get());
            if (partner && partner->age >= maturity_age && partner->birth_time == 0.0f &&
                partner->gender != gender &&
                distanceSquared(x, y, partner->x, partner->y) < 20.0f) {

                auto offspring = std::make_shared<Wolf>(*this);
                offspring->age = 0;
                offspring->hunger = 0;
                offspring->dead = false;
                offspring->gender = (rand() % 2 == 0) ? gender_::male : gender_::female;
                offspring->x += Random::Int(-5, 5);
                offspring->y += Random::Int(-5, 5);
                new_creatures.push_back(offspring);
                birth_time = currentTime;
                break;
            }
        }
    }

    void eat(std::vector<std::shared_ptr<Rabbit>>& rabbits) {
        if (hunger <= 10 || dead) return;

        auto it = std::find_if(rabbits.begin(), rabbits.end(), [this](const auto& r) {
            return distanceSquared(x, y, r->x, r->y) < eating_range;
            });

        if (it != rabbits.end()) {
            hunger -= (*it)->nutritional_value;
            (*it)->dead = true;
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

        move();
        eat(rabbits);
        if (!pop.canAddWolf(static_cast<int>(new_wolves.size()))) return;
        reproduce(wolves, new_wolves);
    }

protected:
    std::vector<std::weak_ptr<Creature>>& getChunkContainer(Chunk& chunk) override {
        return chunk.wolfs;
    }

    void addToChunk(Chunk& chunk) override {
        chunk.wolfs.push_back(weak_from_this());
    }
};

// Глобальный контейнер существ


std::vector<std::shared_ptr<Rabbit>> rabbits;
std::vector<std::shared_ptr<Tree>> trees;
std::vector<std::shared_ptr<Wolf>> wolfs;

