#include <algorithm>
#include <random> 
#include <memory>

float SIZEWOLFS = 10.0f;
float SIZETREES = 10.0f;
float SIZERABBITS = 100.0f;
float base_rangey = 500.0f;
float base_rangex = 1000.0f;
POINT p;
enum class gender_ { male, female };
enum class type_ { tree, rabbit, wolf, grass };

float TimeTic;

#define PI 3.1415926535897932384626433832795f
float DegreesToRadians(float degrees)
{
    return degrees * PI / 180.f;
}
float clamp(float x, float a, float b)
{
    return fmax(fmin(x, b), a);
}

inline float Wrap(float x, float range) {
    float size = range * 2.0f; // полный размер мира
    if (x >= range) x -= size;
    if (x < -range) x += size;
    return x;
}
inline float WrapX(float x) {
    float size = base_rangex * 2.0f; // ширина мира
    x = fmod(x + size, size);        // чтобы всё оказалось в [0, size)
    if (x < 0) x += size;            // страховка для отрицательных значений
    return x - base_rangex;          // смещаем обратно в [-base_rangex, base_rangex)
}

inline float WrapY(float y) {
    float size = base_rangey * 2.0f; // высота мира
    y = fmod(y + size, size);
    if (y < 0) y += size;
    return y - base_rangey;
}

const int CHUNK_SIZE = 50; // Размер чанка
const int CHUNKS_PER_SIDEX = base_rangex * 2 / CHUNK_SIZE;
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
    const int tree_limit = 500;

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
class Chunk;
extern std::vector<std::vector<Chunk>> chunk_grid(
    CHUNKS_PER_SIDEX,
    std::vector<Chunk>(CHUNKS_PER_SIDEY)
);
struct Grass {
    float growthLevel = 1.0f;
    float growth = 100;
    int maxGrowth = 100;
};

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

inline float torusDelta(float from, float to, float size) {
    float d = to - from;
    return Wrap(d, size);
}

struct Chunk {
    std::vector<std::weak_ptr<Creature>> trees;
    std::vector<std::weak_ptr<Creature>> rabbits;
    std::vector<std::weak_ptr<Creature>> wolfs;
    Grass grass;

    template<typename T>
    std::pair<float, float> nearest_mature_creature(const std::vector<std::weak_ptr<T>>& creatures, float x, float y) const {
        float best_dx = 0, best_dy = 0;
        float best_dist2 = 1e9f; // очень большое число
        bool found = false;

        for (const auto& w : creatures) {
            if (auto c = w.lock()) {
                if (c->age < c->maturity_age) continue; // только зрелые
                float dx = torusDelta(x, c->x, base_rangex);
                float dy = torusDelta(y, c->y, base_rangey);
                float dist2 = dx * dx + dy * dy;
                if (dist2 > 0.0f && dist2 < best_dist2) {
                    best_dist2 = dist2;
                    best_dx = dx;
                    best_dy = dy;
                    found = true;
                }
            }
        }

        return found ? std::make_pair(best_dx, best_dy) : std::make_pair(-5000.0f, -5000.0f);
    }

    template<typename T>
    std::tuple<int, float, float> nearly_creature(const std::vector<std::weak_ptr<T>>& creatures,
        float x, float y, float avoidance_radius) {
        float avoidX = 0, avoidY = 0;
        int nearby = 0;
        const float r2 = avoidance_radius * avoidance_radius;

        for (const auto& w : creatures) {
            if (auto c = w.lock()) {
                // короткая дельта на торе (size берём из глобалок)
                float dx = torusDelta(x, c->x, base_rangex);
                float dy = torusDelta(y, c->y, base_rangey);

                float dist2 = dx * dx + dy * dy;
                if (dist2 > 0.0f && dist2 < r2) {
                    // вектор от соседа к себе = отталкивание
                    avoidX += -dx;
                    avoidY += -dy;
                    ++nearby;
                }
            }
        }
        return { nearby, avoidX, avoidY };
    }


    template<typename T>
    int countCreatures(const std::vector<std::weak_ptr<T>>& creatures) const {
        int count = 0;
        for (const auto& weak_creature : creatures) {
            if (!weak_creature.expired()) ++count;
        }
        return count;
    }

    // Шаблонная функция для получения случайных координат существа
    template<typename T>
    std::pair<int, int> RandomCreatureXY(const std::vector<std::weak_ptr<T>>& creatures) const {
        if (countCreatures(creatures) == 0) return { -5000, -5000 };

        int rx = 0, ry = 0;
        int count = 0;

        for (const auto& weak_creature : creatures) {
            if (auto creature = weak_creature.lock()) {
                count++;
                // С вероятностью 1/count заменяем выбранное существо на текущее
                if ((creature->age > creature->maturity_age)&&   (Random::Int(1, count) == 1) ){
                    rx = static_cast<int>(creature->x);
                    ry = static_cast<int>(creature->y);
                }
            }
        }

        return (count > 0) ? std::make_pair(rx, ry) : std::make_pair(-5000, -5000);
    }
    std::tuple<int, float, float> nearly_wolfs(float x, float y, float avoidance_radius) {
        return nearly_creature(wolfs, x, y, avoidance_radius);
    }

    std::tuple<int, float, float> nearly_rabbits(float x, float y, float avoidance_radius) {
        return nearly_creature(rabbits, x, y, avoidance_radius);
    }
    // Оригинальные функции для обратной совместимости
    int countRabbits() const {
        return countCreatures(rabbits);
    }

    std::pair<int, int> RandomRabbitXY() const {
        return RandomCreatureXY(rabbits);
    }

    int countTrees() const {
        int count = 0;
        for (const auto& weak_tree : trees) {
            if (!weak_tree.expired()) ++count;
        }
        return count;
    }

    // Дополнительные удобные функции
    int countWolfs() const {
        return countCreatures(wolfs);
    }

    std::pair<int, int> RandomWolfXY() const {
        return RandomCreatureXY(wolfs);
    }

    void UpdateGrassGrowth() {

        float growthSpeed = 1.0f;    // скорость прироста травы 

        // Трава растет, прибавляем рост пропорционально growthLevel
        if (grass.growth < grass.maxGrowth)
        grass.growth += growthSpeed;
    }
};

std::pair<float, float> search_nearest_rabbit(float x, float y) {
    int center_cx = coord_to_chunkx(x);
    int center_cy = coord_to_chunky(y);

    // сначала проверяем текущий чанк
    auto delta = chunk_grid[center_cx][center_cy].nearest_mature_creature(chunk_grid[center_cx][center_cy].rabbits, x, y);
    if (delta.first != -5000.0f)
        return std::make_pair(x + delta.first, y + delta.second);

    // если не нашли, проверяем соседние чанки в радиусе 1..10
    for (int chunk_factor = 1; chunk_factor <= 10; ++chunk_factor) {
        int R = CHUNK_SIZE * chunk_factor;
        float angle_step = 360.0f / (8 * chunk_factor);
        for (float angle = 0; angle < 360.0f; angle += angle_step) {
            float rad = angle * (PI / 180.0f);
            int dotX = x + R * cos(rad);
            int dotY = y + R * sin(rad);
            dotX = Wrap(dotX, base_rangex);
            dotY = Wrap(dotY, base_rangey);

            int cx = coord_to_chunkx(dotX);
            int cy = coord_to_chunky(dotY);

            auto delta = chunk_grid[cx][cy].nearest_mature_creature(chunk_grid[cx][cy].rabbits, x, y);
            if (delta.first != -5000.0f)
                return std::make_pair(x + delta.first, y + delta.second);
        }
    }

    return std::make_pair(-5000.0f, -5000.0f);
}

std::pair<float, float> search_creature(float x, float y, const std::string& creatureType) {
    int foundX = 0, foundY = 0;
    int center_cx = coord_to_chunkx(x);
    int center_cy = coord_to_chunky(y);

    // Лямбда-функции для условий поиска
    auto condition = [&](const Chunk& chunk) -> bool {
        if (creatureType == "rabbit") return chunk.countRabbits() > 0;
        if (creatureType == "wolf") return chunk.countWolfs() > 0;
        return false;
        };

    auto getCoords = [&](const Chunk& chunk) -> std::pair<int, int> {
        if (creatureType == "rabbit") return chunk.RandomRabbitXY();
        if (creatureType == "wolf") return chunk.RandomWolfXY();
        return { -5000, -5000 };
        };


    auto& chunk = chunk_grid[center_cx][center_cy];
    if (condition(chunk)) {
        return getCoords(chunk);
    }


    // Ищем в соседних чанках
    for (int chunk_factor = 1; chunk_factor <= 10; chunk_factor++) {
        int R = (CHUNK_SIZE * chunk_factor);
        float angel_step = 360.0f / (8 * chunk_factor);

        for (float angle = 0; angle < 360.0f; angle += angel_step) {
            float rad = angle * (PI / 180.0f);
            int dotX = x + R * cos(rad);
            int dotY = y + R * sin(rad);
            dotX = Wrap(dotX, base_rangex);
            dotY = Wrap(dotY, base_rangey);
            int cx = coord_to_chunkx(dotX);
            int cy = coord_to_chunky(dotY);

            auto& chunk = chunk_grid[cx][cy];
            if (condition(chunk)) {
                return getCoords(chunk);
            }

        }
    }

    return std::make_pair(-5000, -5000);
}





