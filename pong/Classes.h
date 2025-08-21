

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
            if (chunk_grid[xc][yc].countTrees()>50) continue;
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

        float ax = 0.0f, ay = 0.0f;
        int Rcount = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                int ncx = coord_to_chunkx(Wrap(x + i * CHUNK_SIZE, base_rangex));
                int ncy = coord_to_chunky(Wrap(y + j * CHUNK_SIZE, base_rangey));
                auto res = chunk_grid[ncx][ncy].nearly_rabbits(x, y, avoidance_radius);
                Rcount += std::get<0>(res);
                ax += std::get<1>(res);
                ay += std::get<2>(res);
            }
        }
        if (Rcount > 0) {
            ax /= Rcount; ay /= Rcount;
            float len = std::sqrt(ax * ax + ay * ay);
            if (len > 1e-6f) { ax /= len; ay /= len; } // нормируем до 1
        }

        // --- выбираем цель (фикс проверки на "не найдено")
        bool needNewDir = (!isDirectionSelect || step <= 0);

        if (isMaturity) {
            auto target = search_nearest_rabbit(x, y);
            bool found = target.first != -5000.0f;
            if (found) {
                float dx = torusDelta(x, target.first, base_rangex);
                float dy = torusDelta(y, target.second, base_rangey);
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
            // тикаем счётчик, если направление уже есть
            --step;
        }

        // --- комбинируем цель и отталкивание
        const float avoidanceWeight = 0.1f; // подбери 0.1..0.5
        float moveX = nextPositionX + ax * (move_range * avoidanceWeight);
        float moveY = nextPositionY + ay * (move_range * avoidanceWeight);

        // ограничиваем итоговую скорость
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
                distanceSquared(x, y, partner->x, partner->y) < 200.0f) {

                auto offspring = std::make_shared<Rabbit>(*this);
                offspring->age = 0;
                offspring->hunger = 0;
                offspring->dead = false;
                offspring->birth_time = 0;
                offspring->gender = (rand() % 2 == 0) ? gender_::male : gender_::female;
                offspring->x += Random::Int(-5, 5);
                offspring->y += Random::Int(-5, 5);
                new_creatures.push_back(offspring);
                birth_time = currentTime;
                isDirectionSelect = false;
                break;
            }
        }
    }


    void eat() {
        if (hunger <= 100 || dead) return;

        // Определяем, в каком чанке находится существо
        int cx = coord_to_chunkx(x);
        int cy = coord_to_chunky(y);

        if (cx < 0 || cx >= CHUNKS_PER_SIDEX || cy < 0 || cy >= CHUNKS_PER_SIDEY) return;

        Chunk& chunk = chunk_grid[cx][cy];

        // Чем больше трава, тем больше уменьшается голод
        // Например, hunger уменьшается пропорционально growthLevel (0..1)
        float hungerReduction = chunk.grass.growth * 5.0f; // 10 — можно настроить

        hunger -= hungerReduction;

        if (hunger < 0) hunger = 0;

        // Можно дополнительно уменьшить рост травы в чанке из-за поедания
        chunk.grass.growth -= 50;  // немного съели травы

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
            if (currentTime - birth_time > 200.0f) {
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
        maturity_age = 300;
        age_limit = 500;
        hunger_limit = 200;
        hunger = 0;
    }
    bool isDirectionSelect = false;
    int step = 0;
    float nextPositionX = 0;
    float nextPositionY = 0;
    float birth_time = 0.0f;
    void move() override {

        const int move_range = 2;  // Увеличил скорость движения
        const float avoidance_radius = 5.0f;  // Радиус избегания других волков
        bool isHunger = hunger > 10;
        bool isMaturity = age >= maturity_age && birth_time == 0.0f;
        float ax = 0;
        float ay = 0;
        int Wcount = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                int new_cx = x + i * CHUNK_SIZE;
                int new_cy = y + j * CHUNK_SIZE;
                new_cx = Wrap(new_cx, base_rangex);
                new_cy = Wrap(new_cy, base_rangey);

                new_cx = coord_to_chunkx(new_cx);
                new_cy = coord_to_chunky(new_cy);
                auto result = chunk_grid[new_cx][new_cy].nearly_wolfs(x, y, avoidance_radius);
                Wcount += std::get<0>(result);
                ax += std::get<1>(result);
                ay += std::get<2>(result);

            }
        }

        if (Wcount > 0) {
            ax /= Wcount;
            ay /= Wcount;
            float length = sqrt(ax * ax + ay * ay);
            if (length > 0) {
                ax = (ax / length) * move_range;
                ay = (ay / length) * move_range;
            }
        }
        //если не голоден и не может размножаться
        if (!isHunger && !isMaturity) {
            if (!isDirectionSelect || --step <= 0) {
                nextPositionX = Random::Int(-move_range, move_range);
                nextPositionY = Random::Int(-move_range, move_range);
                isDirectionSelect = true;
                step = Random::Int(10, 30);
            }

        }
        //если голоден
        else if (isHunger) {


            auto rabbitCoords = search_creature(x, y, "rabbit");
            float rabbitX = rabbitCoords.first;
            float rabbitY = rabbitCoords.second;
            if (rabbitX != 0 || rabbitY != 0) {
                float dx = rabbitX - x;
                float dy = rabbitY - y;
                float dist = sqrt(dx * dx + dy * dy);

                if (dist > 0) {
                    nextPositionX = (dx / dist) * move_range;
                    nextPositionY = (dy / dist) * move_range;
                }
            }
            else if (!isDirectionSelect || --step <= 0) {
                // Если кролик не найден - блуждаем
                nextPositionX = Random::Int(-move_range, move_range);
                nextPositionY = Random::Int(-move_range, move_range);
                isDirectionSelect = true;
                step = Random::Int(10, 30);
            }
        }

        //если не голоден и хочет размножаться
        else {
            auto wolfCoords = search_creature(x, y, "wolf");
            float wolfX = wolfCoords.first;
            float wolfY = wolfCoords.second;
            if (wolfX != 0 || wolfY != 0) {
                float dx = wolfX - x;
                float dy = wolfY - y;
                float dist = sqrt(dx * dx + dy * dy);

                if (dist > 0) {
                    nextPositionX = (dx / dist) * move_range;
                    nextPositionY = (dy / dist) * move_range;
                }
            }
            else if (!isDirectionSelect || --step <= 0) {
                // Если кролик не найден - блуждаем
                nextPositionX = Random::Int(-move_range, move_range);
                nextPositionY = Random::Int(-move_range, move_range);
                isDirectionSelect = true;
                step = Random::Int(10, 30);
            }
        }



        float moveX = nextPositionX + ax;
        float moveY = nextPositionY + ay;

        // Нормализуем, если суммарный вектор слишком большой
        float moveLength = sqrt(moveX * moveX + moveY * moveY);
        if (moveLength > move_range) {
            moveX = (moveX / moveLength) * move_range;
            moveY = (moveY / moveLength) * move_range;
        }

        x += moveX;
        y += moveY;
        x = Wrap(x, base_rangex);
        y = Wrap(y, base_rangey);

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


    void eat(std::vector<std::shared_ptr<Rabbit>>& rabbit) {
        if (hunger <= 10 || dead) return;

        auto it = std::find_if(rabbit.begin(), rabbit.end(), [this](const auto& p) {
            return distanceSquared(x, y, p->x, p->y) < eating_range;
            });

        if (it != rabbit.end()) {
            hunger -= (*it)->nutritional_value;
            (*it)->dead = true;
        }
    }

    bool shouldDie() const override {
        return dead || age > age_limit || hunger > hunger_limit;

    }

    void process(std::vector<std::shared_ptr<Wolf>>& wolf,
        std::vector<std::shared_ptr<Wolf>>& new_wolfs,
        std::vector<std::shared_ptr<Rabbit>>& rabbit,
        PopulationManager& pop) {
        if (shouldDie()) return;
        age++; hunger++;
        if (birth_time != 0.0f) {
            if (currentTime - birth_time > 2000.0f) {
                birth_time = 0.0f;
            }
        }
        move();
        eat(rabbit);
        if (!pop.canAddWolf(static_cast<int>(new_wolfs.size())))
            return;
        reproduce(wolf, new_wolfs);
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

