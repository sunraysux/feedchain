
enum class gender_ { male, female };
enum class type_ { plant, rabbit };

// ������ ������ ����  
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
    // �������� � �������� ������� ��������
    for (int i = 0; i < rabbit.size(); i++)
    {
        rabbit[i].age++;
        rabbit[i].hunger++; // ���������� ������ � �������� �������

        // ������ �� �������� ��� ������
        if (rabbit[i].age > rabbit[i].age_limit || rabbit[i].hunger > rabbit[i].hunger_limit)
        {
            rabbit.erase(rabbit.begin() + i);
            i--; // ������������� ������� ����� ��������
            continue;
        }

        creature* n = &rabbit[i];
        // ������������ �������
        int move_range = 1; // ������������ ���������� �� ���

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
    // ����������� ��������
    for (int i = 0; i < rabbit.size(); i++)
    {
        for (int k = 0; k < rabbit.size(); k++)
        {
            if (k == i)
            {
                continue;
            }
            //��������, ����� �� ������ � �����������
            if (rabbit[i].age >= rabbit[i].maturity_age && rabbit.size() < rabbit[i].limit)
            {
                // ����� �������� ����������

                    // ���� ������� ������ � ���� ����� � �����������
                            // ��������� ������� ��� �����������:
            // 1. ������� ����� � �����������
            // 2. ������� ���������������� ����
            // 3. ������� ��������� ���������� ������ (��������, ���������� < 10)
                if (rabbit[k].age >= rabbit[k].maturity_age &&
                    rabbit[k].gender != rabbit[i].gender &&
                    calculateDistance(rabbit[i].x, rabbit[i].y, rabbit[k].x, rabbit[k].y) < 20.0f)
                {
                    // �������� �������
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


                    // ��������� ����� ��������� ����� ��������� �����������
                    break;
                }

            }


        }
    }

    if (plant.size() == 0)
    {
        return;
    }
    
    // ������� �������� (���� ���� ��������)
    for (int i = 0; i < rabbit.size(); i++)
    {
        if (rabbit[i].hunger>10)
        for (int j = 0; j < plant.size(); j++)
        {
            float distance = sqrt(pow(rabbit[i].x - plant[j].x, 2) +
                pow(rabbit[i].y - plant[j].y, 2));

            // ���� �������� ���������� ������, ������ ��� �������
            if (distance < rabbit[i].eating_range)
            {
                rabbit[i].hunger = (rabbit[i].hunger - plant[j].nutritional_value);
                plant.erase(plant.begin() + j);
                j--; // ������������� ������� ����� ��������
                break; // ������ ����� ������ ������ ���� �������� �� ���
            }
        }
    }
}


void processPlant()
{
    if (plant.size() == 0)
    {
        return;
    }
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
        if ((plant[i].age > plant[i].maturity_age))
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

            if (isGrowingSpace && plant.size() < t.limit && t.x < 50 && t.y < 50 && t.x > -50 && t.y > -50)
            {
                plant.push_back(t);
            }
            if (isGrowingSpace && plant.size() < t.limit && z.x < 50 && z.y < 50 && z.x > -50 && z.y > -50)
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

            if (isGrowingSpace && plant.size() < t.limit && o.x < 50 && o.y < 50 && o.x > -50 && o.y > -50)
            {
                plant.push_back(o);
            }
            if (isGrowingSpace && plant.size() < t.limit && v.x < 50 && v.y < 50 && v.x > -50 && v.y > -50)
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
        t.limit = 1000;
        t.nutritional_value = 100;
        t.widht = 1;  
        t.age = rand() % 10050;
        t.maturity_age = 1115;
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
        ConstBuf::global[i] = XMFLOAT4(rabbit[i].x, rabbit[i].y, rabbit[i].x + t, rabbit[i].y + t);
        

    }
    ConstBuf::Update(5, ConstBuf::global);
    ConstBuf::ConstToVertex(5);
    Draw::NullDrawer(1, rabbit.size());

    context->PSSetShaderResources(0, 1, &Textures::Texture[1].TextureResView);

    for (int i = 0;i < plant.size();i++)
    {
        float t = plant[i].age;
        t = t / 100;
        ConstBuf::global[i] = XMFLOAT4(plant[i].x, plant[i].y, plant[i].x + t, plant[i].y + t);
        

    }
    ConstBuf::Update(5, ConstBuf::global);
    ConstBuf::ConstToVertex(5);
    Draw::NullDrawer(1, plant.size());

}





