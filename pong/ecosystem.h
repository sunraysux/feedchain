
void ProcessCreatures(PopulationManager& pop) {
    tick++;
    int dead_rabbits = 0;
    int dead_trees = 0;
    int dead_wolfs = 0;
    int dead_eagles = 0;
    int dead_bushes = 0;
    int dead_rats = 0;
    int dead_grass = 0;
    int dead_berrys = 0;
    int dead_bears = 0;
    std::vector<std::shared_ptr<Wolf>> new_wolfs;
    std::vector<std::shared_ptr<Rabbit>> new_rabbits;        //список для новых существ
    std::vector<std::shared_ptr<Tree>> new_trees;          //список для новых существ
    std::vector<std::shared_ptr<Bush>> new_bushes;
    std::vector<std::shared_ptr<Eagle>> new_eagles;
    std::vector<std::shared_ptr<Rat>> new_rats;
    std::vector<std::shared_ptr<Grass>> new_grass;
    std::vector<std::shared_ptr<Berry>> new_berrys;
    std::vector<std::shared_ptr<Bear>> new_bears;
    

    
    for (auto& tree : trees) tree->process(new_trees, pop);
    for (auto& bush : bushes) bush->process(new_bushes,new_berrys, pop);
    for (auto& gras : grass) gras->process(new_grass, pop);
    for (auto& berry : berrys) berry->process(new_berrys, pop);

    for (auto& rabbit : rabbits) rabbit->process(rabbits, new_rabbits, pop);
    for (auto& wolf : wolves) wolf->process(wolves, new_wolfs, pop);
    for (auto& rat : rats) rat->process(rats, new_rats, pop);
    for (auto& eagle : eagles) eagle->process(eagles, new_eagles, pop);
    for (auto& bear : bears) bear->process(bears, new_bears, pop);
    

    auto remove_dead = [](auto& container, int& counter) {
        using ContainerType = typename std::remove_reference<decltype(container)>::type;
        using ValueType = typename ContainerType::value_type;

        container.erase(
            std::remove_if(container.begin(), container.end(),
                [&](const ValueType& entity) {
                    if (entity->shouldDie()) {
                        counter++;
                        entity->removeFromChunk(true);

                        return true;
                    }
                    return false;
                }
            ),
            container.end()
        );
        };
    if (tick % Random::Int(1,5) == 0) {
        remove_dead(grass, dead_grass);
    }
    remove_dead(rabbits, dead_rabbits);
    remove_dead(wolves, dead_wolfs);
    remove_dead(trees, dead_trees);
    remove_dead(bushes, dead_bushes);
    remove_dead(eagles, dead_eagles);
    remove_dead(rats, dead_rats);
    remove_dead(berrys, dead_berrys);
    remove_dead(bears, dead_bears);
    //remove_dead(grass, dead_grass);
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
        static_cast<int>(new_rats.size()) - dead_rats,
        static_cast<int>(new_grass.size()) - dead_grass,
        static_cast<int>(new_berrys.size()) - dead_berrys,
        static_cast<int>(new_bears.size()) - dead_bears
    );
    add_new_entities(rabbits, new_rabbits);
    add_new_entities(wolves, new_wolfs);
    add_new_entities(trees, new_trees);
    add_new_entities(bushes, new_bushes);
    add_new_entities(eagles, new_eagles);
    add_new_entities(rats, new_rats);
    add_new_entities(grass, new_grass);
    add_new_entities(berrys, new_berrys);
    add_new_entities(bears, new_bears);

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
// --- Одна текстура ---
template<typename T, typename F>
void DrawFromSharedVector(int textureIndex, const std::vector<std::shared_ptr<T>>& vec, F toInstance)
{
    if (vec.empty()) return;

    std::vector<XMFLOAT4> instances;
    instances.reserve(vec.size());
    for (const auto& p : vec) {
        if (!p) continue;
        instances.push_back(toInstance(p.get()));
    }
    DrawBatchedInstances(textureIndex, instances);
}

// ---для растений (3 категории по возрасту) ---
template<typename T>
void DrawPlantsBySize(const int arr[4], const std::vector<std::shared_ptr<T>>& vec, float ageScale)
{
    std::vector<XMFLOAT4> smallInstances; smallInstances.reserve(vec.size() / 3);
    std::vector<XMFLOAT4> standardInstances; standardInstances.reserve(vec.size() / 3);
    std::vector<XMFLOAT4> bigInstances; bigInstances.reserve(vec.size() / 3);

    for (const auto& p : vec) {
        if (!p) continue;
        const Creature* c = p.get();
        float as = c->age / ageScale;
        if (c->age > c->age_limit / 2) {
            bigInstances.emplace_back(c->x, c->y, as, static_cast<float>(arr[0]));
        }
        else if (c->age > c->age_limit / 3) {
            standardInstances.emplace_back(c->x, c->y, as, static_cast<float>(arr[0]));
        }
        else {
            smallInstances.emplace_back(c->x, c->y, max(as, 1.0f), static_cast<float>(arr[0]));
        }
    }

    DrawBatchedInstances(arr[1], smallInstances);
    DrawBatchedInstances(arr[2], standardInstances);
    DrawBatchedInstances(arr[3], bigInstances);
}

// ---  для животных с разделением по полу ---
template<typename T>
void DrawAnimalsByGender(const int arr[3], const std::vector<std::shared_ptr<T>>& vec, float ageScale)
{
    std::vector<XMFLOAT4> male; male.reserve(vec.size() / 2);
    std::vector<XMFLOAT4> female; female.reserve(vec.size() / 2);

    for (const auto& p : vec) {
        if (!p) continue;
        const Creature* c = p.get();
        float s = max(c->age / ageScale, 10.0f);
        if (c->gender == gender_::male) male.emplace_back(c->x, c->y, s, arr[0]);
        else female.emplace_back(c->x, c->y, s, arr[0]);
    }

    DrawBatchedInstances(arr[1], male);
    DrawBatchedInstances(arr[2], female);
}

// --- Спец-хелпер для простых одно-текстурных существ (Rabbits/Wolves) ---
template<typename T>
void DrawSimpleCreatures(int textureIndex, const std::vector<std::shared_ptr<T>>& vec, float ageScale)
{
    DrawFromSharedVector(textureIndex, vec, [ageScale](const T* c)->XMFLOAT4 {
        float s = max(c->age / ageScale, 10.0f);
        return XMFLOAT4(c->x, c->y, s, 1.0f);
        });
}

// --- Спец-хелпер для вирусной проверки (infect / not infect) ---
template<typename T>
void DrawInfectCheck(const int arr[2], const std::vector<std::shared_ptr<T>>& vec, float ageScale)
{
    std::vector<XMFLOAT4> infect; infect.reserve(vec.size() / 2);
    std::vector<XMFLOAT4> noinfect; noinfect.reserve(vec.size() / 2);

    for (const auto& p : vec) {
        if (!p) continue;
        const Creature* c = p.get();
        float s = max(c->age / ageScale, 10.0f);
        if (c->infect) infect.emplace_back(c->x, c->y, s, 1.0f);
        else noinfect.emplace_back(c->x, c->y, s, 1.0f);
    }

    DrawBatchedInstances(arr[0], infect);
    DrawBatchedInstances(arr[1], noinfect);
}


  
void ShowRacketAndBallFromVectors(
    const std::vector<std::shared_ptr<Rabbit>>& rabbits,
    const std::vector<std::shared_ptr<Tree>>& trees,
    const std::vector<std::shared_ptr<Wolf>>& wolves,
    const std::vector<std::shared_ptr<Bush>>& bushes,
    const std::vector<std::shared_ptr<Eagle>>& eagles,
    const std::vector<std::shared_ptr<Rat>>& rats,
    const std::vector<std::shared_ptr<Grass>>& grass,
    const std::vector<std::shared_ptr<Berry>>& berrys,
    const std::vector<std::shared_ptr<Bear>>& bears)
{
    Shaders::vShader(0);
    Shaders::pShader(0);

    int tree_arr[] = { 2,9,11,12 };
    int bush_arr[] = { 1,7,13,14 };
    int eagle_arr[] = {2, 8,16 };
    int rat_arr[] = { 17,15 };   
    int grass_arr[] = { 1,19,20,21 };

    // Рисуем: напрямую из векторов
    DrawSimpleCreatures<Rabbit>(2, rabbits, SIZERABBITS);   
    DrawSimpleCreatures<Wolf>(3, wolves, SIZEWOLFS);
    DrawSimpleCreatures<Bear>(28, bears, SIZEBEARS);
    

    DrawAnimalsByGender<Eagle>(eagle_arr, eagles, SIZEAGLES);
    DrawPlantsBySize<Tree>(tree_arr, trees, SIZETREES);      
    DrawPlantsBySize<Bush>(bush_arr, bushes, SIZEBUSHES);    
    DrawSimpleCreatures<Berry>(27, berrys, SIZEBERRYS);
    DrawPlantsBySize<Grass>(grass_arr, grass, SIZEGRASS);    
    DrawInfectCheck<Rat>(rat_arr, rats, SIZERATS);           
}


