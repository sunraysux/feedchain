
//основной процесс

void UpdateAllGrass() {
    for (int cx = 0; cx < CHUNKS_PER_SIDEX; ++cx) {
        for (int cy = 0; cy < CHUNKS_PER_SIDEY; ++cy) {
            chunk_grid[cx][cy].UpdateGrassGrowth(cx,cy);
        }
    }
}
void ProcessCreatures(PopulationManager& pop) {

    int dead_rabbits = 0;
    int dead_trees = 0;
    int dead_wolfs = 0;
    int dead_eagles = 0;
    int dead_bushes = 0;
    int dead_rats = 0;
    std::vector<std::shared_ptr<Wolf>> new_wolfs;
    std::vector<std::shared_ptr<Rabbit>> new_rabbits;        //список для новых существ
    std::vector<std::shared_ptr<Tree>> new_trees;          //список для новых существ
    std::vector<std::shared_ptr<Bush>> new_bushes;
    std::vector<std::shared_ptr<Eagle>> new_eagles;
    std::vector<std::shared_ptr<Rat>> new_rats;

    for (auto& rabbit : rabbits) rabbit->process(rabbits, new_rabbits, trees, pop);
    for (auto& tree : trees) tree->process(trees, new_trees, pop);
    for (auto& bush : bushes) bush->process(bushes, new_bushes, pop);
    for (auto& wolf : wolves) wolf->process(wolves, new_wolfs, rabbits, pop);
    for (auto& rat : rats) rat->process(rats, new_rats, bushes, pop);
    for (auto& eagle : eagles) eagle->process(eagles, new_eagles, rats, pop);


    auto remove_dead = [](auto& container, int& counter) {
        using ContainerType = typename std::remove_reference<decltype(container)>::type;
        using ValueType = typename ContainerType::value_type;

        container.erase(
            std::remove_if(container.begin(), container.end(),
                [&](const ValueType& entity) {
                    if (entity->shouldDie()) {
                        counter++;
                        entity->removeFromChunk();
                        return true;
                    }
                    return false;
                }
            ),
            container.end()
        );
        };

    remove_dead(rabbits, dead_rabbits);
    remove_dead(wolves, dead_wolfs);
    remove_dead(trees, dead_trees);
    remove_dead(bushes, dead_bushes);
    remove_dead(eagles, dead_eagles);
    remove_dead(rats, dead_rats);

    // 3.  добавления новых существ
    auto add_new_entities = [](auto& dest, auto& src) {
        dest.reserve(dest.size() + src.size());
        for (auto& entity : src) {
            entity->updateChunk();
            dest.emplace_back(std::move(entity));
        }
        src.clear();
        };
    pop.update(
        static_cast<int>(new_rabbits.size()) - dead_rabbits,//обновление статистики
        static_cast<int>(new_trees.size()) - dead_trees,
        static_cast<int>(new_wolfs.size()) - dead_wolfs,
        static_cast<int>(new_bushes.size()) - dead_bushes,
        static_cast<int>(new_eagles.size()) - dead_eagles,
        static_cast<int>(new_rats.size()) - dead_rats
    );
    add_new_entities(rabbits, new_rabbits);
    add_new_entities(wolves, new_wolfs);
    add_new_entities(trees, new_trees);
    add_new_entities(bushes, new_bushes);
    add_new_entities(eagles, new_eagles);
    add_new_entities(rats, new_rats);

}
//инициализация игры
void InitGame() {
    std::random_device rd;
    std::mt19937 gen(rd()); // генератор
    std::uniform_real_distribution<float> plant_dist(-base_rangex , base_rangey );



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
    Textures::LoadTextureFromFile(10, L"Debug/i.jpg");
    Textures::ReadTextureToCPU(10);
   // Textures::CreateDepthForTexture(10);
    Textures::LoadTextureFromFile(11, L"Debug/standartTree.png");
    Textures::LoadTextureFromFile(12, L"Debug/bigTree.png");
    Textures::LoadTextureFromFile(13, L"Debug/standartBush.png");
    Textures::LoadTextureFromFile(14, L"Debug/bigBush.png");
    Textures::LoadTextureFromFile(15, L"Debug/rat.png");
    Textures::LoadTextureFromFile(16, L"Debug/eagleFemale.png");
    Textures::LoadTextureFromFile(17, L"Debug/infectRat.png");

    // Начальные растения
    for (int i = 0; i < 10; i++) {
        auto tree = std::make_shared<Tree>();
        tree-> y = Random::Int(-base_rangey, base_rangey);
        tree-> x = Random::Int(-base_rangex, base_rangex);
        tree->age = Random::Int(0, 500);
        tree->updateChunk();
        trees.push_back(tree);
        population.tree_count++;
    }
    for (int i = 0; i < 10; i++) {
        auto bush = std::make_shared<Bush>();
        bush-> y = Random::Int(-base_rangey, base_rangey);
        bush-> x = Random::Int(-base_rangex, base_rangex);
        bush->age = Random::Int(0, 500);
        bush->updateChunk();
        bushes.push_back(bush);
        population.bush_count++;
    }
   // Начальные кролики
   for (int i = 0; i < 10; i++) {
       auto rabbit = std::make_shared<Rabbit>();
       rabbit->y = Random::Int(-100, 100);
       rabbit->x = Random::Int(-100, 100);
       rabbit->hunger = Random::Int(0, 10);
       rabbit->age = Random::Int(0, 500);
       rabbits.push_back(rabbit);
       population.rabbit_count++;
   }
   for (int i = 0; i < 10; i++) {
       auto wolf = std::make_shared<Wolf>();
       wolf->y = Random::Int(-base_rangey, base_rangey);
       wolf->x = Random::Int(-base_rangex, base_rangex);
       wolf->hunger = Random::Int(0, 500);
       wolf->age = Random::Int(0, 500);
       wolves.push_back(wolf);
       population.wolf_count++;
   }

   for (int i = 0; i < 10; i++) {
       auto eagle = std::make_shared<Eagle>();
       eagle->y = Random::Int(-base_rangey, base_rangey);
       eagle->x = Random::Int(-base_rangex, base_rangex);
       eagle->hunger = Random::Int(0, 500);
       eagle->age = Random::Int(0, 500);
       eagles.push_back(eagle);
       population.eagle_count++;
   }
   for (int i = 0; i < 10; i++) {
       auto rat = std::make_shared<Rat>();
       rat->y = Random::Int(-base_rangey, base_rangey);
       rat->x = Random::Int(-base_rangex, base_rangex);
       rat->hunger = Random::Int(0, 500);
       rat->age = Random::Int(0, 500);
       rats.push_back(rat);
       population.rat_count++;
   }
}
void mouse()
{
    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
        Camera::screenmouse();
        auto wolf = std::make_shared<Wolf>();
        wolf->y = Wrap(Camera::state.mouseY,base_rangey);
        wolf->x = Wrap(Camera::state.mouseX,base_rangex);
        wolf->hunger = 0;
        wolf->age = 0;
        wolves.push_back(wolf);
        population.wolf_count++;

    }

}
void mouse2()
{
    if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
        Camera::screenmouse();
        auto rabbit = std::make_shared<Rabbit>();
        rabbit->y = Wrap(Camera::state.mouseY, base_rangey);
        rabbit->x = Wrap(Camera::state.mouseX, base_rangex);
        rabbit->hunger = 0;
        rabbit->age = 0;
        rabbits.push_back(rabbit);
        population.rabbit_count++;

    }

}

void Showpopulations() {

    // Визуализация популяций (две полоски)                                                                  // тут нормализуется количество существ до 1
                                                                                                             // в зависимости от лимита
    Shaders::vShader(2);                                                                                     // и если полоса снизу доходит до края карты
    Shaders::pShader(2);                                                                                     // то количество существ в списке достигло лимита
    float rabbitRatio = min(                                                                                 //
        static_cast<float>(population.rabbit_count)*2 / population.rabbit_limit,                               //
        2.0f                                                                                                 //
    );                                                                                                       //
                                                                                                             //
    float treeRatio = min(                                                                                  //
        static_cast<float>(population.tree_count)*2 / population.tree_limit,                                 //
        2.0f                                                                                                 //
    );       

    float wolfRatio = min(                                                                                  //
        static_cast<float>(population.wolf_count)*2 / population.wolf_limit,                                 //
        2.0f                                                                                                 //
    );
    float bushRatio = min(                                                                                  //
        static_cast<float>(population.bush_count) * 2 / population.bush_limit,                                 //
        2.0f                                                                                                 //
    );

    ConstBuf::global[0] = XMFLOAT4(                                                                          //
        rabbitRatio,                                                                                         //
        treeRatio,
        wolfRatio,
        bushRatio
        
    );

    ConstBuf::Update(5, ConstBuf::global);
    ConstBuf::ConstToVertex(5);
    Draw::NullDrawer18(1); 
    
}
int BATCH_SIZE = 4000;
void DrawBatchedInstances(int textureIndex, const std::vector<XMFLOAT4>& instances) {
    if (instances.empty()) return;
    Textures::TextureToShader(10, 0, vertex);
    context->PSSetShaderResources(0, 1, &Textures::Texture[textureIndex].TextureResView);
    for (size_t start = 0; start < instances.size(); start += BATCH_SIZE) {
        size_t count = min(BATCH_SIZE, static_cast<int>(instances.size() - start));

        // Копируем порцию данных в ConstBuf::global
        std::copy(instances.begin() + start, instances.begin() + start + count, ConstBuf::global);

        ConstBuf::Update(5, ConstBuf::global);
        ConstBuf::ConstToVertex(5);
        Draw::NullDrawer(1, static_cast<int>(count));
    }
}
auto isVisible = [&](float x, float y) -> bool {
    // Вектор направления камеры
    XMVECTOR camForward = Camera::state.Forward;
    XMVECTOR camPos = Camera::state.Eye;

    // Центр на XY
    float centerX = XMVectorGetX(Camera::state.at);
    float centerY = XMVectorGetY(Camera::state.at);

    // Половина размеров видимой области на плоскости XY (упрощение)
    float halfHeight = Camera::state.camDist * tanf(Camera::state.fovAngle );
    float halfWidth = halfHeight * ((float)width / (float)height);

    // Границы
    float minX = centerX - halfWidth ;
    float maxX = centerX + halfWidth ;
    float minY = centerY - halfHeight;
    float maxY = centerY + halfHeight*2.3 ;

    // Если объект попадает в прямоугольник
    return (x >= minX && x <= maxX && y >= minY && y <= maxY);
    };
void ShowGrow() {
    Shaders::vShader(5);
    Shaders::pShader(0);
    // Векторы для разных групп травы
    std::vector<XMFLOAT4> lowGrowthInstances;
    std::vector<XMFLOAT4> midGrowthInstances;
    std::vector<XMFLOAT4> highGrowthInstances;

    // Собираем траву по чанкам
    for (int cy = CHUNKS_PER_SIDEY - 1; cy >= 0; --cy) {
        for (int cx = 0; cx < CHUNKS_PER_SIDEX; ++cx) {
            const Chunk& chunk = chunk_grid[cx][cy];
            int x1 = cx * CHUNK_SIZE - base_rangex;
            int y1 = cy * CHUNK_SIZE - base_rangey;
            int x2 = x1 + CHUNK_SIZE;
            int y2 = y1 + CHUNK_SIZE;
            float worldWidth = base_rangex * 2.0f;
            float worldHeight = base_rangey * 2.0f;

            XMFLOAT4 rect(x1, y1, CHUNK_SIZE, 0);
            if (chunk.grass.growth < 33) {
                lowGrowthInstances.push_back(rect);
            }
            else if (chunk.grass.growth < 66) {
                midGrowthInstances.push_back(rect);
            }
            else {
                highGrowthInstances.push_back(rect);




            }
        }


    }
    // Отрисовываем траву батчами
    DrawBatchedInstances(6, lowGrowthInstances);
    DrawBatchedInstances(5, midGrowthInstances);
    DrawBatchedInstances(4, highGrowthInstances);
}
void ShowRacketAndBall() {
    Shaders::vShader(0);
    Shaders::pShader(0);
   

    // Универсальная функция для сбора и отрисовки существ
    auto drawCreatures = [&](int textureIndex, auto&& getCreatureList, float ageScale) {
        std::vector<XMFLOAT4> instances;

        for (int cy = CHUNKS_PER_SIDEY - 1; cy >= 0; --cy) {
            for (int cx = 0; cx < CHUNKS_PER_SIDEX; ++cx) {
                const auto& creatureList = getCreatureList(chunk_grid[cx][cy]);
                for (const auto& weakPtr : creatureList) {
                    if (auto c = weakPtr.lock()) {
                        float t = c->age / ageScale;
                        float x1 = c->x - t / 1.2f;
                        float y1 = c->y;
                        float x2 = c->x + t;
                        float y2 = c->y + t;
                        /*if (c->type == type_::bush) {

                        }*/
                        instances.emplace_back(c->x , c->y, max(c->age/ ageScale,10),8);
                     
                    }   
                }
            }

        }
        DrawBatchedInstances(textureIndex, instances);
        };
    auto drawPlant = [&](int arr [], auto&& getCreatureList, float ageScale) {
        std::vector<XMFLOAT4> smallInstances;
        std::vector<XMFLOAT4> standartInstances;
        std::vector<XMFLOAT4> bigInstances;
        for (int cy = CHUNKS_PER_SIDEY - 1; cy >= 0; --cy) {
            for (int cx = 0; cx < CHUNKS_PER_SIDEX; ++cx) {
                const auto& creatureList = getCreatureList(chunk_grid[cx][cy]);
                for (const auto& weakPtr : creatureList) {
                    if (auto c = weakPtr.lock()) {
                        float t = c->age / ageScale;
                        float x1 = c->x - t / 1.2f;
                        float y1 = c->y;
                        float x2 = c->x + t;
                        float y2 = c->y + t;
                        
                        if (c->age > c->age_limit / 2) {
                            bigInstances.emplace_back(c->x, c->y, max(c->age / ageScale, 10), 8);
                        }
                        else if (c->age > c->age_limit / 3) {
                            standartInstances.emplace_back(c->x, c->y, max(c->age / ageScale, 10), 8);
                        }
                        else if (c->age > c->age_limit / 4) {
                            smallInstances.emplace_back(c->x, c->y, max(c->age / ageScale, 10), 8);
                        }

                    }
                }
            }

        }
       
        
        DrawBatchedInstances(arr[0], smallInstances);
        DrawBatchedInstances(arr[1], standartInstances);
        DrawBatchedInstances(arr[2], bigInstances);
        };
    auto drawAnimals = [&](int arr[], auto&& getCreatureList, float ageScale) {
        std::vector<XMFLOAT4> maleInstances;
        std::vector<XMFLOAT4> femaleInstances;
        
        for (int cy = CHUNKS_PER_SIDEY - 1; cy >= 0; --cy) {
            for (int cx = 0; cx < CHUNKS_PER_SIDEX; ++cx) {
                const auto& creatureList = getCreatureList(chunk_grid[cx][cy]);
                for (const auto& weakPtr : creatureList) {
                    if (auto c = weakPtr.lock()) {
                        float t = c->age / ageScale;
                        float x1 = c->x - t / 1.2f;
                        float y1 = c->y;
                        float x2 = c->x + t;
                        float y2 = c->y + t;

                        if (c->gender == gender_::male) {
                            maleInstances.emplace_back(c->x, c->y, max(c->age / ageScale, 10), 8);
                        }
                        else if (c->gender == gender_::female) {
                            femaleInstances.emplace_back(c->x, c->y, max(c->age / ageScale, 10), 8);
                        }
                        

                    }
                }
            }

        }


        DrawBatchedInstances(arr[0], maleInstances);
        DrawBatchedInstances(arr[1], femaleInstances);
        
        };
    auto drawVirusCheack = [&](int arr[], auto&& getCreatureList, float ageScale) {
        std::vector<XMFLOAT4> infectInstances;
        std::vector<XMFLOAT4> noinfectInstances;

        for (int cy = CHUNKS_PER_SIDEY - 1; cy >= 0; --cy) {
            for (int cx = 0; cx < CHUNKS_PER_SIDEX; ++cx) {
                const auto& creatureList = getCreatureList(chunk_grid[cx][cy]);
                for (const auto& weakPtr : creatureList) {
                    if (auto c = weakPtr.lock()) {
                        float t = c->age / ageScale;
                        float x1 = c->x - t / 1.2f;
                        float y1 = c->y;
                        float x2 = c->x + t;
                        float y2 = c->y + t;

                        if (c->infect == true ) {
                            infectInstances.emplace_back(c->x, c->y, max(c->age / ageScale, 10), 8);
                        }
                        else if (c->infect == false) {
                            noinfectInstances.emplace_back(c->x, c->y, max(c->age / ageScale, 10), 8);
                        }


                    }
                }
            }

        }


        DrawBatchedInstances(arr[0], infectInstances);
        DrawBatchedInstances(arr[1], noinfectInstances);

        };

    int tree_arr[] = { 9,11,12 };
    int bush_arr[] = { 7,13,14 };
    int eagle_arr[] = {8,16};
    int rat_arr[] = {17,15};
    drawPlant(bush_arr, [](const Chunk& c) -> const std::vector<std::weak_ptr<Creature>>&{ return c.bushes; }, SIZEBUSHES);
    drawVirusCheack(rat_arr, [](const Chunk& c) -> const std::vector<std::weak_ptr<Creature>>& { return c.rats; }, SIZERATS);
    drawCreatures(2, [](const Chunk& c) -> const std::vector<std::weak_ptr<Creature>>&{ return c.rabbits; }, SIZERABBITS);
    drawCreatures(3, [](const Chunk& c) -> const std::vector<std::weak_ptr<Creature>>&{ return c.wolves; }, SIZEWOLFS);
    drawAnimals(eagle_arr, [](const Chunk& c) -> const std::vector<std::weak_ptr<Creature>>&{ return c.eagles; }, SIZEAGLES);
    drawPlant(tree_arr, [](const Chunk& c) -> const std::vector<std::weak_ptr<Creature>>& { return c.trees; }, SIZETREES);
    
}