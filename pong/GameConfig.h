#include <algorithm>
#include <random> 
#include <memory>

float SIZEWOLFS = 10.0f;
float SIZETREES = 10.0f;
float SIZERABBITS = 10.0f;
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

const int CHUNK_SIZE = 10; // Размер чанка
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
    const int wolf_limit = 500;
    const int rabbit_limit = 500;
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

    int countTrees() const {
        int count = 0;
        for (const auto& weak_tree : trees) {
            if (!weak_tree.expired()) ++count;
        }
        return count;
    }

    void UpdateGrassGrowth() {

        float growthSpeed = 1.0f;    // скорость прироста травы 

        // Трава растет, прибавляем рост пропорционально growthLevel
        grass.growth += growthSpeed;
    }

    std::pair<int, int> RabbitXY() const {
        if (countRabbits() == 0) return { 0,0 };
        int rx;
        int ry;
        for (const auto& weak_rabbit : rabbits) {
            if (auto rabbit = weak_rabbit.lock()) {
                rx = static_cast<int>(rabbit->x);
                ry = static_cast<int>(rabbit->y);
                return { rx,ry };
            }
        }
    }
};


inline float distanceSquared(float x1, float y1, float x2, float y2) {
    float dx = x1 - x2;
    float dy = y1 - y2;
    return dx * dx + dy * dy;
}




