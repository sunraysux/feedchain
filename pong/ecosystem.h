#include <algorithm>
#include <random> 
#include <memory>
enum class gender_ { male, female };
enum class type_ { tree, rabbit, wolf, grass };
POINT p;
float TimeTic;
int base_rangey = 500;
int base_rangex = 1000;
const int CHUNK_SIZE = 5; // Размер чанка
const int CHUNKS_PER_SIDEX = base_rangex*2/ CHUNK_SIZE;
const int CHUNKS_PER_SIDEY = base_rangey * 2 / CHUNK_SIZE;
// секция данных игры  
class Creature;

class Random {
public:
    static std::mt19937& GetGenerator() {
        static std::mt19937 gen{ std::random_device{}() }; // Инициализируется один раз
        return gen;
    }

    // Дополнительно — хелперы:
    static float Float(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(GetGenerator());
    }

    static int Int(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(GetGenerator());
    }

    static bool Chance(float probability) { // от 0.0 до 1.0
        std::bernoulli_distribution dist(probability);
        return dist(GetGenerator());
    }
};

inline int coord_to_chunkx(float coord) {
    // Смещаем координату из [-50,50] в [0,100]
    float normalized = coord + base_rangex;
    // Вычисляем индекс и ограничиваем его
    int index = static_cast<int>(normalized / CHUNK_SIZE);
    return clamp(index, 0, CHUNKS_PER_SIDEX - 1);
}

inline int coord_to_chunky(float coord) {
    // Смещаем координату из [-50,50] в [0,100]
    float normalized = coord + base_rangey;
    // Вычисляем индекс и ограничиваем его
    int index = static_cast<int>(normalized / CHUNK_SIZE);
    return clamp(index, 0, CHUNKS_PER_SIDEY - 1);
}


class PopulationManager {
public:
    int rabbit_count = 0;
    int tree_count = 0;
    int wolf_count = 0;
    const int wolf_limit = 1000;
    const int rabbit_limit = 1000;
    const int tree_limit = 1000;

    bool canAddWolf(int pending = 0) const {
        return wolf_count + pending < wolf_limit;
    }

    bool canAddRabbit(int pending = 0) const {
        return rabbit_count + pending < rabbit_limit;
    }

    bool canAddTree(int pending = 0) const {
        return tree_count + pending < tree_limit;
    }

    void update(int delta_rabbits, int delta_plants, int delta_wolfs) {
        rabbit_count += delta_rabbits;
        tree_count += delta_plants;
        wolf_count += delta_wolfs;
    }
};

PopulationManager population;

struct Grass {
    float growthLevel = 1.0f; 
    float growth = 100;
    int maxGrowth = 100;
};

struct Chunk {
    std::vector<std::weak_ptr<Creature>> trees;
    std::vector<std::weak_ptr<Creature>> rabbits;
    std::vector<std::weak_ptr<Creature>> wolfs;
    Grass grass;
    int countRabbits() const {
        int count = 0;
        for (const auto& weak_rabbit : rabbits) {
            if (!weak_rabbit.expired()) ++count;
        }
        return count;
    }
    void UpdateGrassGrowth() {
       
        float growthSpeed = 1.0f;    // скорость прироста травы 

        // Трава растет, прибавляем рост пропорционально growthLevel
        grass.growth += growthSpeed ;
    }
};

std::vector<std::vector<Chunk>> chunk_grid(
    CHUNKS_PER_SIDEX,
    std::vector<Chunk>(CHUNKS_PER_SIDEY)
);

extern std::vector<std::vector<Chunk>> chunk_grid;
extern std::vector<std::shared_ptr<Creature>> creatures;

inline float distanceSquared(float x1, float y1, float x2, float y2) {
    float dx = x1 - x2;
    float dy = y1 - y2;
    return dx * dx + dy * dy;
}

class Creature : public std::enable_shared_from_this<Creature> {
public:
    float x, y, widht, age_limit, limit, hunger, hunger_limit, maturity_age, eating_range, nutritional_value;
    int age;
    int current_chunk_x = -1;
    int current_chunk_y = -1;
    gender_ gender;
    type_ type;
    bool dead = false;
    Creature(type_ t) : type(t) {}


    void removeFromChunk() {
        if (current_chunk_x < 0 || current_chunk_y < 0) return;

        auto& chunk = chunk_grid[current_chunk_x][current_chunk_y];
        auto& container = getChunkContainer(chunk);

        // Удаляем weak_ptr, указывающий на текущий объект
        container.erase(
            std::remove_if(container.begin(), container.end(),
                [this](const std::weak_ptr<Creature>& wp) {
                    auto sp = wp.lock();
                    return !sp || sp.get() == this;
                }),
            container.end()
        );

        current_chunk_x = -1;
        current_chunk_y = -1;
    }
    virtual ~Creature() = default;
    virtual void updateChunk() {
        int new_cx = coord_to_chunkx(x);
        int new_cy = coord_to_chunky(y);

        if (new_cx != current_chunk_x || new_cy != current_chunk_y) {
            removeFromChunk();  // Удаляем из старого чанка

            // Добавляем в новый чанк
            current_chunk_x = new_cx;
            current_chunk_y = new_cy;
            addToChunk(chunk_grid[new_cx][new_cy]);
        }
    }

    virtual void move() = 0;
    virtual bool shouldDie() const = 0;

protected:
    // Виртуальный метод для получения нужного контейнера в чанке
    virtual std::vector<std::weak_ptr<Creature>>& getChunkContainer(Chunk& chunk) = 0;

    // Виртуальный метод для добавления в чанк (уже объявлен)
    virtual void addToChunk(Chunk& chunk) = 0;
};

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

            float distance = Random::Int(1,10); // 3–50
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
            const float radius2 = 5.0f*5.0f;

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

    void eat(std::vector<std::shared_ptr<Creature>>& creatures)  {} // Растения не едят

    bool shouldDie() const override {
        return dead || age > age_limit ;
    }

    void process(std::vector<std::shared_ptr<Tree>>& creatures,
        std::vector<std::shared_ptr<Tree>>& new_trees,
        PopulationManager& pop)  {
        if (shouldDie()) return;
        age++;
        if (age >= maturity_age && pop.canAddTree(static_cast<int>(new_trees.size()))){
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
        std::vector<std::shared_ptr<Rabbit>>& new_creatures)  {

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

    void move() override {
        int move_range = 1;
        x += Random::Int(-move_range, move_range);
        y += Random::Int(-move_range, move_range);

        // Проверка границ
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

//основной процесс

void UpdateAllGrass() {
    for (int cx = 0; cx < CHUNKS_PER_SIDEX; ++cx) {
        for (int cy = 0; cy < CHUNKS_PER_SIDEY; ++cy) {
            chunk_grid[cx][cy].UpdateGrassGrowth();
        }
    }
}
void ProcessCreatures(PopulationManager& pop) {

    int dead_rabbits = 0;
    int dead_plants = 0;
    int dead_wolfs = 0;
    std::vector<std::shared_ptr<Wolf>> new_wolfs;
    std::vector<std::shared_ptr<Rabbit>> new_rabbits;        //список для новых существ
    std::vector<std::shared_ptr<Tree>> new_trees;          //список для новых существ
   
    for (auto& rabbit : rabbits) rabbit->process(rabbits, new_rabbits, trees, pop);
    for (auto& tree : trees) tree->process(trees, new_trees, pop);
    for (auto& wolf : wolfs) wolf->process(wolfs, new_wolfs, rabbits, pop);


    auto remove_dead = [](auto& container, int& counter) {
        using ContainerType = typename std::remove_reference<decltype(container)>::type;
        using ValueType = typename ContainerType::value_type;

        container.erase(
            std::remove_if(container.begin(), container.end(),
                [&](const ValueType& entity) {
                    if (entity->shouldDie()) {
                        counter++;
                        entity->removeFromChunk();
                        return true;
                    }
                    return false;
                }
            ),
            container.end()
        );
        };

    remove_dead(rabbits, dead_rabbits);
    remove_dead(wolfs, dead_wolfs);
    remove_dead(trees, dead_plants);

    // 3.  добавления новых существ
    auto add_new_entities = [](auto& dest, auto& src) {
        dest.reserve(dest.size() + src.size());
        for (auto& entity : src) {
            entity->updateChunk();
            dest.emplace_back(std::move(entity));
        }
        src.clear();
        };
    pop.update(
        static_cast<int>(new_rabbits.size()) - dead_rabbits,//обновление статистики
        static_cast<int>(new_trees.size()) - dead_plants,
        static_cast<int>(new_wolfs.size()) - dead_wolfs
    );
    add_new_entities(rabbits, new_rabbits);
    add_new_entities(wolfs, new_wolfs);
    add_new_entities(trees, new_trees);

}
//инициализация игры
void InitGame() {
    std::random_device rd;
    std::mt19937 gen(rd()); // генератор
    std::uniform_real_distribution<float> plant_dist(-base_rangex , base_rangey );

    Textures::LoadTextureFromFile(1, L"Debug/tree.png");
    Textures::LoadTextureFromFile(2, L"Debug/animal.png");
    Textures::LoadTextureFromFile(3, L"Debug/animal0.jpg");
    Textures::LoadTextureFromFile(4, L"Debug/grass.jpg");
    Textures::LoadTextureFromFile(5, L"Debug/grass2.jpg");
    Textures::LoadTextureFromFile(6, L"Debug/grass3.jpg");
    // Начальные растения
    for (int i = 0; i < 500; i++) {
        auto tree = std::make_shared<Tree>();
        tree->x =Random::Int(-base_rangex, base_rangex);
        tree->y = Random::Int(-base_rangey, base_rangey);
        tree->age = Random::Int(0, 500);
        tree->updateChunk();
        trees.push_back(tree);
        population.tree_count++;
    }

   // Начальные кролики
   for (int i = 0; i < 500; i++) {
       auto rabbit = std::make_shared<Rabbit>();
       rabbit->y = Random::Int(-base_rangey, base_rangey);
       rabbit->x = Random::Int(-base_rangex, base_rangex);
       rabbit->hunger = Random::Int(-50, 50);
       rabbit->age = Random::Int(0, 50);
       rabbits.push_back(rabbit);
       population.rabbit_count++;
   }
   for (int i = 0; i < 0; i++) {
       auto wolf = std::make_shared<Wolf>();
       wolf->y = Random::Int(-base_rangey, base_rangey);
       wolf->x = Random::Int(-base_rangex, base_rangex);
       wolf->hunger = Random::Int(-50, 50);
       wolf->age = Random::Int(0, 50);
       wolfs.push_back(wolf);
       population.wolf_count++;
   }
}
void mouse()
{
    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
        GetCursorPos(&p);
        float X = p.x;
        float Y = p.y;
        float ndcX = ((float)p.x / width) * 2.0f - 1.0f;
        float ndcY = (1.0f - (float)p.y / height) * 2.0f - 1.0f;
        XMMATRIX view = XMMatrixLookAtLH(Camera::state.Eye, Camera::state.at, Camera::state.Up);
        XMMATRIX proj = XMMatrixOrthographicLH(Camera::state.widthzoom, Camera::state.heightzoom, 0.01f, 1.0f);
        XMMATRIX invViewProj = XMMatrixInverse(nullptr, view * proj);
        XMVECTOR mouseNDC = XMVectorSet(ndcX, ndcY, 0.0f, 1.0f);
        XMVECTOR worldPos = XMVector3TransformCoord(mouseNDC, invViewProj);

        float worldX = XMVectorGetX(worldPos);
        float worldY = XMVectorGetY(worldPos);

        auto wolf = std::make_shared<Wolf>();
        worldX = clamp(worldX, -base_rangex, base_rangex);
        worldY = clamp(worldY, -base_rangey, base_rangey);
        wolf->y = worldY;
        wolf->x = worldX;
        wolf->hunger = 0;
        wolf->age = 0;
        wolfs.push_back(wolf);
        population.wolf_count++;

    }

}

void Showpopulations() {

    // Визуализация популяций (две полоски)                                                                  // тут нормализуется количество существ до 1
                                                                                                             // в зависимости от лимита
    Shaders::vShader(2);                                                                                     // и если полоса снизу доходит до края карты
    Shaders::pShader(2);                                                                                     // то количество существ в списке достигло лимита
    float rabbitRatio = min(                                                                                 //
        static_cast<float>(population.rabbit_count)*2 / population.rabbit_limit,                               //
        2.0f                                                                                                 //
    );                                                                                                       //
                                                                                                             //
    float plantRatio = min(                                                                                  //
        static_cast<float>(population.tree_count)*2 / population.tree_limit,                                 //
        2.0f                                                                                                 //
    );       

    float wolfRatio = min(                                                                                  //
        static_cast<float>(population.wolf_count)*2 / population.wolf_limit,                                 //
        2.0f                                                                                                 //
    );

    ConstBuf::global[0] = XMFLOAT4(                                                                          //
        rabbitRatio,                                                                                         //
        plantRatio,
        wolfRatio,
        0
    );

    ConstBuf::Update(5, ConstBuf::global);
    ConstBuf::ConstToVertex(5);
    Draw::NullDrawer18(1); 
    
}
int BATCH_SIZE = 4000;
void DrawBatchedInstances(int textureIndex, const std::vector<XMFLOAT4>& instances) {
    if (instances.empty()) return;

    context->PSSetShaderResources(0, 1, &Textures::Texture[textureIndex].TextureResView);

    for (size_t start = 0; start < instances.size(); start += BATCH_SIZE) {
        size_t count = min(BATCH_SIZE, static_cast<int>(instances.size() - start));

        // Копируем порцию данных в ConstBuf::global
        std::copy(instances.begin() + start, instances.begin() + start + count, ConstBuf::global);

        ConstBuf::Update(5, ConstBuf::global);
        ConstBuf::ConstToVertex(5);
        Draw::NullDrawer(1, static_cast<int>(count));
    }
}

void ShowRacketAndBall() {
    // Векторы для разных групп травы
    std::vector<XMFLOAT4> lowGrowthInstances;
    std::vector<XMFLOAT4> midGrowthInstances;
    std::vector<XMFLOAT4> highGrowthInstances;

    // Собираем траву по чанкам
    for (int cy = CHUNKS_PER_SIDEY - 1; cy >= 0; --cy) {
        for (int cx = 0; cx < CHUNKS_PER_SIDEX; ++cx) {
            const Chunk& chunk = chunk_grid[cx][cy];
            int x1 = cx * CHUNK_SIZE - base_rangex;
            int y1 = cy * CHUNK_SIZE - base_rangey;
            int x2 = x1 + CHUNK_SIZE;
            int y2 = y1 + CHUNK_SIZE;
            XMFLOAT4 rect(x1, y1, x2, y2);

            if (chunk.grass.growth < 33) {
                lowGrowthInstances.push_back(rect);
            }
            else if (chunk.grass.growth < 66) {
                midGrowthInstances.push_back(rect);
            }
            else {
                highGrowthInstances.push_back(rect);
            }
        }
    }

    // Отрисовываем траву батчами
    DrawBatchedInstances(6, lowGrowthInstances);
    DrawBatchedInstances(5, midGrowthInstances);
    DrawBatchedInstances(4, highGrowthInstances);

    // Универсальная функция для сбора и отрисовки существ
    auto drawCreatures = [&](int textureIndex, auto&& getCreatureList, float ageScale) {
        std::vector<XMFLOAT4> instances;

        for (int cy = CHUNKS_PER_SIDEY - 1; cy >= 0; --cy) {
            for (int cx = 0; cx < CHUNKS_PER_SIDEX; ++cx) {
                const auto& creatureList = getCreatureList(chunk_grid[cx][cy]);
                for (const auto& weakPtr : creatureList) {
                    if (auto c = weakPtr.lock()) {
                        float t = c->age / ageScale;
                        float x1 = c->x - t / 1.2f;
                        float y1 = c->y;
                        float x2 = c->x + t;
                        float y2 = c->y + t;
                        instances.emplace_back(x1, y1, x2, y2);
                    }
                }
            }
        }

        DrawBatchedInstances(textureIndex, instances);
        };

    
    drawCreatures(2, [](const Chunk& c) -> const std::vector<std::weak_ptr<Creature>>&{ return c.rabbits; }, 100.0f);
    drawCreatures(3, [](const Chunk& c) -> const std::vector<std::weak_ptr<Creature>>&{ return c.wolfs; }, 50.0f);
    drawCreatures(1, [](const Chunk& c) -> const std::vector<std::weak_ptr<Creature>>&{ return c.trees; }, 10.0f);
}