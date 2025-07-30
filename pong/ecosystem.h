
enum class gender_ { male, female };
enum class type_ { plant, rabbit };

// секция данных игры  
struct creature {
    float x, y, widht, age_limit, limit, hunger, hunger_limit, maturity_age, eating_range, nutritional_value;

    int age;
    int breeding_period;
    gender_ gender;
    type_ type;

    void show()
    {
        float t = age;
        t = t / 10;
        ConstBuf::global[0] = XMFLOAT4(x - t , y, x + t , y+t); // x=100, y=50, size=2);

        ConstBuf::Update(5, ConstBuf::global);
        ConstBuf::ConstToVertex(5);
        Draw::NullDrawer(1, 1);


    }
    void showRabbit()
    {
        float t = age;
        t = t/10;
        ConstBuf::global[0] = XMFLOAT4(x , y, x+t, y+t); // x=100, y=50, size=2);
       
        ConstBuf::Update(5, ConstBuf::global);
        ConstBuf::ConstToVertex(5);
        Draw::NullDrawer(1, 1);
    }

};

std::vector<creature>plant;
std::vector<creature>rabbit;

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
        int move_range = 10; // Максимальное расстояние за ход

        n->x += (rand() % (move_range * 2 + 1)) - move_range;
        n->y += (rand() % (move_range * 2 + 1)) - move_range;

        if (rabbit[i].x > 100)
        {
            n->x -= move_range;
        }

        if (rabbit[i].y > 100)
        {
            n->y -= move_range;
        }

        if (rabbit[i].x < -100)
        {
            n->x += move_range;
        }

        if (rabbit[i].y < -100)
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
            if (rabbit[i].age >= rabbit[i].breeding_period && rabbit.size() < rabbit[i].limit)
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


    // Питание кроликов (если есть растения)
    for (int i = 0; i < rabbit.size(); i++)
    {
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


void processPlant()
{

    for (int i = 0;i < plant.size();i++)
    {
        plant[i].age++;

        if (plant[i].age > plant[i].age_limit)
        {
            plant.erase(plant.begin() + i);
            i--;
        }
    }

    for (int i = 0;i < plant.size() - 1;i++)
    {
        if ((plant[i].age > plant[i].breeding_period))
        {
            creature t;
            creature z;
            t = plant[i];
            z = plant[i + 1];
            int amp = 100;
            t.x += rand() % amp - amp / 2;
            t.y += rand() % amp - amp / 2;
            t.age = 0;
            z.x += t.x + 1;
            z.y += t.y + 1;
            z.age = 0;
            bool isGrowingSpace = true;
            for (int j = 0;j < plant.size();j++)
            {
                float distance = sqrt(pow(t.x - plant[j].x, 2) + pow(t.y - plant[j].y, 2));
                if (distance < 1)
                {
                    isGrowingSpace = false;
                }

            }

            if (isGrowingSpace && plant.size() < t.limit && t.x < 100 && t.y < 100 && t.x > -100 && t.y > -100)
            {
                plant.push_back(t);
            }
            if (isGrowingSpace && plant.size() < t.limit && z.x < 100 && z.y < 100 && z.x > -100 && z.y > -100)
            {
                plant.push_back(z);
            }
            creature o;
            creature v;
            o = plant[i];
            v = plant[i + 1];
            o.x += rand() % amp - amp / 2;
            o.y += rand() % amp - amp / 2;
            o.age = 0;
            v.x += t.x + 1;
            v.y += t.y + 1;
            v.age = 0;
            isGrowingSpace = true;
            for (int j = 0;j < plant.size();j++)
            {
                float distance = sqrt(pow(t.x - plant[j].x, 2) + pow(t.y - plant[j].y, 2));
                if (distance < 1)
                {
                    isGrowingSpace = false;
                }

            }

            if (isGrowingSpace && plant.size() < t.limit && o.x < 100 && o.y < 100 && o.x > -100 && o.y > -100)
            {
                plant.push_back(o);
            }
            if (isGrowingSpace && plant.size() < t.limit && v.x < 100 && v.y < 100 && v.x > -100 && v.y > -100)
            {
                plant.push_back(v);
            }

        }
    }

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
        t.limit = 600;
        t.nutritional_value = 100;
        t.widht = 1;  
        t.age = rand() % 150;
        t.breeding_period = 115;
        t.age_limit = 117;
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
        n.eating_range = 1;
        n.x = 0;
        n.y = 0;
        n.age = rand() % 5;
        n.breeding_period = 50;
        n.age_limit = 200;
        n.limit = 100;
        n.hunger_limit = 100;
        n.hunger = 0;
        rabbit.push_back(n);
    }
}



void ShowRacketAndBall()
{
    context->PSSetShaderResources(0, 1, &Textures::Texture[1].TextureResView);

    for (int i = 0;i < plant.size();i++)
    {

        plant[i].show();

    }
    context->PSSetShaderResources(0, 1, &Textures::Texture[2].TextureResView);
    for (int i = 0;i < rabbit.size();i++)
    {

        rabbit[i].showRabbit();

    }
}





