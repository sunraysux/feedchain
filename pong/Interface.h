void checkButtons() {
    if ((-0.3 < Camera::state.mousendcY && Camera::state.mousendcY < -0.1) &&
        (-0.1 < Camera::state.mousendcX && Camera::state.mousendcX < 0.1) &&
        GetAsyncKeyState(VK_LBUTTON) & 0x8000)
    {
        ExitProcess(0);
    }
    if ((0 < Camera::state.mousendcY && Camera::state.mousendcY < 0.2) &&
        (-0.1 < Camera::state.mousendcX && Camera::state.mousendcX < 0.1) &&
        GetAsyncKeyState(VK_LBUTTON) & 0x8000)
    {
        gameState = gameState_::game;
    }
}
void drawCursor()
{
    ID3D11ShaderResourceView* texture = nullptr;
    switch (gameState) {
    case gameState_::game:
        ShowCursor(false);
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
        case type_::lightning:
            texture = Textures::Texture[18].TextureResView;
            break;
        case type_::grass:
            texture = Textures::Texture[19].TextureResView;
            break;

        }

        if (texture) {
            context->PSSetShaderResources(0, 1, &texture);
        }
        Shaders::vShader(6);
        Shaders::pShader(6);

        ConstBuf::global[0] = XMFLOAT4(Camera::state.mousendcX, Camera::state.mousendcY, 0.0f, 1.0f);
        ConstBuf::Update(5, ConstBuf::global);
        ConstBuf::ConstToVertex(5);
        Draw::Cursor();
        break;
    case gameState_::pause:
        ShowCursor(false);
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
        case type_::lightning:
            texture = Textures::Texture[18].TextureResView;
            break;
        case type_::grass:
            texture = Textures::Texture[19].TextureResView;
            break;

        }

        if (texture) {
            context->PSSetShaderResources(0, 1, &texture);
        }
        Shaders::vShader(6);
        Shaders::pShader(6);

        ConstBuf::global[0] = XMFLOAT4(Camera::state.mousendcX, Camera::state.mousendcY, 0.0f, 1.0f);
        ConstBuf::Update(5, ConstBuf::global);
        ConstBuf::ConstToVertex(5);
        Draw::Cursor();
        break;
    case gameState_::MainMenu:
        ShowCursor(true);
    }
}

void mouse()
{
    //  HandleCreatureSelection(); // обновляем текущий выбор
    drawCursor(); // отрисовываем курсор с текстурой выбранного животного

    if (seed > 0) {
        if (seed == 1)
        {
            x = Camera::state.mouseX;
            y = Camera::state.mouseY;
            z = Camera::state.mouseZ;
        }
        Shaders::vShader(8);
        Shaders::pShader(8);

        ConstBuf::global[0] = XMFLOAT4(x, y, z + 10, seed);
        ConstBuf::ConstToVertex(5);
        ConstBuf::Update(ConstBuf::getbyname::global, ConstBuf::global);
        Draw::Thunder(1);

        seed++;
        if (seed > 8) // сколько кадров жить
            seed = 0; // выключить

    }
    float barPositions = -0.35;
    float barHeights = -0.85;
    float barBottom = -1;
    float barTop = barHeights;


    if ((barBottom < Camera::state.mousendcY && Camera::state.mousendcY < barHeights) &&
        (barPositions < Camera::state.mousendcX && Camera::state.mousendcX < barPositions + 0.8)) {
        return;
    }

    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
        std::vector<std::shared_ptr<Wolf>> new_wolfs;
        std::vector<std::shared_ptr<Rabbit>> new_rabbits;
        std::vector<std::shared_ptr<Tree>> new_trees;
        std::vector<std::shared_ptr<Bush>> new_bushes;
        std::vector<std::shared_ptr<Eagle>> new_eagles;
        std::vector<std::shared_ptr<Rat>> new_rats;
        std::vector<std::shared_ptr<Grass>> new_grass;

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
            if (heightW(Camera::state.mouseX, Camera::state.mouseY)) {
                return;
            }
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
            if (heightW(Camera::state.mouseX, Camera::state.mouseY)) {
                return;
            }
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
            if (heightW(Camera::state.mouseX, Camera::state.mouseY)) {
                return;
            }
            if (population.canAddTree(static_cast<int>(new_trees.size()))) {
                auto tree = std::make_shared<Tree>();
                tree->y = Wrap(Camera::state.mouseY, base_rangey);
                tree->x = Wrap(Camera::state.mouseX, base_rangex);
                tree->age = 0;
                plant_id += 1;
                tree->id = plant_id;
                tree->updateChunk();
                new_trees.push_back(tree);
                population.tree_count++;
            }
            break;
        }
        case type_::grass: {
            if (heightW(Camera::state.mouseX, Camera::state.mouseY)) {
                return;
            }
            if (population.canAddGrass(static_cast<int>(new_grass.size()))) {
                auto gras = std::make_shared<Grass>();
                gras->y = Wrap(Camera::state.mouseY, base_rangey);
                gras->x = Wrap(Camera::state.mouseX, base_rangex);
                gras->age = 0;
                plant_id += 1;
                gras->id = plant_id;
                gras->updateChunk();
                new_grass.push_back(gras);
                population.grass_count++;
            }
            break;
        }
        case type_::bush: {
            if (heightW(Camera::state.mouseX, Camera::state.mouseY)) {
                return;
            }
            if (population.canAddBush(static_cast<int>(new_bushes.size()))) {
                auto bush = std::make_shared<Bush>();
                bush->y = Wrap(Camera::state.mouseY, base_rangey);
                bush->x = Wrap(Camera::state.mouseX, base_rangex);
                bush->age = 0;
                plant_id += 1;
                bush->id = plant_id;
                new_bushes.push_back(bush);
                population.bush_count++;
            }
            break;
        }
        case type_::eagle: {
            if (heightW(Camera::state.mouseX, Camera::state.mouseY)) {
                return;
            }
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
            if (heightW(Camera::state.mouseX, Camera::state.mouseY)) {
                return;
            }
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
        case type_::lightning: {
            kill_creatures_in_radius(Camera::state.mouseX, Camera::state.mouseY, 50);
            seed += 1;
            break;
        }
        }

        add_new_entities(grass, new_grass);
        add_new_entities(rabbits, new_rabbits);
        add_new_entities(wolves, new_wolfs);
        add_new_entities(trees, new_trees);
        add_new_entities(bushes, new_bushes);
        add_new_entities(eagles, new_eagles);
        add_new_entities(rats, new_rats);
    }
}




void Showpopulations() {

    // Визуализация популяций (две полоски)                                                                  // тут нормализуется количество существ до 1
                                                                                                             // в зависимости от лимита
    Shaders::vShader(2);                                                                                     // и если полоса снизу доходит до края карты
    Shaders::pShader(2);                                                                                     // то количество существ в списке достигло лимита
    float rabbitRatio = min(                                                                                 //
        static_cast<float>(population.rabbit_count) * 2 / population.rabbit_limit,                               //
        2.0f                                                                                                 //
    );                                                                                                       //
    //
    float treeRatio = min(                                                                                  //
        static_cast<float>(population.tree_count) * 2 / population.tree_limit,                                 //
        2.0f                                                                                                 //
    );

    float wolfRatio = min(                                                                                  //
        static_cast<float>(population.wolf_count) * 2 / population.wolf_limit,                                 //
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
    float grassRatio = min(                                                                                  //
        static_cast<float>(population.grass_count) * 2 / population.grass_limit,                                 //
        2.0f                                                                                                 //
    );

    ConstBuf::global[0] = XMFLOAT4(                                                                          //
        rabbitRatio,                                                                                         //
        treeRatio,
        wolfRatio,
        bushRatio

    );
    ConstBuf::global[1] = XMFLOAT4(
        ratRatio,
        eagleRatio,
        grassRatio,
        0

    );

    ConstBuf::Update(5, ConstBuf::global);
    ConstBuf::ConstToVertex(5);
    Draw::NullDrawer18(1);


    Draw::dravslot(1, 3);
    Draw::dravslot(2, 2);
    Draw::dravslot(3, 9);
    Draw::dravslot(4, 7);
    Draw::dravslot(5, 16);
    Draw::dravslot(6, 15);
    Draw::dravslot(7, 19);
    Draw::dravslot(8, 18);
    if (gameSpeed == 1)
        Draw::dravslot(-1, 22);
    if (gameSpeed == 2)
        Draw::dravslot(-1, 23);
    if (gameSpeed == 3)
        Draw::dravslot(-1, 24);
    if (gameSpeed == 4)
        Draw::dravslot(-1, 25);
    if (gameSpeed == 5)
        Draw::dravslot(-1, 26);

}
