void ProcessCreatures(PopulationManager& pop) {
    tick++;
    int dead_creature = 0;

    // Обработать всех существ
    for (auto& cr : creature) {
        cr->process(pop);
    }

    // Удалить мертвых существ напрямую
    creature.erase(
        std::remove_if(creature.begin(), creature.end(),
            [&](const auto& entity) {
                if (entity->shouldDie()) {
                    dead_creature++;
                    entity->removeFromChunk(true);
                    return true;
                }
                return false;
            }
        ),
        creature.end()
    );

    // Добавить новых существ
    creature.reserve(creature.size() + new_creature.size());
    for (auto& entity : new_creature) {
        entity->updateChunk();
        creature.emplace_back(std::move(entity));
    }
    new_creature.clear();
}
//инициализация игры
void InitGame() {
    //std::random_device rd;
    //std::mt19937 gen(rd()); // генератор
    //std::uniform_real_distribution<float> plant_dist(-base_rangex , base_rangey );



    Textures::LoadTextureFromFile(2, L"Debug/animal.png");
 //   Textures::CreateDepthForTexture(2);
    Textures::LoadTextureFromFile(3, L"Debug/wolf.png");
  //  Textures::CreateDepthForTexture(3);
    Textures::LoadTextureFromFile(4, L"Debug/grass.jpg");
  //  Textures::CreateDepthForTexture(4);
    Textures::LoadTextureFromFile(5, L"Debug/grass2.jpg");
  //  Textures::CreateDepthForTexture(5);
    Textures::LoadTextureFromFile(6, L"Debug/grass3.jpg");
  //  Textures::CreateDepthForTexture(6);
    Textures::LoadTextureFromFile(7, L"Debug/smallBush.png");
   // Textures::CreateDepthForTexture(7);
    Textures::LoadTextureFromFile(8, L"Debug/eagleMale.png");
   // Textures::CreateDepthForTexture(8);
    Textures::LoadTextureFromFile(9, L"Debug/smallTree.png");
  //  Textures::CreateDepthForTexture(9);
   // Textures::LoadTextureFromFile(10, L"Debug/i.jpg");
   // Textures::ReadTextureToCPU(10);
   // Textures::CreateDepthForTexture(10);
    Textures::LoadTextureFromFile(11, L"Debug/standartTree.png");
    Textures::LoadTextureFromFile(12, L"Debug/bigTree.png");
    Textures::LoadTextureFromFile(13, L"Debug/standartBush.png");
    Textures::LoadTextureFromFile(14, L"Debug/bigBush.png");
    Textures::LoadTextureFromFile(15, L"Debug/rat.png");
    Textures::LoadTextureFromFile(16, L"Debug/eagleFemale.png");
    Textures::LoadTextureFromFile(17, L"Debug/infectRat.png");
    Textures::LoadTextureFromFile(18, L"Debug/lightning.png");
    Textures::LoadTextureFromFile(19, L"Debug/трава1.png");
    Textures::LoadTextureFromFile(20, L"Debug/трава2.png");
    Textures::LoadTextureFromFile(21, L"Debug/трава3.png");
    Textures::LoadTextureFromFile(27, L"Debug/Ягода.png");
    Textures::LoadTextureFromFile(28, L"Debug/bear.png");
    Textures::LoadTextureFromFile(40, L"Debug/statBTN.png");
    Textures::LoadTextureFromFile(41, L"Debug/typeBar1.png");
    Textures::LoadTextureFromFile(42, L"Debug/speedBar1.png");
    Textures::LoadTextureFromFile(47, L"Debug/statistik.png");
    Textures::LoadTextureFromFile(48, L"Debug/speedBar2.png");
    Textures::LoadTextureFromFile(49, L"Debug/speedBar3.png");
    Textures::LoadTextureFromFile(50, L"Debug/speedBar4.png");
    Textures::LoadTextureFromFile(51, L"Debug/typeBar2.png");
    Textures::LoadTextureFromFile(52, L"Debug/typeBar3.png");
    Textures::LoadTextureFromFile(53, L"Debug/typeBar4.png");
    Textures::LoadTextureFromFile(54, L"Debug/statBTNf.png");
    Textures::LoadTextureFromFile(100, L"Debug/0.png");
    Textures::LoadTextureFromFile(101, L"Debug/1.png");
    Textures::LoadTextureFromFile(102, L"Debug/2.png");
    Textures::LoadTextureFromFile(103, L"Debug/3.png");
    Textures::LoadTextureFromFile(104, L"Debug/4.png");
    Textures::LoadTextureFromFile(105, L"Debug/5.png");
    Textures::LoadTextureFromFile(106, L"Debug/6.png");
    Textures::LoadTextureFromFile(107, L"Debug/7.png");
    Textures::LoadTextureFromFile(108, L"Debug/8.png");
    Textures::LoadTextureFromFile(109, L"Debug/9.png");
    // Начальные растения
   //for (int i = 0; i < 1000; i++) {
   //    auto tree = std::make_shared<Tree>();
   //    tree-> y = Random::Int(-base_rangey, base_rangey);
   //    tree-> x = Random::Int(-base_rangex, base_rangex);
   //    tree->age = 0;
   //    tree->updateChunk();
   //    trees.push_back(tree);
   //    population.tree_count++;
   //}
   // for (int i = 0; i < 0; i++) {
   //     auto bush = std::make_shared<Bush>();
   //     bush-> y = Random::Int(-100, 100);
   //     bush-> x = Random::Int(-100, 100);
   //     bush->age = Random::Int(0, 1000);
   //     bush->updateChunk();
   //     bushes.push_back(bush);
   //     population.bush_count++;
   // }
   //// Начальные кролики
   //for (int i = 0; i < 100; i++) {
   //    auto rabbit = std::make_shared<Rabbit>();
   //    rabbit->y = Random::Int(-base_rangey, base_rangey);
   //    rabbit->x = Random::Int(-base_rangex, base_rangex);
   //    rabbit->hunger = Random::Int(0, 100);
   //    rabbit->age = Random::Int(0, 100);
   //    rabbits.push_back(rabbit);
   //    population.rabbit_count++;
   //}
   //for (int i = 0; i < 0; i++) {
   //    auto wolf = std::make_shared<Wolf>();
   //    wolf->y = Random::Int(-100, 100);
   //    wolf->x = Random::Int(-100, 100);
   //    wolf->hunger = Random::Int(0, 500);
   //    wolf->age = Random::Int(0, 500);
   //    wolves.push_back(wolf);
   //    population.wolf_count++;
   //}

   //for (int i = 0; i < 0; i++) {
   //    auto eagle = std::make_shared<Eagle>();
   //    eagle->y = Random::Int(-100, 100);
   //    eagle->x = Random::Int(-100, 100);
   //    eagle->hunger = Random::Int(0, 500);
   //    eagle->age = Random::Int(0, 500);
   //    eagles.push_back(eagle);
   //    population.eagle_count++;
   //}
   //for (int i = 0; i < 0; i++) {
   //    auto rat = std::make_shared<Rat>();
   //    rat->y = Random::Int(-100, 100);
   //    rat->x = Random::Int(-100, 100);
   //    rat->hunger = Random::Int(0, 100);
   //    rat->age = Random::Int(0, 100);
   //    rats.push_back(rat);
   //    population.rat_count++;
   //}
   //for (int i = 0; i < 0; i++) {
   //    auto gras = std::make_shared<Grass>();
   //    gras->y = Random::Int(-base_rangey, base_rangey);
   //    gras->x = Random::Int(-base_rangex, base_rangex);
   //    gras->age = 0;
   //    gras->updateChunk();
   //    grass.push_back(gras);
   //    population.grass_count++;
   //}
}

type_ currentType = type_::wolf; // по умолчанию волк



auto isVisible = [&](float x, float y) -> bool {
    // Преобразуем мировые координаты объекта в однородные координаты
    XMVECTOR objectPos = XMVectorSet(x, y, 0, 1.0f);

    // Применяем матрицу вида
    XMVECTOR viewSpacePos = XMVector3Transform(objectPos, Camera::state.viewMatrix);

    // Применяем матрицу проекции
    XMVECTOR clipSpacePos = XMVector3Transform(viewSpacePos, Camera::state.projMatrix);

    // Перспективное деление
    float w = XMVectorGetW(clipSpacePos);
    if (fabs(w) < 0.0001f) return false; // Избегаем деления на ноль

    float ndcX = XMVectorGetX(clipSpacePos) / w;
    float ndcY = XMVectorGetY(clipSpacePos) / w;
    float ndcZ = XMVectorGetZ(clipSpacePos) / w;

    // Проверяем, находится ли точка в пределах видимой области
    return (ndcX >= -1.0f && ndcX <= 1.0f &&
        ndcY >= -1.0f && ndcY <= 1.0f &&
        ndcZ >= 0.0f && ndcZ <= 1.0f);
    };
int BATCH_SIZE = 4000;

void DrawBatchedInstances(int textureIndex, const std::vector<XMFLOAT4>& instances) {
    if (instances.empty()) return;

    context->PSSetShaderResources(0, 1, &Textures::Texture[textureIndex].TextureResView);
    for (size_t start = 0; start < instances.size(); start += BATCH_SIZE) {
        size_t count = min(BATCH_SIZE, static_cast<int>(instances.size() - start));

        const int OFFSET = 8;
        std::copy(instances.begin() + start, instances.begin() + start + count, ConstBuf::global + OFFSET);

        ConstBuf::global[0] = XMFLOAT4(Camera::state.camXChunk, Camera::state.camYChunk, 0, 0);

        ConstBuf::Update(5, ConstBuf::global);
        ConstBuf::ConstToVertex(5);
        Textures::TextureToShader(1, 0, vertex);
        Draw::NullDrawer(1, static_cast<int>(count));
    }
}

void ShowRacketAndBallFromVectors()
{
    Shaders::vShader(0);
    Shaders::pShader(0);

    // Группировка существ по типам
    std::vector<XMFLOAT4> wolves;
    std::vector<XMFLOAT4> rabbits;
    std::vector<XMFLOAT4> bears;
    std::vector<XMFLOAT4> maleEagles;
    std::vector<XMFLOAT4> femaleEagles;
    std::vector<XMFLOAT4> smallTrees;
    std::vector<XMFLOAT4> standardTrees;
    std::vector<XMFLOAT4> bigTrees;
    std::vector<XMFLOAT4> smallBushes;
    std::vector<XMFLOAT4> standardBushes;
    std::vector<XMFLOAT4> bigBushes;
    std::vector<XMFLOAT4> berrys;
    std::vector<XMFLOAT4> smallGrass;
    std::vector<XMFLOAT4> standardGrass;
    std::vector<XMFLOAT4> bigGrass;
    std::vector<XMFLOAT4> infectedRats;
    std::vector<XMFLOAT4> healthyRats;

    // Предварительное резервирование памяти (оптимизация)
    size_t totalCreatures = creature.size();
    size_t estimatedPerType = totalCreatures / 10; // примерная оценка

    wolves.reserve(estimatedPerType);
    rabbits.reserve(estimatedPerType);

    // Обработка всех существ в одном цикле
    for (const auto& cr : creature) {
        if (!cr) continue;

        switch (cr->type) {
        case type_::wolf: {
            float s = max(cr->age / SIZEWOLFS, 10.0f);
            wolves.emplace_back(cr->x, cr->y, s, 1.0f);
            break;
        }
        case type_::rabbit: {
            float s = max(cr->age / SIZERABBITS, 10.0f);
            rabbits.emplace_back(cr->x, cr->y, s, 1.0f);
            break;
        }
        case type_::bear: {
            float s = max(cr->age / SIZEBEARS, 10.0f);
            bears.emplace_back(cr->x, cr->y, s, 1.0f);
            break;
        }
        case type_::eagle: {
            float s = max(cr->age / SIZEAGLES, 10.0f);
            if (cr->gender == gender_::male) {
                maleEagles.emplace_back(cr->x, cr->y, s, 2.0f);
            }
            else {
                femaleEagles.emplace_back(cr->x, cr->y, s, 2.0f);
            }
            break;
        }
        case type_::tree: {
            float as = cr->age / SIZETREES;
            if (cr->age > cr->age_limit / 2) {
                bigTrees.emplace_back(cr->x, cr->y, as, 2.0f);
            }
            else if (cr->age > cr->age_limit / 3) {
                standardTrees.emplace_back(cr->x, cr->y, as, 2.0f);
            }
            else {
                smallTrees.emplace_back(cr->x, cr->y, max(as, 1.0f), 2.0f);
            }
            break;
        }
        case type_::bush: {
            float as = cr->age / SIZEBUSHES;
            if (cr->age > 500) {
                bigBushes.emplace_back(cr->x, cr->y, min(as,10), 1.0f);
            }
            else if (cr->age > 300) {
                standardBushes.emplace_back(cr->x, cr->y, as, 1.0f);
            }
            else {
                smallBushes.emplace_back(cr->x, cr->y, max(as, 1.0f), 1.0f);
            }
            break;
        }
        case type_::berry: {
            float s = max(cr->age / SIZEBERRYS, 10.0f);
            berrys.emplace_back(cr->x, cr->y, s, 1.0f);
            break;
        }
        case type_::grass: {
            float as = cr->age / SIZEGRASS;
            if (cr->age > cr->age_limit / 2) {
                bigGrass.emplace_back(cr->x, cr->y, as, 1.0f);
            }
            else if (cr->age > cr->age_limit / 3) {
                standardGrass.emplace_back(cr->x, cr->y, as, 1.0f);
            }
            else {
                smallGrass.emplace_back(cr->x, cr->y, max(as, 1.0f), 1.0f);
            }
            break;
        }
        case type_::rat: {
            float s = max(cr->age / SIZERATS, 10.0f);
            if (cr->infect) {
                infectedRats.emplace_back(cr->x, cr->y, s, 1.0f);
            }
            else {
                healthyRats.emplace_back(cr->x, cr->y, s, 1.0f);
            }
            break;
        }
        }
    }

    // Отрисовка всех групп
    // Животные с одной текстурой
    DrawBatchedInstances(3, wolves);        // волки
    DrawBatchedInstances(2, rabbits);       // кролики  
    DrawBatchedInstances(28, bears);        // медведи
    DrawBatchedInstances(27, berrys);       // ягоды

    // Животные с разделением по полу
    DrawBatchedInstances(8, maleEagles);    // орлы-самцы
    DrawBatchedInstances(16, femaleEagles); // орлы-самки

    // Растения с разделением по размеру
    DrawBatchedInstances(9, smallTrees);    // маленькие деревья
    DrawBatchedInstances(11, standardTrees);// средние деревья  
    DrawBatchedInstances(12, bigTrees);     // большие деревья

    DrawBatchedInstances(7, smallBushes);   // маленькие кусты
    DrawBatchedInstances(13, standardBushes);// средние кусты
    DrawBatchedInstances(14, bigBushes);    // большие кусты

    DrawBatchedInstances(19, smallGrass);   // маленькая трава
    DrawBatchedInstances(20, standardGrass);// средняя трава
    DrawBatchedInstances(21, bigGrass);     // большая трава

    // Животные с проверкой заражения
    DrawBatchedInstances(17, infectedRats); // зараженные крысы
    DrawBatchedInstances(15, healthyRats);  // здоровые крысы
}