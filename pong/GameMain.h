inline int coord_to_chunkx(float coord) {
    // Смещаем координату из [-50,50] в [0,100]
    float normalized = coord + xmin - Camera::state.camX;
    // Вычисляем индекс и ограничиваем его
    int index = static_cast<int>(normalized / CHUNK_SIZE);
    return clamp(index, 0, CHUNKS_PER_SIDEX - 1);
}

inline int coord_to_chunky(float coord) {
    // Смещаем координату из [-50,50] в [0,100]
    float normalized = coord + ymin - Camera::state.camY;
    // Вычисляем индекс и ограничиваем его
    int index = static_cast<int>(normalized / CHUNK_SIZE);
    return clamp(index, 0, CHUNKS_PER_SIDEY - 1);
}


struct ChunkWorld {
    int rabbit_sum;
    int wolf_sum;
    int bear_sum;
    int eagle_sum;
    int tree_sum;
    int bush_sum;
    int grass_sum;
    int berry_sum;
    int rat_sum;
};

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
    const int grass_limit = 50000;
    const int wolf_limit = 5000;
    const int rabbit_limit = 5000;
    const int tree_limit = 5000;
    const int bush_limit = 20000;
    const int eagle_limit = 10000;
    const int rat_limit = 5000;
    const int berry_limit = 50000;
    const int bear_limit = 5000;


    std::vector<std::vector<ChunkWorld>> chunks;

    // Конструктор
    PopulationManager() : chunks(CHUNKS_PER_SIDE_LARGE, std::vector<ChunkWorld>(CHUNKS_PER_SIDE_LARGE)) {}

    // Методы для работы с чанками
    ChunkWorld& getChunk(int worldX, int worldY) {
        int chunkX = coord_to_large_chunkx(worldX);
        int chunkY = coord_to_large_chunky(worldY);
        return chunks[chunkX][chunkY];
    }
    ChunkWorld& getChunkByIndex(int chunkX, int chunkY) {
        return chunks[chunkX][chunkY];
    }

    // Обновление статистики в чанке при добавлении/удалении существа
    void addToChunkWorld(int worldX, int worldY, type_ type) {

        auto& chunk = getChunkByIndex(worldX, worldY);
        switch (type) {
        case type_::rabbit: chunk.rabbit_sum++;rabbit_count++; break;
        case type_::wolf:chunk.wolf_sum++;wolf_count++; break;
        case type_::bear: chunk.bear_sum++;   bear_count++; break;
        case type_::eagle: chunk.eagle_sum++; eagle_count++; break;
        case type_::rat: chunk.rat_sum++;     rat_count++;  break;
        case type_::tree: chunk.tree_sum++;   tree_count++; break;
        case type_::berry: chunk.berry_sum++; berry_count++; break;
        case type_::grass: chunk.grass_sum++; grass_count++; break;
        case type_::bush: chunk.bush_sum++;   bush_count++; break;
        }
    }

    void removeFromChunkWorld(int worldX, int worldY, type_ type) {
        if (worldX < 0 || worldY < 0) return;
        auto& chunk = getChunkByIndex(worldX, worldY);
        switch (type) {
        case type_::rabbit: chunk.rabbit_sum--;rabbit_count--; break;
        case type_::wolf: chunk.wolf_sum--; wolf_count--; break;
        

        case type_::bear: chunk.bear_sum--;  bear_count--;  break;
        case type_::eagle: chunk.eagle_sum--;eagle_count--; break;
        case type_::rat: chunk.rat_sum--;    rat_count--;   break;
        case type_::tree: chunk.tree_sum--;  tree_count--; break;
        case type_::berry: chunk.berry_sum--;berry_count--; break;
        case type_::grass: chunk.grass_sum--;grass_count--; break;
        case type_::bush: chunk.bush_sum--;  bush_count--; break;
        }
    }

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

};

PopulationManager population;
class Chunk;
extern std::vector<std::vector<Chunk>> chunk_grid(
    CHUNKS_PER_SIDEX,
    std::vector<Chunk>(CHUNKS_PER_SIDEY)
);


class Creature : public std::enable_shared_from_this<Creature> {
public:
    float x, y, widht, age_limit, limit, hunger, hunger_limit, maturity_age, eating_range, nutritional_value, nextPositionX, nextPositionY, move_range, step;
    int age;
    int current_chunkWORLD_x = -1;
    int current_chunkWORLD_y = -1;
    int current_chunk_x = -1;
    int current_chunk_y = -1;
    gender_ gender;
    type_ type;
    bool dead = false;
    bool eating = false;
    bool isDirectionSelect = false;
    float birth_tick;
    int berry_count;
    int blossoming_age = 0;
    int berry_limit;
    bool infect = false;
    bool isUsedInfection = false;
    bool isRotten = false;
    int id;
    int cont = 2;
    Creature(type_ t) : type(t) {}
    virtual void process(PopulationManager& pop) = 0;
    

    void removeFromChunk(bool world) {
        if (current_chunk_x < 0 || current_chunk_y < 0) return;
        if (world)
            population.removeFromChunkWorld(current_chunkWORLD_x,
                current_chunkWORLD_y, type);
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
        int chunkWORLD_x = coord_to_large_chunkx(x);
        int chunkWORLD_y = coord_to_large_chunky(y);
        if (new_cx != current_chunk_x || new_cy != current_chunk_y) {
            removeFromChunk(false);  // Удаляем из старого чанка
            // Добавляем в новый чанк
            current_chunk_x = new_cx;
            current_chunk_y = new_cy;

            addToChunk(chunk_grid[new_cx][new_cy], false);
        }
        if (chunkWORLD_x != current_chunkWORLD_x || chunkWORLD_y != current_chunkWORLD_y) {
            population.removeFromChunkWorld(current_chunkWORLD_x,
                current_chunkWORLD_y, type);
            current_chunkWORLD_x = chunkWORLD_x;
            current_chunkWORLD_y = chunkWORLD_y;
            addToChunk(chunk_grid[new_cx][new_cy], true);
        }
    }


    virtual bool shouldDie() const = 0;

protected:
    // Виртуальный метод для получения нужного контейнера в чанке
    virtual std::vector<std::weak_ptr<Creature>>& getChunkContainer(Chunk& chunk, int i) = 0;
    // Виртуальный метод для добавления в чанк (уже объявлен)
    virtual void addToChunk(Chunk& chunk, bool world) = 0;
};
std::vector<std::shared_ptr<Creature>> creature;
std::vector<std::shared_ptr<Creature>> new_creature;
