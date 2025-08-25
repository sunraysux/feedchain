
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
    int dead_bushes = 0;
    std::vector<std::shared_ptr<Wolf>> new_wolfs;
    std::vector<std::shared_ptr<Rabbit>> new_rabbits;        //список для новых существ
    std::vector<std::shared_ptr<Tree>> new_trees;          //список для новых существ
    std::vector<std::shared_ptr<Bush>> new_bushes;

    for (auto& rabbit : rabbits) rabbit->process(rabbits, new_rabbits, trees, pop);
    for (auto& tree : trees) tree->process(trees, new_trees, pop);
    for (auto& bush : bushes) bush->process(bushes, new_bushes, pop);
    for (auto& wolf : wolfs) wolf->process(wolfs, new_wolfs, rabbits, pop);


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
    remove_dead(wolfs, dead_wolfs);
    remove_dead(trees, dead_trees);
    remove_dead(bushes, dead_bushes);

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
        static_cast<int>(new_bushes.size()) - dead_bushes
    );
    add_new_entities(rabbits, new_rabbits);
    add_new_entities(wolfs, new_wolfs);
    add_new_entities(trees, new_trees);
    add_new_entities(bushes, new_bushes);

}
//инициализация игры
void InitGame() {
    std::random_device rd;
    std::mt19937 gen(rd()); // генератор
    std::uniform_real_distribution<float> plant_dist(-base_rangex , base_rangey );

    Textures::LoadTextureFromFile(1, L"Debug/tree.png");
    Textures::LoadTextureFromFile(2, L"Debug/animal.png");
    Textures::LoadTextureFromFile(3, L"Debug/animal0.jpg");
    Textures::LoadTextureFromFile(4, L"Debug/grass.jpg");
    Textures::LoadTextureFromFile(5, L"Debug/grass2.jpg");
    Textures::LoadTextureFromFile(6, L"Debug/grass3.jpg");
    Textures::LoadTextureFromFile(7, L"Debug/kust.png");
    // Начальные растения
    for (int i = 0; i < 50; i++) {
        auto tree = std::make_shared<Tree>();
        tree->x = 0;
        tree->y = 0;
        tree->age = Random::Int(0, 500);
        tree->updateChunk();
        trees.push_back(tree);
        population.tree_count++;
    }
    for (int i = 0; i < 50; i++) {
        auto bush = std::make_shared<Bush>();
        bush->x = 0;
        bush->y = 0;
        bush->age = Random::Int(0, 500);
        bush->updateChunk();
        bushes.push_back(bush);
        population.bush_count++;
    }
   // Начальные кролики
   for (int i = 0; i < 50; i++) {
       auto rabbit = std::make_shared<Rabbit>();
       rabbit->y = Random::Int(-1, 1);
       rabbit->x = Random::Int(-1, 1);
       rabbit->hunger = Random::Int(-50, 50);
       rabbit->age = Random::Int(0, 500);
       rabbits.push_back(rabbit);
       population.rabbit_count++;
   }
   for (int i = 0; i < 0; i++) {
       auto wolf = std::make_shared<Wolf>();
       wolf->y = Random::Int(-base_rangey, base_rangey);
       wolf->x = Random::Int(-base_rangex, base_rangex);
       wolf->hunger = Random::Int(0, 50);
       wolf->age = Random::Int(0, 50);
       wolfs.push_back(wolf);
       population.wolf_count++;
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
        wolfs.push_back(wolf);
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
void DrawBatchedInstancesfon(int textureIndex, const std::vector<XMFLOAT4>& instances) {
    if (instances.empty()) return;

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

auto isVisible = [&](float x1, float y1, float x2, float y2) {
    // координаты камеры

    // размеры области камеры
    float halfW = Camera::state.widthzoom / 2.0f;
    float halfH = Camera::state.heightzoom / 2.0f;

    // прямоугольник видимости
    float viewMinX = Camera::state.camX - halfW;
    float viewMaxX = Camera::state.camX + halfW;
    float viewMinY = Camera::state.camY - halfH;
    float viewMaxY = Camera::state.camY + halfH;

    // проверка пересечения
    return !(x2 < viewMinX || x1 > viewMaxX ||
        y2 < viewMinY || y1 > viewMaxY);
    };

void ShowRacketAndBall() {

    Shaders::vShader(1);
    Shaders::pShader(1);
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
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    float px1 = x1 + dx * worldWidth;
                    float py1 = y1 + dy * worldHeight;
                    float px2 = x2 + dx * worldWidth;
                    float py2 = y2 + dy * worldHeight;

                    if (isVisible(px1, py1, px2, py2)) {
                        XMFLOAT4 rect(px1, py1, px2, py2);
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
            }


        }
    }


            
        
    

    // Отрисовываем траву батчами
    DrawBatchedInstances(6, lowGrowthInstances);
    DrawBatchedInstances(5, midGrowthInstances);
    DrawBatchedInstances(4, highGrowthInstances);


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
                        float worldWidth = base_rangex * 2.0f;
                        float worldHeight = base_rangey * 2.0f;

                        // 3×3 сетка (оригинал + 8 копий)
                        for (int dx = -1; dx <= 1; dx++) {
                            for (int dy = -1; dy <= 1; dy++) {
                                float px1 = x1 + dx * worldWidth;
                                float py1 = y1 + dy * worldHeight;
                                float px2 = x2 + dx * worldWidth;
                                float py2 = y2 + dy * worldHeight;
                                if (isVisible(px1, py1, px2, py2)) {
                                    instances.emplace_back(px1, py1, px2, py2);
                                }
                            }
                        }
                    }
                }
            }
        }

        DrawBatchedInstances(textureIndex, instances);
        };
    
    drawCreatures(7, [](const Chunk& c) -> const std::vector<std::weak_ptr<Creature>>&{ return c.bushes; }, SIZEBUSHES);
    drawCreatures(2, [](const Chunk& c) -> const std::vector<std::weak_ptr<Creature>>&{ return c.rabbits; }, SIZERABBITS);
    drawCreatures(3, [](const Chunk& c) -> const std::vector<std::weak_ptr<Creature>>&{ return c.wolfs; }, SIZEWOLFS);
    drawCreatures(1, [](const Chunk& c) -> const std::vector<std::weak_ptr<Creature>>&{ return c.trees; }, SIZETREES);
    
}