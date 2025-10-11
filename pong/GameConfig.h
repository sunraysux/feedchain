#include <algorithm>
#include <random> 
#include <memory>
#include <limits>
enum class gameState_ {
    MainMenu, game, pause
};

gameState_  gameState = gameState_::MainMenu;


//#include "ecosystem.h"
float x=0;
float y=0;
float z=0;
int seed = 0;
int tick = 0;
int gameSpeed = 3;
int slot_number = 1;
int typeSelect = 1;
int oldGameSpeed = 1;
static int ticloop = 0;
float SIZEGRASS = 50;
float SIZEWOLFS = 100.0f;
float SIZETREES = 100.0f;
float SIZEBUSHES = 50.0f;
float SIZERABBITS = 100.0f;
float SIZEAGLES = 100.0f;
float SIZERATS = 100.0f;
float SIZEBERRYS = 50.0f;
float SIZEBEARS = 100.0f;
float base_rangey = 1024.0f;
float base_rangex = 1024.0f;
float waterLevel = 0.6 + cos(timer::frameBeginTime * .01 * 0.3) * 0.02;
POINT p;
enum class gender_ { male, female };
enum class type_ { tree, rabbit, wolf, grass,bush, eagle,berry,rat,lightning,bear };

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

inline int GetChunk(float x) {
    const float WORLD_HALF = 2048.0f * 8; // 16384
    const float CHUNK_SIZE = 2048.0f;
    const int TOTAL_CHUNKS = 16;

    float normalized = x + WORLD_HALF;
    int chunk = static_cast<int>(normalized / CHUNK_SIZE);
    chunk = chunk % TOTAL_CHUNKS;
    if (chunk < 0) chunk += TOTAL_CHUNKS;
    return chunk;
}


inline float WrapXcam(float x)
{
    x /= 2;
    float size = base_rangex * 2.0f;
    while (x < -base_rangex) x += size;
    while (x > base_rangex) x -= size;
    return x;
}

inline float WrapYcam(float y)
{
    y /= 2;
    float size = base_rangey * 2.0f;
    while (y < -base_rangey) y += size;
    while (y > base_rangey) y -= size;
    return y;
}


inline float WrapX(float x)
{
    float size = base_rangex * 2.0f;
    while (x < -base_rangex) x += size;
    while (x > base_rangex) x -= size;
    return x;
}

inline float WrapY(float y)
{
    float size = base_rangey * 2.0f;
    while (y < -base_rangey) y += size;
    while (y > base_rangey) y -= size;
    return y;
}
const int CHUNK_SIZE = 8; // Размер чанка
const int CHUNKS_PER_SIDEX = base_rangex * 2 / CHUNK_SIZE;
const int CHUNKS_PER_SIDEY = base_rangey * 2 / CHUNK_SIZE;
// секция данных игры  
class Creature;

#include <random>
#include <cstdint>
#include <limits>

class Random {
public:
    static std::mt19937& GetGenerator() {
        static std::mt19937 gen{ std::random_device{}() };
        return gen;
    }

    static float Float(float minVal, float maxVal) {
        std::uniform_real_distribution<float> dist(minVal, maxVal);
        return dist(GetGenerator());
    }

    static int Int(int minVal, int maxVal) {
        std::uniform_int_distribution<int> dist(minVal, maxVal);
        return dist(GetGenerator());
    }

    static bool Chance(float probability) {
        std::bernoulli_distribution dist(probability);
        return dist(GetGenerator());
    }

    static int Int0_100() {
        static std::uniform_int_distribution<int> dist(0, 100);
        return dist(GetGenerator());
    }
    static float Float01() {
        static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        return dist(GetGenerator());
    }

    // xorshift32 — очень быстрый генератор, пригоден для массовых вызовов
    static uint32_t& FastState() {
        static uint32_t state = 123456789u;
        return state;
    }

    static inline uint32_t FastNext() {
        uint32_t& s = FastState();
        s ^= s << 13;
        s ^= s >> 17;
        s ^= s << 5;
        return s;
    }

    static inline int FastInt(int minVal, int maxVal) {
        return minVal + (FastNext() % (maxVal - minVal + 1));
    }

    static inline float FastFloat(float minVal, float maxVal) {
        static const float invMax = 1.0f / static_cast<float>(UINT32_MAX);
        return minVal + (FastNext() * invMax) * (maxVal - minVal);
    }

    static inline bool FastChance(float probability) {
        static const float invMax = 1.0f / static_cast<float>(UINT32_MAX);
        return (FastNext() * invMax) < probability;
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
    int bush_count = 0;
    int eagle_count = 0;
    int grass_count = 0;
    int rat_count = 0;
    int berry_count = 0;
    int bear_count = 0;
    const int grass_limit = 5000;
    const int wolf_limit = 100;
    const int rabbit_limit = 500;
    const int tree_limit = 500;
    const int bush_limit = 500;
    const int eagle_limit = 100;
    const int rat_limit = 500;
    const int berry_limit = 2500;
    const int bear_limit = 500;

    bool canAddWolf(int pending = 0) const {
        return wolf_count + pending < wolf_limit;
    }

    bool canAddRabbit(int pending = 0) const {
        return rabbit_count + pending < rabbit_limit;
    }

    bool canAddTree(int pending = 0) const {
        return tree_count + pending < tree_limit;
    }
    bool canAddBush(int pending = 0) const {
        return bush_count + pending < bush_limit;
    }

    bool canAddEagle(int pending = 0) const {
        return eagle_count + pending < eagle_limit;
    }
    bool canAddRat(int pending = 0) const {
        return rat_count + pending < rat_limit;
    }
    bool canAddGrass(int pending = 0) const {
        return grass_count + pending < grass_limit;
    }
    bool canAddBerrys(int pending = 0) const {
        return berry_count + pending < berry_limit;
    }
    bool canAddBear(int pending = 0) const {
        return bear_count + pending < bear_limit;
    }

    void update(int delta_rabbits, int delta_trees, int delta_wolfs,int delta_bushes, int delta_eagles, int delta_rats, int delta_grass, int delta_berrys, int delta_bears) {
        rabbit_count += delta_rabbits;
        tree_count += delta_trees;
        wolf_count += delta_wolfs;
        bush_count += delta_bushes;
        eagle_count += delta_eagles;
        rat_count += delta_rats;
        grass_count += delta_grass;
        berry_count += delta_berrys;
        bear_count += delta_bears;
    }
};

PopulationManager population;
class Chunk;
extern std::vector<std::vector<Chunk>> chunk_grid(
    CHUNKS_PER_SIDEX,
    std::vector<Chunk>(CHUNKS_PER_SIDEY)
);
//struct Grass {
//    float growthLevel = 1.0f;
//    float growth = 100;
//    int maxGrowth = 100;
//};

inline float distanceSquared(float x1, float y1, float x2, float y2) {
    float dx = x1 - x2;
    float dy = y1 - y2;
    return dx * dx + dy * dy;
}


class Creature : public std::enable_shared_from_this<Creature> {
public:
    float x, y, widht, age_limit, limit, hunger, hunger_limit, maturity_age, eating_range, nutritional_value, nextPositionX, nextPositionY, move_range, step;
    int age;
    int current_chunk_x = -1;
    int current_chunk_y = -1;
    gender_ gender;
    type_ type;
    bool dead = false;
    bool isDirectionSelect = false;
    float birth_tick;
    int berry_count;
    int blossoming_age = 0;
    int berry_limit;
    bool infect = false;
    bool isUsedInfection = false;
    bool isRotten = false;
    int id;
    int cont=2;
    Creature(type_ t) : type(t) {}


    void removeFromChunk() {
        if (current_chunk_x < 0 || current_chunk_y < 0) return;

        auto& chunk = chunk_grid[current_chunk_x][current_chunk_y];
        for (int i = 1;i <= cont;i++) {


            auto& container = getChunkContainer(chunk, i);

            // Удаляем weak_ptr, указывающий на текущий объект
            container.erase(
                std::remove_if(container.begin(), container.end(),
                    [this](const std::weak_ptr<Creature>& wp) {
                        auto sp = wp.lock();
                        return !sp || sp.get() == this;
                    }),
                container.end()
            );
        }

        // Удаляем weak_ptr, указывающий на текущий объект

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

    virtual bool shouldDie() const = 0;

protected:
    // Виртуальный метод для получения нужного контейнера в чанке
    virtual std::vector<std::weak_ptr<Creature>>& getChunkContainer(Chunk& chunk, int i) = 0;
    // Виртуальный метод для добавления в чанк (уже объявлен)
    virtual void addToChunk(Chunk& chunk) = 0;
};


inline float torusDelta(float from, float to, float size) {
    float d = to - from;
    return Wrap(d, size);
}



