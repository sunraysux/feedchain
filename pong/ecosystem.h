#include <algorithm>
#include <random> 
enum class gender_ { male, female };
enum class type_ { plant, rabbit };
int base_range = 50;

const int CHUNK_SIZE = base_range*2/10; // Размер чанка
const int GRID_SIZE = base_range*2; // Размер игрового поля
const int CHUNKS_PER_SIDE = GRID_SIZE / CHUNK_SIZE;
// секция данных игры  

struct creature {
    float x, y, widht, age_limit, limit, hunger, hunger_limit, maturity_age, eating_range, nutritional_value;

    int age;
    gender_ gender;
    type_ type;


};

std::vector<creature> plant;
std::vector<creature> rabbit;

struct Chunk {
    std::vector<creature*> plants;   // Указатели на растения в чанке
    std::vector<creature*> rabbits;  // Указатели на кроликов в чанке
};

std::vector<std::vector<Chunk>> chunk_grid(
    CHUNKS_PER_SIDE,
    std::vector<Chunk>(CHUNKS_PER_SIDE)
);


inline int coord_to_chunk(float coord) {
    // Смещаем координату из [-50,50] в [0,100]
    float normalized = coord + 50.0f;
    // Вычисляем индекс и ограничиваем его
    int index = static_cast<int>(normalized / CHUNK_SIZE);
    return clamp(index, 0, CHUNKS_PER_SIDE - 1);
}

void updatechunks() {
    // Очищаем предыдущие данные
    for (auto& row : chunk_grid) {
        for (auto& chunk : row) {
            chunk.plants.clear();
            chunk.rabbits.clear();
        }
    }

    // Распределяем растения
    for (auto& p : plant) {
        int cx = coord_to_chunk(p.x);
        int cy = coord_to_chunk(p.y);
        chunk_grid[cx][cy].plants.push_back(&p);
    }

    // Распределяем кроликов
    for (auto& r : rabbit) {
        int cx = coord_to_chunk(r.x);
        int cy = coord_to_chunk(r.y);
        chunk_grid[cx][cy].rabbits.push_back(&r);
    }
}

float calculateDistance(float x1, float y1, float x2, float y2) {
    // Calculate the difference in x-coordinates
    float deltaX = x2 - x1;
    // Calculate the difference in y-coordinates
    float deltaY = y2 - y1;

    // Apply the distance formula
    float distance = std::sqrt(std::pow(deltaX, 2) + std::pow(deltaY, 2));
    return distance;
}

void processRabbit()
{
    // Старение и удаление умерших кроликов
    for (int i = 0; i < rabbit.size(); i++)
    {
        rabbit[i].age++;
        rabbit[i].hunger++; // Увеличение голода с течением времени

        // Смерть от старости или голода
        if (rabbit[i].age > rabbit[i].age_limit || rabbit[i].hunger > rabbit[i].hunger_limit)
        {
            rabbit.erase(rabbit.begin() + i);
            i--; // Корректировка индекса после удаления
            continue;
        }

        creature* n = &rabbit[i];
        // Передвижение кролика
        int move_range = 1; // Максимальное расстояние за ход

        n->x += (rand() % (move_range * 2 + 1)) - move_range;
        n->y += (rand() % (move_range * 2 + 1)) - move_range;

        if (rabbit[i].x > base_range)
        {
            n->x -= move_range;
        }

        if (rabbit[i].y > base_range)
        {
            n->y -= move_range;
        }

        if (rabbit[i].x < -base_range)
        {
            n->x += move_range;
        }

        if (rabbit[i].y < -base_range)
        {
            n->y += move_range;

        }
    }
    // Размножение кроликов
    for (int i = 0; i < rabbit.size(); i++)
    {
        for (int k = 0; k < rabbit.size(); k++)
        {
            if (k == i)
            {
                continue;
            }
            //Проверка, готов ли кролик к размножению
            if (rabbit[i].age >= rabbit[i].maturity_age && rabbit.size() < rabbit[i].limit)
            {
                // Поиск партнера поблизости

                    // Если партнер найден и тоже готов к размножению
                            // Проверяем условия для размножения:
            // 1. Партнер готов к размножению
            // 2. Партнер противоположного пола
            // 3. Партнер находится достаточно близко (например, расстояние < 10)
                if (rabbit[k].age >= rabbit[k].maturity_age &&
                    rabbit[k].gender != rabbit[i].gender &&
                    calculateDistance(rabbit[i].x, rabbit[i].y, rabbit[k].x, rabbit[k].y) < 20.0f)
                {
                    // Создание потомка
                    creature n;
                    n = rabbit[i];
                    int amp = 10;
                    n.x += rand() % amp - amp / 2;
                    n.y += rand() % amp - amp / 2;
                    n.hunger = 0;
                    n.age = 0;
                    n.gender = (rand() % 2 == 0) ? gender_::male : gender_::female;
                    if (n.gender == gender_::male)
                    {
                        n.type = type_::rabbit;
                    }
                    else
                    {
                        n.type = type_::rabbit;
                    }
                    rabbit.push_back(n);


                    // Прерываем поиск партнеров после успешного размножения
                    break;
                }

            }


        }
    }

    if (plant.empty()) return;
    
    // Питание кроликов (если есть растения)
    for (int i = 0; i < rabbit.size(); i++)
    {
        if (rabbit[i].hunger>10)
        for (int j = 0; j < plant.size(); j++)
        {
            float distance = sqrt(pow(rabbit[i].x - plant[j].x, 2) +
                pow(rabbit[i].y - plant[j].y, 2));

            // Если растение достаточно близко, кролик его съедает
            if (distance < rabbit[i].eating_range)
            {
                rabbit[i].hunger = (rabbit[i].hunger - plant[j].nutritional_value);
                plant.erase(plant.begin() + j);
                j--; // Корректировка индекса после удаления
                break; // Кролик может съесть только одно растение за ход
            }
        }
    }
}


void processPlant() {
    //if (plant.empty()) return;

    // 1. Старение и смерть растений
    for (int i = 0; i < plant.size(); ) {
        plant[i].age++;

        // Случайная смерть даже до age_limit (как в природе)
        if (plant[i].age > plant[i].age_limit || (rand() % 1000 < 5)) { // 0.2% шанс смерти
            plant.erase(plant.begin() + i);
        }
        else {
            i++;
        }
    }
    
    // 2. Размножение 
    std::vector<creature> newPlants;

    for (auto& p : plant) {
        if (p.age < p.maturity_age) continue;

        // Чем старше растение, тем больше потомства 
        float reproductionChance = min(0.01f * (p.age - p.maturity_age), 0.05f); 

        if ((rand() % 100) < (reproductionChance * 100) && plant.size() + newPlants.size() < p.limit) {
            // Количество семян (1-5)
            int seeds = 1 + rand() % 5;

            for (int s = 0; s < seeds; s++) {
                creature seedling = p;
                seedling.age = 0;

                // Распределение семян вокруг материнского растения
                float distance = 3.0f + (rand() % 10); // 3-12 единиц
                float angle = (rand() % 360) * 3.14159f / 180.0f;

                seedling.x += distance * cos(angle);
                seedling.y += distance * sin(angle);

                // Генетические вариации
                //seedling.age_limit += rand() % 100 - 50;
                //seedling.maturity_age += rand() % 50 - 25;
                //seedling.nutritional_value += rand() % 10 - 5;

                // Проверка границ и пересечений
                if (seedling.x < -base_range || seedling.x > base_range ||
                    seedling.y < -base_range || seedling.y > base_range) {
                    continue;
                }

                bool canGrow = true;
                for (const auto& other : plant) {
                    float dist = sqrt(pow(seedling.x - other.x, 2) +
                        pow(seedling.y - other.y, 2));
                    if (dist < 2.0f) { // Минимальное расстояние между растениями
                        canGrow = false;
                        break;
                    }
                }

                if (canGrow) {
                    newPlants.push_back(seedling);
                }
            }
        }
    }

    // Добавление новых растений
    plant.insert(plant.end(), newPlants.begin(), newPlants.end());
}




void InitGame() {
    std::random_device rd; 
    std::mt19937 gen(rd());
    Textures::LoadTextureFromFile(1, L"Debug/plant.png");
    Textures::LoadTextureFromFile(2, L"Debug/animal.png");
    
    std::uniform_int_distribution<> x(-base_range / 2, base_range / 2);
    std::uniform_int_distribution<> y(-base_range / 2, base_range / 2);

    int group_spread = window.width / 2;
    std::uniform_int_distribution<> dist_spread(-group_spread, group_spread);


    for (int i = 0; i < 1000; i++) {
        creature t;
        t.x = x(gen);
        t.y = y(gen);
        t.limit = 1000;
        t.nutritional_value = 100;
        t.widht = 1;  
        t.age = rand() % 1050;
        t.maturity_age = 115;
        t.age_limit = 11700;
        plant.push_back(t);
    }
    for (int i = 0; i < 5; i++)
    {
        creature n;
        n.gender = (rand() % 2 == 0) ? gender_::male : gender_::female;
        if (n.gender == gender_::male)
        {
            n.type = type_::rabbit;
        }
        else
        {
            n.type = type_::rabbit;
        }
        n.eating_range = 2;
        n.x = 0;
        n.y = 0;
        n.age = rand() % 50;
        n.maturity_age = 100;
        n.age_limit = 200;
        n.limit = 550;
        n.hunger_limit = 50;
        n.hunger = 0;
        rabbit.push_back(n);
    }
}

void Showpopulations()
{
    Shaders::vShader(2);
    Shaders::pShader(2);
    ConstBuf::global[0] = XMFLOAT4(rabbit.size(), plant.size(), 0,0);

    ConstBuf::Update(5, ConstBuf::global);
    ConstBuf::ConstToVertex(5);
    Draw::NullDrawer12(1,2);
}

void ShowRacketAndBall()
{
    // Отрисовка кроликов по чанкам
    context->PSSetShaderResources(0, 1, &Textures::Texture[2].TextureResView);
    int rabbitCount = 0;
    for (int cx = 0; cx < CHUNKS_PER_SIDE; ++cx) {
        for (int cy = 0; cy < CHUNKS_PER_SIDE; ++cy) {
            const auto& chunk = chunk_grid[cx][cy];
            for (const auto& r : chunk.rabbits) {
                float t = r->age / 10.0f;
                ConstBuf::global[rabbitCount] = XMFLOAT4(r->x - t / 1.2f, r->y, r->x + t, r->y + t);
                rabbitCount++;
            }
        }
    }
    if (rabbitCount > 0) {
        ConstBuf::Update(5, ConstBuf::global);
        ConstBuf::ConstToVertex(5);
        Draw::NullDrawer(1, rabbitCount);
    }

    // Отрисовка растений по чанкам
    context->PSSetShaderResources(0, 1, &Textures::Texture[1].TextureResView);
    int plantCount = 0;
    for (int cy = CHUNKS_PER_SIDE - 1; cy >= 0; --cy) {  // Перебор по Y в обратном порядке
        for (int cx = 0; cx < CHUNKS_PER_SIDE; ++cx) {
            const auto& chunk = chunk_grid[cx][cy];
            for (const auto& p : chunk.plants) {
                float t = p->age / 10.0f;
                ConstBuf::global[plantCount] = XMFLOAT4(p->x - t / 1.2f, p->y, p->x + t, p->y + t);
                plantCount++;
            }
        }
    }
    if (plantCount > 0) {
        ConstBuf::Update(5, ConstBuf::global);
        ConstBuf::ConstToVertex(5);
        Draw::NullDrawer(1, plantCount);
    }
}





