

class Tree : public Creature {
public:
    Tree() : Creature(type_::tree) {
        // Инициализация параметров растения
        nutritional_value = 100;
        age = 0;
        maturity_age = 115;
        age_limit = 1170;
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

            float distance = Random::Int(1, 10); // 3–50
            float angle = Random::Float(0, 3.14 * 2);
            seedling->x += distance * cos(angle);
            seedling->y += distance * sin(angle);

            // Обрезка по границам
            seedling->x = clamp(seedling->x, -base_rangex, base_rangex);
            seedling->y = clamp(seedling->y, -base_rangey, base_rangey);

            // Проверка минимального расстояния (например, 2.0f)
            bool tooClose = false;

            // Проверка плотности (например, не более 10 растений в радиусе 5)
            int nearbyCount = 0;
            const float radius2 = 5.0f * 5.0f;

            for (const auto& other : all_trees) {
                if (!other) continue;
                if (distanceSquared(seedling->x, seedling->y, other->x, other->y) < radius2) {
                    nearbyCount++;
                    if (nearbyCount >= 5) break;
                }
            }

            if (nearbyCount >= 5) continue;
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
        nutritional_value = 50;
        gender = (rand() % 2 == 0) ? gender_::male : gender_::female;
        eating_range = 2;
        age = 0;
        maturity_age = 100;
        age_limit = 200;
        hunger_limit = 50;
        hunger = 0;
    }

    void move() override {
        int move_range = 1;
        x += Random::Int(-move_range, move_range);
        y += Random::Int(-move_range, move_range);

        // Проверка границ
        x = clamp(x, -base_rangex, base_rangex);
        y = clamp(y, -base_rangey, base_rangey);
        updateChunk();
    }

    void reproduce(std::vector<std::shared_ptr<Rabbit>>& creatures,
        std::vector<std::shared_ptr<Rabbit>>& new_creatures) {

        if (age < maturity_age || dead) return;

        for (auto& other : creatures) {
            if (other->dead || other->type != type_::rabbit || other.get() == this) continue;

            Rabbit* partner = dynamic_cast<Rabbit*>(other.get());
            if (partner && partner->age >= maturity_age &&
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
        maturity_age = 100;
        age_limit = 200;
        hunger_limit = 50;
        hunger = 0;
    }
    bool isDirectionSelect = false;
    int step = 0;
    float nextPositionX;
    float nextPositionY;
    void move() override {
        int move_range = 1;
        bool isHunger = hunger > 10;

        if (!isHunger) {
            if (!isDirectionSelect) {
                isDirectionSelect = true;
                step = Random::Int(2, 5);
                nextPositionX = Random::Int(-move_range, move_range);
                nextPositionY = Random::Int(-move_range, move_range);
            }
            if (isDirectionSelect) {
                step -= 1;
                if (step == 0) {
                    isDirectionSelect = false;
                }
                x += nextPositionX;
                y += nextPositionY;
            }

        }
        else {
            int cx = coord_to_chunkx(x);
            int cy = coord_to_chunky(y);
            int rabbitX = 0;
            int rabbitY = 0;
            bool broke = false;
            for (int i = -1; (cx + i > 0) && (cx + i < cx + 1) && (cx + i < CHUNKS_PER_SIDEX); i++) {
                for (int j = -1; (cy + j > 0) && (cy + j < cy + 1) && (cy + j < CHUNKS_PER_SIDEY); j++) {
                    if (chunk_grid[cx + i][cy + j].countRabbits() == 0) {
                        continue;
                    }
                    std::pair<int, int> rabbitCoords = chunk_grid[cx + i][cy + i].RabbitXY();
                    rabbitX = rabbitCoords.first;
                    rabbitY = rabbitCoords.second;
                    broke = true;
                    break;
                }
                if (broke) break;
            }
            if (rabbitX != 0 || rabbitY != 0) {
                if (rabbitX > x) {
                    x += move_range;
                }
                else {
                    x -= move_range;
                }
                if (rabbitY > y) {
                    y += move_range;
                }
                else {
                    y -= move_range;
                }
            }
            else {

                if (!isDirectionSelect) {
                    isDirectionSelect = true;
                    step = Random::Int(2, 5);
                    nextPositionX = Random::Int(-move_range, move_range);
                    nextPositionY = Random::Int(-move_range, move_range);
                }
                if (isDirectionSelect) {
                    step -= 1;
                    if (step == 0) {
                        isDirectionSelect = false;
                    }
                    x += nextPositionX;
                    y += nextPositionY;
                }
            }

        }
        x = clamp(x, -base_rangex, base_rangex);
        y = clamp(y, -base_rangey, base_rangey);
        updateChunk();
    }

    void reproduce(std::vector<std::shared_ptr<Wolf>>& creatures,
        std::vector<std::shared_ptr<Wolf>>& new_creatures) {

        if (age < maturity_age || dead) return;

        for (auto& other : creatures) {
            if (other->dead || other->type != type_::wolf || other.get() == this) continue;

            Wolf* partner = dynamic_cast<Wolf*>(other.get());
            if (partner && partner->age >= maturity_age &&
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

