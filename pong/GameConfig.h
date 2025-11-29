#include <algorithm>
#include <random> 
#include <memory>
#include <limits>

int tick = 0;
float base_rangey = 32768.0;
float base_rangex = 32768.0;
float clamp(float x, float a, float b)
{
    return fmax(fmin(x, b), a);
}
int gameSpeed = 1;
int oldGameSpeed = 1;
int currentStartButton = 1;
float cursorY1 = -0.35;
float cursorY2 = -0.38;
int slot_number = 1;
int typeSelect = 1;
inline float Wrap(float x, float range) {
    if (x >= range) x -= range;
    if (x < 0) x += range;
    return x;
}
enum class gameState_ {
    MainMenu, game, pause
};

gameState_  gameState = gameState_::MainMenu;


//#include "ecosystem.h"
int tickSTAT = -1000;
float x = 0;
float y = 0;
float z = 0;
int plants_pop[4000];
int herbivores_pop[4000];
int hunter_pop[4000];
int pmax=0;
int hemax=0;
int humax=0;
int stat_size = 0;
int seed = 0;


int herbivoresTick = 30;
int hunterTick = 50;
int deadTick = 50;

int rabbitSpawnTick = -1000;
int ratSpawnTick = -1000;
int wolfSpawnTick = -1000;
int eagleSpawnTick = -1000;
int bearSpawnTick = -1000;
int lightingSpawnTick = -5000;






int virus = 0;
bool info = false;
bool settings = false;
bool statistik = false;
bool hunterStat = false;
bool herbivoresStat = false;
bool plantStat = false;

float SIZEGRASS = 50;
float SIZEWOLFS = 100.0f;
float SIZETREES = 100.0f;
float SIZEBEARS = 100.0f;
float SIZEBUSHES = 50.0f;
float SIZERABBITS = 100.0f;
float SIZEAGLES = 100.0f;
float SIZERATS = 100.0f;
float SIZEBERRYS = 50.0f;



float waterLevel = 0.6 + cos(timer::frameBeginTime * .01 * 0.3) * 0.02;

POINT p;

enum class gender_ { male, female };
enum class type_ { tree, rabbit, wolf, grass, bush, eagle, berry, rat, lightning, bear };

float TimeTic;





#define PI 3.1415926535897932384626433832795f
float DegreesToRadians(float degrees)
{
    return degrees * PI / 180.f;
}




const int CHUNK_SIZE = 8; // Размер чанка
int xmin = 1024*4;
int ymin = 1024*4;
const int CHUNKS_PER_SIDEX = xmin * 2 / CHUNK_SIZE;
const int CHUNKS_PER_SIDEY = ymin * 2 / CHUNK_SIZE;

static const int LARGE_CHUNK_SIZE = 128; 
static const int CHUNKS_PER_SIDE_LARGE = base_rangex/128;
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

inline int coord_to_large_chunkx(float coord) {
    // Смещаем координату в положительный диапазон [0, 2048]
    float normalized = coord;
    // Вычисляем индекс и ограничиваем его
    int index = static_cast<int>(normalized / LARGE_CHUNK_SIZE);
    return clamp(index, 0, CHUNKS_PER_SIDE_LARGE - 1);
}

inline int coord_to_large_chunky(float coord) {
    // Смещаем координату в положительный диапазон [0, 2048]
    float normalized = coord;
    // Вычисляем индекс и ограничиваем его
    int index = static_cast<int>(normalized / LARGE_CHUNK_SIZE);
    return clamp(index, 0, CHUNKS_PER_SIDE_LARGE - 1);
}



float lerp(float x1, float x2, float a)
{
    return x1 * (1 - a) + x2 * a;
}

float length(float x1, float y1, float x2, float y2)
{
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}


inline float distanceSquared(float x1, float y1, float x2, float y2) {
    float dx = x1 - x2;
    float dy = y1 - y2;
    return dx * dx + dy * dy;
}

inline float torusDeltaSigned(float from, float to, float size) {
    if (size <= 0.0f) return to - from;
    float half = size * 0.5f;
    float d = to - from;
    d = std::fmod(d + half, size);
    if (d < 0.0f) d += size;
    d -= half;
    return d;
}

inline float torusDeltaA(float from, float to, float size) {
    float diff = fabs(to - from);
    if (diff > size / 2)
        return size - diff;
    else
        return diff;
}
