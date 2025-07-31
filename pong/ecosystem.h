#include <algorithm>
enum class gender_ { male, female };
enum class type_ { plant, rabbit };

// секция данных игры  
struct creature {
    float x, y, widht, age_limit, limit, hunger, hunger_limit, maturity_age, eating_range, nutritional_value;

    int age;
    gender_ gender;
    type_ type;


};

std::vector<creature>plant;
std::vector<creature>rabbit;
void show()
{

    ConstBuf::Update(5, ConstBuf::global);
    ConstBuf::ConstToVertex(5);
    Draw::NullDrawer(1, plant.size());


}
void showRabbit()
{

    ConstBuf::Update(5, ConstBuf::global);
    ConstBuf::ConstToVertex(5);
    Draw::NullDrawer(1, rabbit.size());
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

        if (rabbit[i].x > 50)
        {
            n->x -= move_range;
        }

        if (rabbit[i].y > 50)
        {
            n->y -= move_range;
        }

        if (rabbit[i].x < -50)
        {
            n->x += move_range;
        }

        if (rabbit[i].y < -50)
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

    if (plant.size() == 0)
    {
        return;
    }
    
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
    if (plant.empty()) return;

    // 1. Старение и смерть растений
    for (int i = 0; i < plant.size(); ) {
        plant[i].age++;

        // Случайная смерть даже до age_limit (как в природе)
        if (plant[i].age > plant[i].age_limit || (rand() % 1000 < 2)) { // 0.2% шанс смерти
            plant.erase(plant.begin() + i);
        }
        else {
            i++;
        }
    }

    // 2. Размножение выживших растений
    std::vector<creature> newPlants;

    for (auto& p : plant) {
        if (p.age < p.maturity_age) continue;

        // Чем старше растение, тем больше потомства (но не линейно)
        float reproductionChance = min(0.1f * (p.age - p.maturity_age), 0.5f); (0.1f * (p.age - p.maturity_age), 0.5f);

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
                seedling.age_limit += rand() % 100 - 50;
                seedling.maturity_age += rand() % 50 - 25;
                seedling.nutritional_value += rand() % 10 - 5;

                // Проверка границ и пересечений
                if (seedling.x < -50 || seedling.x > 50 ||
                    seedling.y < -50 || seedling.y > 50) {
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


#include <random> 

void InitGame() {
    std::random_device rd; 
    std::mt19937 gen(rd());
    Textures::LoadTextureFromFile(1, L"Debug/plant.png");
    Textures::LoadTextureFromFile(2, L"Debug/animal.png");
    int base_range_x = 50;
    int base_range_y = 50;
    std::uniform_int_distribution<> x(-base_range_x / 2, base_range_x / 2);
    std::uniform_int_distribution<> y(-base_range_y / 2, base_range_y / 2);

    int group_spread = window.width / 2;
    std::uniform_int_distribution<> dist_spread(-group_spread, group_spread);


    for (int i = 0; i < 20; i++) {
        creature t;
        t.x = x(gen);
        t.y = y(gen);
        t.limit = 1000;
        t.nutritional_value = 100;
        t.widht = 1;  
        t.age = rand() % 10050;
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
        n.limit = 500;
        n.hunger_limit = 50;
        n.hunger = 0;
        rabbit.push_back(n);
    }
}



void ShowRacketAndBall()
{
    context->PSSetShaderResources(0, 1, &Textures::Texture[2].TextureResView);
    for (int i = 0;i < rabbit.size();i++)
    {
        float t = rabbit[i].age;
        t = t / 100;
        ConstBuf::global[i] = XMFLOAT4(rabbit[i].x-t, rabbit[i].y, rabbit[i].x + t, rabbit[i].y + t);
        

    }
    ConstBuf::Update(5, ConstBuf::global);
    ConstBuf::ConstToVertex(5);
    Draw::NullDrawer(1, rabbit.size());

    context->PSSetShaderResources(0, 1, &Textures::Texture[1].TextureResView);

    for (int i = 0;i < plant.size();i++)
    {
        float t = plant[i].age;
        t = t / 100;
        ConstBuf::global[i] = XMFLOAT4(plant[i].x-t, plant[i].y, plant[i].x + t, plant[i].y + t);
        

    }
    ConstBuf::Update(5, ConstBuf::global);
    ConstBuf::ConstToVertex(5);
    Draw::NullDrawer(1, plant.size());

}





