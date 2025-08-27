
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
    int dead_bears = 0;
    int dead_bushes = 0;
    std::vector<std::shared_ptr<Wolf>> new_wolfs;
    std::vector<std::shared_ptr<Rabbit>> new_rabbits;        //список для новых существ
    std::vector<std::shared_ptr<Tree>> new_trees;          //список для новых существ
    std::vector<std::shared_ptr<Bush>> new_bushes;
    std::vector<std::shared_ptr<Bear>> new_bears;

    for (auto& rabbit : rabbits) rabbit->process(rabbits, new_rabbits, trees, pop);
    for (auto& tree : trees) tree->process(trees, new_trees, pop);
    for (auto& bush : bushes) bush->process(bushes, new_bushes, pop);
    for (auto& wolf : wolves) wolf->process(wolves, new_wolfs, rabbits, pop);
    for (auto& bear : bears) bear->process(bears, new_bears, rabbits, pop);


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
    remove_dead(bushes, dead_bears);

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
        static_cast<int>(new_bears.size()) - dead_bears
    );
    add_new_entities(rabbits, new_rabbits);
    add_new_entities(wolves, new_wolfs);
    add_new_entities(trees, new_trees);
    add_new_entities(bushes, new_bushes);
    add_new_entities(bears, new_bears);

}
//инициализация игры
void InitGame() {
    std::random_device rd;
    std::mt19937 gen(rd()); // генератор
    std::uniform_real_distribution<float> plant_dist(-base_rangex , base_rangey );



    Textures::LoadTextureFromFile(2, L"Debug/animal.png");
    Textures::CreateDepthForTexture(2);
    Textures::LoadTextureFromFile(3, L"Debug/wolf.png");
    Textures::CreateDepthForTexture(3);
    Textures::LoadTextureFromFile(4, L"Debug/grass.jpg");
    Textures::CreateDepthForTexture(4);
    Textures::LoadTextureFromFile(5, L"Debug/grass2.jpg");
    Textures::CreateDepthForTexture(5);
    Textures::LoadTextureFromFile(6, L"Debug/grass3.jpg");
    Textures::CreateDepthForTexture(6);
    Textures::LoadTextureFromFile(7, L"Debug/kust.png");
    Textures::CreateDepthForTexture(7);
    Textures::LoadTextureFromFile(8, L"Debug/bear.png");
    Textures::CreateDepthForTexture(8);
    Textures::LoadTextureFromFile(9, L"Debug/tree.png");
    Textures::CreateDepthForTexture(9);
    // Начальные растения
    for (int i = 0; i < 1500; i++) {
        auto tree = std::make_shared<Tree>();
        tree-> y = Random::Int(-base_rangey, base_rangey);
        tree-> x = Random::Int(-base_rangex, base_rangex);
        tree->age = Random::Int(0, 500);
        tree->updateChunk();
        trees.push_back(tree);
        population.tree_count++;
    }
    for (int i = 0; i < 1500; i++) {
        auto bush = std::make_shared<Bush>();
        bush-> y = Random::Int(-base_rangey, base_rangey);
        bush-> x = Random::Int(-base_rangex, base_rangex);
        bush->age = Random::Int(0, 500);
        bush->updateChunk();
        bushes.push_back(bush);
        population.bush_count++;
    }
   // Начальные кролики
   for (int i = 0; i < 5000; i++) {
       auto rabbit = std::make_shared<Rabbit>();
       rabbit->y = Random::Int(-base_rangey, base_rangey);
       rabbit->x = Random::Int(-base_rangex, base_rangex);
       rabbit->hunger = Random::Int(0, 500);
       rabbit->age = Random::Int(0, 500);
       rabbits.push_back(rabbit);
       population.rabbit_count++;
   }
   for (int i = 0; i < 50; i++) {
       auto wolf = std::make_shared<Wolf>();
       wolf->y = Random::Int(-base_rangey, base_rangey);
       wolf->x = Random::Int(-base_rangex, base_rangex);
       wolf->hunger = Random::Int(0, 500);
       wolf->age = Random::Int(0, 500);
       wolves.push_back(wolf);
       population.wolf_count++;
   }

   for (int i = 0; i < 50; i++) {
       auto bear = std::make_shared<Bear>();
       bear->y = Random::Int(-base_rangey, base_rangey);
       bear->x = Random::Int(-base_rangex, base_rangex);
       bear->hunger = Random::Int(0, 500);
       bear->age = Random::Int(0, 500);
       bears.push_back(bear);
       population.bear_count++;
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
    Textures::TextureToShader(1, 0, vertex);
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

void ShowRacketAndBall() {
    Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
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
                                float px = c->x + dx * worldWidth;
                                float py = c->y + dy * worldHeight;

                                // Проверяем видимость центра существа
                                if (isVisible(px, py)) {
                                    // Передаём x, y, age, scale
                                    instances.emplace_back(px, py, c->age, ageScale);
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
    drawCreatures(3, [](const Chunk& c) -> const std::vector<std::weak_ptr<Creature>>&{ return c.wolves; }, SIZEWOLFS);
    drawCreatures(8, [](const Chunk& c) -> const std::vector<std::weak_ptr<Creature>>&{ return c.bears; }, SIZEBEARS);
    drawCreatures(9, [](const Chunk& c) -> const std::vector<std::weak_ptr<Creature>>&{ return c.trees; }, SIZETREES);
    
}