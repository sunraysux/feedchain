
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
    for (int i = 0; i < 0; i++) {
        auto tree = std::make_shared<Tree>();
        tree-> y = Random::Int(-base_rangey, base_rangey);
        tree-> x = Random::Int(-base_rangex, base_rangex);
        tree->age = 0;
        tree->updateChunk();
        trees.push_back(tree);
        population.tree_count++;
    }
    for (int i = 0; i < 0; i++) {
        auto bush = std::make_shared<Bush>();
        bush-> y = Random::Int(-100, 100);
        bush-> x = Random::Int(-100, 100);
        bush->age = Random::Int(0, 1000);
        bush->updateChunk();
        bushes.push_back(bush);
        population.bush_count++;
    }
   // Начальные кролики
   for (int i = 0; i < 0; i++) {
       auto rabbit = std::make_shared<Rabbit>();
       rabbit->y = Random::Int(-100, 100);
       rabbit->x = Random::Int(-100, 100);
       rabbit->hunger = Random::Int(0, 100);
       rabbit->age = Random::Int(0, 100);
       rabbits.push_back(rabbit);
       population.rabbit_count++;
   }
   for (int i = 0; i < 0; i++) {
       auto wolf = std::make_shared<Wolf>();
       wolf->y = Random::Int(-100, 100);
       wolf->x = Random::Int(-100, 100);
       wolf->hunger = Random::Int(0, 500);
       wolf->age = Random::Int(0, 500);
       wolves.push_back(wolf);
       population.wolf_count++;
   }

   for (int i = 0; i < 0; i++) {
       auto eagle = std::make_shared<Eagle>();
       eagle->y = Random::Int(-100, 100);
       eagle->x = Random::Int(-100, 100);
       eagle->hunger = Random::Int(0, 500);
       eagle->age = Random::Int(0, 500);
       eagles.push_back(eagle);
       population.eagle_count++;
   }
   for (int i = 0; i < 0; i++) {
       auto rat = std::make_shared<Rat>();
       rat->y = Random::Int(-100, 100);
       rat->x = Random::Int(-100, 100);
       rat->hunger = Random::Int(0, 100);
       rat->age = Random::Int(0, 100);
       rats.push_back(rat);
       population.rat_count++;
   }
}

type_ currentType = type_::wolf; // по умолчанию волк

void HandleCreatureSelection() {
    if (GetAsyncKeyState('1') & 0x8000) currentType = type_::wolf;
    if (GetAsyncKeyState('2') & 0x8000) currentType = type_::rabbit;
    if (GetAsyncKeyState('3') & 0x8000) currentType = type_::tree;
    if (GetAsyncKeyState('4') & 0x8000) currentType = type_::bush;
    if (GetAsyncKeyState('5') & 0x8000) currentType = type_::eagle;
    if (GetAsyncKeyState('6') & 0x8000) currentType = type_::rat;
}

void drawCursor()
{
    ID3D11ShaderResourceView* texture = nullptr;
    switch (currentType) {
    case type_::wolf:
        texture = Textures::Texture[3].TextureResView;
        break;
    case type_::rabbit:
        texture = Textures::Texture[2].TextureResView;
        break;
    case type_::tree:
        texture = Textures::Texture[9].TextureResView;
        break;
    case type_::bush:
        texture = Textures::Texture[7].TextureResView;
        break;
    case type_::eagle:
        texture = Textures::Texture[8].TextureResView;
        break;
    case type_::rat:
        texture = Textures::Texture[15].TextureResView;
        break;
    }

    if (texture) {
        context->PSSetShaderResources(0, 1, &texture);
    }
    Shaders::vShader(6);
    Shaders::pShader(6);

    ConstBuf::global[0] = XMFLOAT4(Camera::state.mousendcX, Camera::state.mousendcY, 0.0f, 1.0f);
    Draw::Cursor();
}

void mouse()
{
    HandleCreatureSelection(); // обновляем текущий выбор
    drawCursor(); // отрисовываем курсор с текстурой выбранного животного

    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
        std::vector<std::shared_ptr<Wolf>> new_wolfs;
        std::vector<std::shared_ptr<Rabbit>> new_rabbits;
        std::vector<std::shared_ptr<Tree>> new_trees;
        std::vector<std::shared_ptr<Bush>> new_bushes;
        std::vector<std::shared_ptr<Eagle>> new_eagles;
        std::vector<std::shared_ptr<Rat>> new_rats;

        auto add_new_entities = [](auto& dest, auto& src) {
            dest.reserve(dest.size() + src.size());
            for (auto& entity : src) {
                entity->updateChunk();
                dest.emplace_back(std::move(entity));
            }
            src.clear();
            };

        switch (currentType) {
        case type_::wolf: {
            if (population.canAddWolf(static_cast<int>(new_wolfs.size()))) {
                auto wolf = std::make_shared<Wolf>();
                wolf->y = Wrap(Camera::state.mouseY, base_rangey);
                wolf->x = Wrap(Camera::state.mouseX, base_rangex);
                wolf->hunger = 0;
                wolf->age = 0;
                new_wolfs.push_back(wolf);
                population.wolf_count++;
            }
            break;
        }
        case type_::rabbit: {
            if (population.canAddRabbit(static_cast<int>(new_rabbits.size()))) {
                auto rabbit = std::make_shared<Rabbit>();
                rabbit->y = Wrap(Camera::state.mouseY, base_rangey);
                rabbit->x = Wrap(Camera::state.mouseX, base_rangex);
                rabbit->hunger = 0;
                rabbit->age = 0;
                new_rabbits.push_back(rabbit);
                population.rabbit_count++;
            }
            break;
        }
        case type_::tree: {
            if (population.canAddTree(static_cast<int>(new_trees.size()))) {
                auto tree = std::make_shared<Tree>();
                tree->y = Wrap(Camera::state.mouseY, base_rangey);
                tree->x = Wrap(Camera::state.mouseX, base_rangex);
                tree->age = 0;
                tree->updateChunk();
                new_trees.push_back(tree);
                population.tree_count++;
            }
            break;
        }
        case type_::bush: {
            if (population.canAddBush(static_cast<int>(new_bushes.size()))) {
                auto bush = std::make_shared<Bush>();
                bush->y = Wrap(Camera::state.mouseY, base_rangey);
                bush->x = Wrap(Camera::state.mouseX, base_rangex);
                bush->age = 0;
                new_bushes.push_back(bush);
                population.bush_count++;
            }
            break;
        }
        case type_::eagle: {
            if (population.canAddEagle(static_cast<int>(new_eagles.size()))) {
                auto eagle = std::make_shared<Eagle>();
                eagle->y = Wrap(Camera::state.mouseY, base_rangey);
                eagle->x = Wrap(Camera::state.mouseX, base_rangex);
                eagle->hunger = 0;
                eagle->age = 0;
                new_eagles.push_back(eagle);
                population.eagle_count++;
            }
            break;
        }
        case type_::rat: {
            if (population.canAddRat(static_cast<int>(new_rats.size()))) {
                auto rat = std::make_shared<Rat>();
                rat->y = Wrap(Camera::state.mouseY, base_rangey);
                rat->x = Wrap(Camera::state.mouseX, base_rangex);
                rat->hunger = 0;
                rat->age = 0;
                new_rats.push_back(rat);
                population.rat_count++;
            }
            break;
        }
        }

        add_new_entities(rabbits, new_rabbits);
        add_new_entities(wolves, new_wolfs);
        add_new_entities(trees, new_trees);
        add_new_entities(bushes, new_bushes);
        add_new_entities(eagles, new_eagles);
        add_new_entities(rats, new_rats);
    }
}
//void mouse2()
//{
//    if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
//        Camera::screenmouse();
//    }
//
//}

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
    float ratRatio = min(                                                                                  //
        static_cast<float>(population.rat_count) * 2 / population.rat_limit,                                 //
        2.0f                                                                                                 //
    );
    float eagleRatio = min(                                                                                  //
        static_cast<float>(population.eagle_count) * 2 / population.eagle_limit,                                 //
        2.0f                                                                                                 //
    );

    ConstBuf::global[0] = XMFLOAT4(                                                                          //
        rabbitRatio,                                                                                         //
        treeRatio,
        wolfRatio,
        bushRatio
        
    );
    ConstBuf::global[1] = XMFLOAT4(                                                                          //
        ratRatio,                                                                                         //
        eagleRatio,
        0,
        0

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
          //  if (isVisible(x1, y1)) {

                XMFLOAT4 rect(x1, y1, CHUNK_SIZE, 0);
                if (chunk.grass.growth < 33) {
                    lowGrowthInstances.push_back(rect);
                }
                else if (chunk.grass.growth < 66) {
                    midGrowthInstances.push_back(rect);
                }
                else {
                    highGrowthInstances.push_back(rect);



               // }
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
                        if (isVisible(c->x, c->y)) {
                            instances.emplace_back(c->x, c->y, max(c->age / ageScale, 10), 0);
                        }
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
                        if (isVisible(c->x, c->y)) {
                            if (c->age > c->age_limit / 2) {
                                bigInstances.emplace_back(c->x, c->y, c->age / ageScale, 0);
                            }
                            else if (c->age > c->age_limit / 3) {
                                standartInstances.emplace_back(c->x, c->y, c->age / ageScale, 0);
                            }
                            else  {
                                smallInstances.emplace_back(c->x, c->y, max(c->age / ageScale, 5), 0);
                            }
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
                        if (isVisible(c->x, c->y)) {
                            if (c->gender == gender_::male) {
                                maleInstances.emplace_back(c->x, c->y, max(c->age / ageScale, 10), 0);
                            }
                            else if (c->gender == gender_::female) {
                                femaleInstances.emplace_back(c->x, c->y, max(c->age / ageScale, 10), 0);
                            }

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
                        if (isVisible(c->x, c->y)) {
                            if (c->infect == true) {
                                infectInstances.emplace_back(c->x, c->y, max(c->age / ageScale, 10), 0);
                            }
                            else if (c->infect == false) {
                                noinfectInstances.emplace_back(c->x, c->y, max(c->age / ageScale, 10), 0);
                            }
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