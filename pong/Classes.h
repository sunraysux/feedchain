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

bool heightW(float worldX, float worldY) {
    auto& heightMap = Textures::Texture[1];

    // 1. Нормализация
    float normalizedX = (worldX + 16384.0f) / 32768.0f;
    float normalizedY = (worldY + 16384.0f) / 32768.0f;

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
    height = height *heightScale;

    return height < waterLevel;
}

int plant_id = 0;
#include "ClassesMain.h"

class Berry : public Plnt {
public:
    Berry() : Plnt(type_::berry) {
        // Èíèöèàëèçàöèÿ ïàðàìåòðîâ ðàñòåíèÿ
        nutritional_value = 200;
        age = 0;
        maturity_age = 100;
        age_limit = 500;
        cont = 4;
    }
    std::shared_ptr<Plnt> createOffspring() override { return std::make_shared<Berry>(); }
    bool canAdd(PopulationManager& pop, size_t newSize) override {
        return pop.canAddBerrys(static_cast<int>(newSize));
    }

    void process(std::vector<std::shared_ptr<Berry>>& new_plants,
        PopulationManager& pop) {
        Plnt::process<Berry>(new_plants, pop);
    }

protected:
    std::vector<std::weak_ptr<Creature>>& getChunkContainer(Chunk& chunk, const int i) override {
        if (i == 1)
            return chunk.berrys;
        else if (i == 2)
            return chunk.rabbit_eat;
        else if (i == 3)
            return chunk.rat_eat;
        else if (i == 4)
            return chunk.bear_eat;
    }
    void addToChunk(Chunk& chunk,bool world) override {

        if(world)
            population.addToChunkWorld(current_chunkWORLD_x, current_chunkWORLD_y, type);
        else {
            chunk.berrys.push_back(weak_from_this());
            chunk.rabbit_eat.push_back(weak_from_this());
            chunk.rat_eat.push_back(weak_from_this());
            chunk.bear_eat.push_back(weak_from_this());
        }
    }
};

class Grass : public Plnt {
public:
    Grass() : Plnt(type_::grass) {
        // Инициализация параметров растения
        nutritional_value = 500;
        maturity_age = 250;
        age_limit = 500;
        age = 0;
        cont = 4;
    }
    std::shared_ptr<Plnt> createOffspring() override { return std::make_shared<Grass>(); }
    bool canAdd(PopulationManager& pop, size_t newSize) override {
        return pop.canAddGrass(static_cast<int>(newSize));
    }

    void process(std::vector<std::shared_ptr<Grass>>& new_plants,
        PopulationManager& pop) {
        age++;
        if ((torusDelta(x, Camera::state.camX, base_rangex) < 1000) && (torusDelta(x, Camera::state.camX, base_rangex) > -1000) &&
            (torusDelta(y, Camera::state.camY, base_rangey) < 1000) && (torusDelta(y, Camera::state.camY, base_rangey) > -1000))
            Plnt::process<Grass>(new_plants, pop);
    }


protected:
    std::vector<std::weak_ptr<Creature>>& getChunkContainer(Chunk& chunk, const int i) override {
        if (i == 1)
            return chunk.grass;
        else if (i == 2)
            return chunk.Plants;
        else if (i == 3)
            return chunk.rabbit_eat;
        else if (i == 4)
            return chunk.rat_eat;
    }
    void addToChunk(Chunk& chunk, const bool world=true) override {
        if (world)
            population.addToChunkWorld(current_chunkWORLD_x, current_chunkWORLD_y, type);
        chunk.grass.push_back(weak_from_this());
        chunk.Plants.push_back(weak_from_this());
        chunk.rabbit_eat.push_back(weak_from_this());
        chunk.rat_eat.push_back(weak_from_this());
    }
};


class Tree : public Plnt {
public:
    Tree() : Plnt(type_::tree) {
        // Инициализация параметров растения
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
        if ((torusDelta(x, Camera::state.camX, base_rangex) < 1000) && (torusDelta(x, Camera::state.camX, base_rangex) > -1000) &&
            (torusDelta(y, Camera::state.camY, base_rangey) < 1000) && (torusDelta(y, Camera::state.camY, base_rangey) > -1000))
        Plnt::process<Tree>(new_plants, pop);
    }

protected:
    std::vector<std::weak_ptr<Creature>>& getChunkContainer(Chunk& chunk, const int i) override {
        if (i == 1)
            return chunk.trees;
        else if (i == 2)
            return chunk.Plants;
    }

    void addToChunk(Chunk& chunk, bool world) override {
        chunk.trees.push_back(weak_from_this());
        chunk.Plants.push_back(weak_from_this()); 
        if (world)
            population.addToChunkWorld(current_chunkWORLD_x, current_chunkWORLD_y, type);
    }
};

class Bush : public Plnt {
public:
    Bush() : Plnt(type_::bush) {
        // Инициализация параметров растения
        nutritional_value = 1000;
        age = 0;
        maturity_age = 500;
        age_limit = 1000;
        berry_count = 0;
        blossoming_age = 80;
        berry_limit = 5;
        cont = 5;
    }

    std::shared_ptr<Plnt> createOffspring() override { return std::make_shared<Bush>(); }
    bool canAdd(PopulationManager& pop, size_t newSize) override {
        return pop.canAddBush(static_cast<int>(newSize));
    }

    void blossoming(std::vector<std::shared_ptr<Berry>>& new_berrys) {


        float reproductionChance = min(0.01f * (age - maturity_age), 0.05f);
        if ((Random::Float(0, 100)) >= (reproductionChance * 100))
            return;
        float t = age / SIZEBUSHES;
        float x1 = x - t / 1.2f;
        float y1 = y;
        float x2 = x + t;
        float y2 = y + t;
        auto seedling = std::make_shared<Berry>();
        seedling->age = 0;
        seedling->dead = false;
        seedling->age_limit = 100 + Random::Int(-10, 10);
        float radiusX = (x2 - x1) / 2.0f;
        float radiusY = (y2 - y1) / 2.0f;
        float angle = Random::Float(0, 3.14f * 2);
        seedling->x = x + radiusX * cos(angle) * Random::Float(0, 1);
        seedling->y = y + radiusY * sin(angle) * Random::Float(0, 1);
        seedling->x = Wrap(seedling->x, base_rangex);
        seedling->y = Wrap(seedling->y, base_rangey);
        seedling->id = id;
        updateChunk();
        new_berrys.push_back(seedling);

    }

    void process(
        std::vector<std::shared_ptr<Bush>>& new_plants,
        std::vector<std::shared_ptr<Berry>>& new_berrys,
        PopulationManager& pop) {
        if (shouldDie()) return;

        if (age > age_limit * 0.95) {
            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    int ncx = coord_to_chunkx(Wrap(x + i * CHUNK_SIZE, base_rangex));
                    int ncy = coord_to_chunky(Wrap(y + j * CHUNK_SIZE, base_rangey));
                    chunk_grid[ncx][ncy].killBerrys(x, y, id);

                }
            }
        }
        age++;


        if (age >= maturity_age && canAdd(pop, 0)) {
            if (tick % 5 == 0) {
                reproduce(new_plants);
            }
        }
        if (age >= blossoming_age && pop.canAddBerrys()) {
            if (tick % 5 == 0) {
                blossoming(new_berrys);
            }
        }

    }

protected:
    std::vector<std::weak_ptr<Creature>>& getChunkContainer(Chunk& chunk, const int i) override {
        if (i == 1)
            return chunk.bushes;
        else if (i == 2)
            return chunk.Plants;
        else if (i == 3)
            return chunk.rabbit_eat;
        else if (i == 4)
            return chunk.rat_eat;
        else if (i == 5)
            return chunk.bear_eat;
    }
    void addToChunk(Chunk& chunk, bool world) override {
        chunk.bushes.push_back(weak_from_this());
        chunk.Plants.push_back(weak_from_this());
        chunk.rabbit_eat.push_back(weak_from_this());
        chunk.rat_eat.push_back(weak_from_this());
        chunk.bear_eat.push_back(weak_from_this());
        if (world)
            population.addToChunkWorld(current_chunkWORLD_x, current_chunkWORLD_y, type);
    }
};

class Rabbit : public Animal {
public:
    Rabbit() : Animal(type_::rabbit) {
        nutritional_value = 1000;
        gender = (Random::Int(0, 1) == 0) ? gender_::male : gender_::female;
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
        cont = 4;

    }


    int stepsTick = 0;
    float dirX = 0.0f, dirY = 0.0f;
    int remainingSteps = 0;
    float speed = 1.0f;
    int MATURITY_TICKS = 200;

    void getFoodContainers(Chunk& c, std::vector<std::weak_ptr<Creature>>& output) override {
        for (auto& weak : c.rabbit_eat) {
            if (!weak.expired()) output.push_back(weak);
        }
    }
    std::vector<type_> getFoodTypes() const override { return { type_::grass }; }
    bool canAdd(PopulationManager& pop, size_t newSize) override {
        return pop.canAddRabbit(static_cast<int>(newSize));
    }
    std::shared_ptr<Animal> createOffspring() override {
        return std::make_shared<Rabbit>();
    }
    std::vector<std::weak_ptr<Creature>>& getMateContainer(Chunk& chunk) override {
        return chunk.rabbits;
    }

    void process(std::vector<std::shared_ptr<Rabbit>>& creature,
        std::vector<std::shared_ptr<Rabbit>>& new_creature,
        PopulationManager& pop) {
        Animal::process<Rabbit>(creature, new_creature, pop);
    }


protected:
    std::vector<std::weak_ptr<Creature>>& getChunkContainer(Chunk& chunk, const int i) override {
        if (i == 1)
            return chunk.rabbits;
        else if (i == 2)
            return chunk.Animals;
        else if (i == 3)
            return chunk.bear_eat;
        else if (i == 4)
            return chunk.wolf_eat;
    }

    void addToChunk(Chunk& chunk, bool world) override {
        chunk.rabbits.push_back(weak_from_this());
        chunk.Animals.push_back(weak_from_this());
        chunk.bear_eat.push_back(weak_from_this());
        chunk.wolf_eat.push_back(weak_from_this());
        if (world)
            population.addToChunkWorld(current_chunkWORLD_x, current_chunkWORLD_y, type);
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
        cont = 3;
        speed = 2.0f;
        nutritional_value = 2000;
    }

    bool isDirectionSelect = false;
    int step = 0;
    float nextPositionX = 0;
    float nextPositionY = 0;
    float birth_time = 0.0f;

    int stepsTick = 0;
    float dirX = 0.0f, dirY = 0.0f;
    int remainingSteps = 0;
    int MATURITY_TICKS = 75;
    void getFoodContainers(Chunk& c, std::vector<std::weak_ptr<Creature>>& output) override {
        for (auto& weak : c.wolf_eat) {
            if (!weak.expired()) output.push_back(weak);
        }
    }
    std::vector<type_> getFoodTypes() const override { return { type_::rabbit }; }
    bool canAdd(PopulationManager& pop, size_t newSize) override {
        return pop.canAddWolf(static_cast<int>(newSize));
    }
    std::shared_ptr<Animal> createOffspring() override {
        return std::make_shared<Wolf>();
    }
    std::vector<std::weak_ptr<Creature>>& getMateContainer(Chunk& chunk) override {
        return chunk.wolves;
    }

    void process(std::vector<std::shared_ptr<Wolf>>& creature,
        std::vector<std::shared_ptr<Wolf>>& new_creature,
        PopulationManager& pop) {
        Animal::process<Wolf>(creature, new_creature, pop);
    }


protected:
    std::vector<std::weak_ptr<Creature>>& getChunkContainer(Chunk& chunk, const int i) override {
        if (i == 1)
            return chunk.wolves;
        else if (i == 2)
            return chunk.Animals;
        else if (i == 3)
            return chunk.bear_eat;
    }

    void addToChunk(Chunk& chunk, bool world) override {
        chunk.wolves.push_back(weak_from_this());
        chunk.Animals.push_back(weak_from_this());
        chunk.bear_eat.push_back(weak_from_this());
        if (world)
            population.addToChunkWorld(current_chunkWORLD_x, current_chunkWORLD_y, type);
    }
};

class Rat : public Animal {
public:
    Rat() : Animal(type_::rat) {
        nutritional_value = 1000;
        gender = (Random::Int(0, 1) == 0) ? gender_::male : gender_::female;
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
        cont = 5;

    }

    bool isUsedInfection = false;

    int stepsTick = 0;
    float dirX = 0.0f, dirY = 0.0f;
    int remainingSteps = 0;
    float speed = 1.0f;
    int MATURITY_TICKS = 25;
    void getFoodContainers(Chunk& c, std::vector<std::weak_ptr<Creature>>& output) override {
        for (auto& weak : c.rat_eat) {
            if (!weak.expired()) output.push_back(weak);
        }
    }
    std::vector<type_> getFoodTypes() const override { return { type_::berry }; }
    bool canAdd(PopulationManager& pop, size_t newSize) override {
        return pop.canAddRat(static_cast<int>(newSize));
    }
    std::shared_ptr<Animal> createOffspring() override {
        return std::make_shared<Rat>();
    }
    std::vector<std::weak_ptr<Creature>>& getMateContainer(Chunk& chunk) override {
        return chunk.rats;
    }

    void process(std::vector<std::shared_ptr<Rat>>& creature,
        std::vector<std::shared_ptr<Rat>>& new_creature,
        PopulationManager& pop) {
        Animal::process<Rat>(creature, new_creature, pop);
    }

protected:
    std::vector<std::weak_ptr<Creature>>& getChunkContainer(Chunk& chunk, const int i) override {
        if (i == 1)
            return chunk.rats;
        else if (i == 2)
            return chunk.Animals;
        else if (i == 3)
            return chunk.bear_eat;
        else if (i == 4)
            return chunk.wolf_eat;
        else if (i == 5)
            return chunk.eagle_eat;
    }

    void addToChunk(Chunk& chunk, bool world) override {
        chunk.rats.push_back(weak_from_this());
        chunk.Animals.push_back(weak_from_this());
        chunk.bear_eat.push_back(weak_from_this());
        chunk.wolf_eat.push_back(weak_from_this());
        chunk.eagle_eat.push_back(weak_from_this());
        if (world)
            population.addToChunkWorld(current_chunkWORLD_x, current_chunkWORLD_y, type);
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
        cont = 3;
        nutritional_value = 1500;
        speed = 3.0f;
    }

    bool isDirectionSelect = false;
    int step = 0;
    float nextPositionX = 0;
    float nextPositionY = 0;
    float birth_tick = 0.0f;
    void getFoodContainers(Chunk& c, std::vector<std::weak_ptr<Creature>>& output) override {
        for (auto& weak : c.eagle_eat) {
            if (!weak.expired()) output.push_back(weak);
        }
    }
    std::vector<type_> getFoodTypes() const override { return { type_::rat }; }
    bool canAdd(PopulationManager& pop, size_t newSize) override {
        return pop.canAddEagle(static_cast<int>(newSize));
    }
    std::shared_ptr<Animal> createOffspring() override {
        return std::make_shared<Eagle>();
    }
    std::vector<std::weak_ptr<Creature>>& getMateContainer(Chunk& chunk) override {
        return chunk.eagles;
    }

    void process(std::vector < std::shared_ptr <Eagle>>& creature,
        std::vector<std::shared_ptr<Eagle>>& new_creature,
        PopulationManager& pop) {
        Animal::process<Eagle>(creature, new_creature, pop);
    }



protected:
    std::vector<std::weak_ptr<Creature>>& getChunkContainer(Chunk& chunk, const int i) override {
        if (i == 1)
            return chunk.eagles;
        else if (i == 2)
            return chunk.Animals;
        else if (i == 3)
            return chunk.wolf_eat;
    }

    void addToChunk(Chunk& chunk, bool world) override {
        if (world)
            population.addToChunkWorld(current_chunkWORLD_x, current_chunkWORLD_y, type);
        chunk.eagles.push_back(weak_from_this());
        chunk.Animals.push_back(weak_from_this());
        chunk.wolf_eat.push_back(weak_from_this());
    }
};

class Bear : public Animal {
public:
    Bear() : Animal(type_::bear) {
        gender = (Random::Int(0, 1) == 0) ? gender_::male : gender_::female;
        eating_range = 10;
        age = 0;
        maturity_age = 2000;
        age_limit = 4000;
        hunger_limit = 1000;
        hunger = 0;
        nutritional_value = 5000;
        speed = 2.0f;
    }

    bool isDirectionSelect = false;
    int step = 0;
    float nextPositionX = 0;
    float nextPositionY = 0;
    float birth_tick = 0.0f;

    void getFoodContainers(Chunk& c, std::vector<std::weak_ptr<Creature>>& output) override {
        for (auto& weak : c.bear_eat) {
            if (!weak.expired()) output.push_back(weak);
        }
    }
    std::vector<type_> getFoodTypes() const override { return { type_::berry,type_::rabbit }; }
    bool canAdd(PopulationManager& pop, size_t newSize) override {
        return pop.canAddBear(static_cast<int>(newSize));
    }
    std::shared_ptr<Animal> createOffspring() override {
        return std::make_shared<Bear>();
    }
    std::vector<std::weak_ptr<Creature>>& getMateContainer(Chunk& chunk) override {
        return chunk.bears;
    }

    void process(std::vector < std::shared_ptr <Bear>>& creature,
        std::vector<std::shared_ptr<Bear>>& new_creature,
        PopulationManager& pop) {
        Animal::process<Bear>(creature, new_creature, pop);
    }



protected:
    std::vector<std::weak_ptr<Creature>>& getChunkContainer(Chunk& chunk, const int i) override {
        if (i == 1)
            return chunk.bears;
        else if (i == 2)
            return chunk.Animals;
    }

    void addToChunk(Chunk& chunk, const bool world=true) override {
        chunk.bears.push_back(weak_from_this());
        chunk.Animals.push_back(weak_from_this());
        if (world)
            population.addToChunkWorld(current_chunkWORLD_x, current_chunkWORLD_y, type);
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
std::vector<std::shared_ptr<Berry>> berrys;
std::vector<std::shared_ptr<Bear>> bears;

