

#include "windows.h"
#include <vector>
#include "math.h"


enum class gender_ { male, female };


// секция данных игры  
struct creature {
    float x, y, size, widht, speed, reproduction_rate, growth_rate, nutritional_value, velocity, prevalence, age_limit, hunger, hunger_limit, maturity_age, eating_range, plant_limit;

    int age;
    int breeding_period;

    gender_ gender;
    int dna_checksum;


    void show()
    {
        float t = age;
        t = t / 10;
        ConstBuf::global[0] = XMFLOAT4(x - t / 10, y, widht + t / 5, t); // x=100, y=50, size=2);
        ConstBuf::Update(5, ConstBuf::global);
        ConstBuf::ConstToVertex(5);
        Draw::NullDrawer(1, 1);


    }
    void showRabbit()
    {
        float amp = age;
    }

};

std::vector<creature>plant;


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
            t.plant_limit = 500;
            int amp = 200;
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
                if (distance < 2)
                {
                    isGrowingSpace = false;
                }

            }

            if (isGrowingSpace && plant.size() < t.plant_limit && t.x < 100 && t.y < 100 && t.x > -100 && t.y > -100)
            {
                plant.push_back(t);
            }
            if (isGrowingSpace && plant.size() < t.plant_limit && z.x < 100 && z.y < 100 && z.x > -100 && z.y > -100)
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
                if (distance < 2)
                {
                    isGrowingSpace = false;
                }

            }

            if (isGrowingSpace && plant.size() < t.plant_limit && o.x < 100 && o.y < 100 && o.x > -100 && o.y > -100)
            {
                plant.push_back(o);
            }
            if (isGrowingSpace && plant.size() < t.plant_limit && v.x < 100 && v.y < 100 && v.x > -100 && v.y > -100)
            {
                plant.push_back(v);
            }

        }
    }

}
std::vector<creature>rabbit;

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
        n->x += rand() % (move_range * 2) - move_range;
        n->y += rand() % (move_range * 2) - move_range;

    }
    // Размножение кроликов
   // for (int i = 0; i < rabbit.size(); i++)
   // {

         //Проверка, готов ли кролик к размножению
      // float maturity_age = 234;
      // if (rabbit[i].age >= rabbit[i].maturity_age)
        //{
            // Поиск партнера поблизости


                // Если партнер найден и тоже готов к размножению
               // if 
               // {
                    // Создание потомка
                   // Rabbit offspring;
                   // offspring = rabbits[i]; // Копируем характеристики

                    // Добавляем потомка в популяцию
                   // rabbit.push_back(offspring);

                    // Прерываем поиск партнеров после успешного размножения
                   // break;
              //  }
           // }
        //}
   // }
//}

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



#include <random> // Для std::random_device, std::mt19937, std::uniform_int_distribution

void InitGame() {
    std::random_device rd;  // Источник энтропии
    std::mt19937 gen(rd()); // Генератор Mersenne Twister

    // Диапазон для центров групп (в 100 раз больше экрана)
    int base_range_x = 50;
    int base_range_y = 50;
    std::uniform_int_distribution<> x(-base_range_x / 2, base_range_x / 2);
    std::uniform_int_distribution<> y(-base_range_y / 2, base_range_y / 2);

    // Разброс внутри группы (например, пол-экрана)
    int group_spread = window.width / 2;
    std::uniform_int_distribution<> dist_spread(-group_spread, group_spread);


    for (int i = 0; i < 20; i++) {
        creature t;
        t.x = x(gen);
        t.y = y(gen);
        t.plant_limit = 60;
        t.size = 60;
        t.widht = 1;  // Исправлено typo (было widht)
        t.age = rand() % 50;
        t.breeding_period = 115;
        t.age_limit = 117;
        plant.push_back(t);
    }

    // for (int i = 0; i < 5; i++)
    // {
    //     creature n;
    //     n.eating_range = 1;
    //     
    //     n.x = x + rand() % size;
    //     n.y = y + rand() % size;
    //     n.size = 50;
    //     n.age = rand() % 5;
    //     n.breeding_period = 700;
    //     n.age_limit = 2000;
    //     rabbit.push_back(n);
    // }








  //------------------------------------------------------



}



void ShowRacketAndBall()
{

    for (int i = 0;i < plant.size();i++)
    {
        plant[i].show();

    }

    for (int i = 0; i < rabbit.size(); i++)
    {
        rabbit[i].showRabbit();
    }


}





