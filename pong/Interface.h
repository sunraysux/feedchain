

static bool keyPressed[256] = { 0 };
static bool mouseLeftDown = false;
static bool mouseRightDown = false;
static bool mouseMiddleDown = false;
static int mouseX = 0;
static int mouseY = 0;
static int prevMouseX = 0;
static int prevMouseY = 0;
static short mouseWheelDelta = 0;

float barPositions = -0.85;
float barHeights = -0.85;
float barBottom = -1;
float barTop = barHeights;

float typeBarX = -1;
float typeBarY = -0.2;
float typeBarW = 0.11;
float typeBarH = 0.8;

float speedBarX = 0.61;
float speedBarY = -0.8;
float speedBarW = 0.39;
float speedBarH = 0.2;

float statBTNX = 0.85f;
float statBTNY = 0.85f;
float statBTNW = 0.15f;
float statBTNH = 0.15f;

//Draw::DrawUIimage(47, 0.2, 1, 0, 0.8);
float statistikX = 0.2f;
float statistikY = 0;
float statistikW = 0.8f;
float statistikH = 0.8f;

bool mouseClickedInRect(float x1, float y1, float x2, float y2)
{
    bool insideX = (Camera::state.mousendcX >= x1 && Camera::state.mousendcX <= x2);
    bool insideY = (Camera::state.mousendcY >= y1 && Camera::state.mousendcY <= y2);

    bool leftPressed = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

    return insideX && insideY && leftPressed;
}

void checkButtons()
{
    bool shiftHeld = (GetAsyncKeyState(VK_SHIFT) & 0x8000);

    // Клавиатура
    if (keyPressed[VK_SHIFT])
        Camera::state.speedMul = 10.0f;
    else
        Camera::state.speedMul = 1.0f;

    if (keyPressed[VK_SPACE])
    {
        if (gameState == gameState_::game)
        {
            gameState = gameState_::pause;
            oldGameSpeed = gameSpeed;
            gameSpeed = 6;
        }
        else if (gameState == gameState_::pause)
        {
            gameState = gameState_::game;
            gameSpeed = oldGameSpeed;
        }
        keyPressed[VK_SPACE] = false; 
    }

    if (shiftHeld)
    {
        if(gameSpeed == 6) gameState = gameState_::game;
        if (keyPressed['1']) gameSpeed = 1;
        if (keyPressed['2']) gameSpeed = 2;
        if (keyPressed['3']) gameSpeed = 3;

    }
    else
    {
        if (keyPressed['1']) {
            if (typeSelect == 1) currentType = type_::lightning;
            if (typeSelect == 2) currentType = type_::rabbit;
            if (typeSelect == 3) currentType = type_::wolf;
            if (typeSelect == 4) currentType = type_::tree;
            slot_number = -4;
        }
        if (keyPressed['2']) {
            if (typeSelect == 2) currentType = type_::rat;
            if (typeSelect == 3) currentType = type_::eagle;
            if (typeSelect == 4) currentType = type_::bush;
            slot_number = -3;
        }
        if (keyPressed['3']) {
            if (typeSelect == 3) currentType = type_::bear;
            if (typeSelect == 4) currentType = type_::grass;
            slot_number = -2;
        }
    }

    if (keyPressed[VK_ESCAPE] && gameState != gameState_::MainMenu) {
        keyPressed[VK_ESCAPE] = false;
        settings = false;
        gameState = gameState_::MainMenu;
    }

    // Мышь 
    if (mouseWheelDelta != 0)
    {
        Camera::HandleMouseWheel(mouseWheelDelta);
        mouseWheelDelta = 0;
    }

    // Главное меню
    if (gameState == gameState_::MainMenu)
    {
        if (!settings) {
            if (keyPressed['W']) {
                if (currentStartButton > 1) {
                    currentStartButton -= 1;
                }
                cursorY1 = -0.35 - (0.16 * (currentStartButton - 1));
                cursorY2 = -0.38 - (0.16 * (currentStartButton - 1));
                keyPressed['W'] = false;
            }
            if (keyPressed['S']) {
                if (currentStartButton < 3) {
                    currentStartButton += 1;
                }
                cursorY1 = -0.35 - (0.16 * (currentStartButton - 1));
                cursorY2 = -0.38 - (0.16 * (currentStartButton - 1));
                keyPressed['S'] = false;
            }
        }
        if (keyPressed[VK_RETURN]) {
            switch (currentStartButton)
            {
            case 1:
                gameState = gameState_::game;
            case 2:
                if (settings) settings = false;
                else settings = true;
            default:
                break;
            }
            keyPressed[VK_RETURN] = false;
        }
        if (keyPressed[VK_ESCAPE]) {
            PostQuitMessage(0);
        }

        return;
    }

    // Игра
    if (mouseLeftDown)
    {


        // --- Слоты ---
        if ((barBottom < Camera::state.mousendcY && Camera::state.mousendcY < barHeights) &&
            (barPositions - 0.1f + 1 * 0.1f < Camera::state.mousendcX && Camera::state.mousendcX < barPositions + 3 * 0.1f))
        {
            for (int slot = 1; slot < 4; slot++)
            {
                if ((barBottom < Camera::state.mousendcY && Camera::state.mousendcY < barHeights) &&
                    (barPositions - 0.1f + slot * 0.1f < Camera::state.mousendcX && Camera::state.mousendcX < barPositions + slot * 0.1f))
                {
                    slot_number = slot;
                }
            }

            switch (slot_number)
            {
            case 1:
                if (typeSelect == 1) currentType = type_::lightning;
                if (typeSelect == 2) currentType = type_::rabbit;
                if (typeSelect == 3) currentType = type_::wolf;
                if (typeSelect == 4) currentType = type_::tree;
                slot_number = -4;
                break;

            case 2:
                if (typeSelect == 2) currentType = type_::rat;
                if (typeSelect == 3) currentType = type_::eagle;
                if (typeSelect == 4) currentType = type_::bush;
                slot_number = -3;
                break;

            case 3:
                if (typeSelect == 3) currentType = type_::bear;
                if (typeSelect == 4) currentType = type_::grass;
                slot_number = -2;
                break;
            }
        }

        // --- Панель типов ---
        if ((Camera::state.mousendcX >= typeBarX && Camera::state.mousendcX <= typeBarX + typeBarW) &&
            (Camera::state.mousendcY <= typeBarY && Camera::state.mousendcY >= typeBarY - typeBarH))
        {
            for (int type = 1; type < 5; type++)
            {
                float segmentTop = typeBarY - (type - 1) * (typeBarH / 4.0f);
                float segmentBottom = typeBarY - type * (typeBarH / 4.0f);

                if (Camera::state.mousendcY <= segmentTop && Camera::state.mousendcY >= segmentBottom)
                {
                    if (typeSelect != type)
                    {
                        typeSelect = type;
                        slot_number = -4;

                        if (type == 1) currentType = type_::lightning;
                        if (type == 2) currentType = type_::rabbit;
                        if (type == 3) currentType = type_::wolf;
                        if (type == 4) currentType = type_::tree;
                    }
                    break;
                }
            }
        }

        // --- Панель скорости ---
        if ((Camera::state.mousendcX >= speedBarX && Camera::state.mousendcX <= speedBarX + speedBarW) &&
            (Camera::state.mousendcY <= speedBarY && Camera::state.mousendcY >= speedBarY - speedBarH))
        {
            for (int speed = 1; speed < 5; speed++)
            {
                float segmentLeft = speedBarX - (speed - 1) * (speedBarW / 4.0f) - (speed - 1) * 0.005;
                float segmentRight = speedBarX + speed * (speedBarW / 4.0f) - (speed - 1) * 0.005;

                if (Camera::state.mousendcX >= segmentLeft && Camera::state.mousendcX <= segmentRight)
                {

                    if (speed == 1)
                    {
                        if (gameState != gameState_::pause)
                        {
                            oldGameSpeed = gameSpeed;
                            gameState = gameState_::pause;
                            gameSpeed = 6;
                        }
                        else
                        {
                            gameState = gameState_::game;
                            gameSpeed = oldGameSpeed;
                        }
                        break;
                    }
                    else {
                        if (gameSpeed == 6)
                            gameState = gameState_::game;
                        gameSpeed = speed - 1;
                    }


                    break;
                }
            }
        }

        // ---Статистика---
        if ((Camera::state.mousendcX >= statBTNX && Camera::state.mousendcX <= statBTNX + statBTNW) &&
            (Camera::state.mousendcY <= statBTNY + statBTNH && Camera::state.mousendcY >= statBTNY)) {
            if (!statistik) statistik = true;
            else statistik = false;
        }
        if (statistik) {
            float virtualStatY = 0.05f;
            float virtualStatH = 0.655f;
            float virtualStatX = 0.25f;
            float virtualStatW = 0.21f;

            for (int button = 1; button < 4; button++)
            {
                float segmentTop = virtualStatY + virtualStatH - (button - 1)* (virtualStatH / 3.0f);
                float segmentBottom = virtualStatY + virtualStatH - button * (virtualStatH / 3.0f);

                if ((Camera::state.mousendcY <= segmentTop && Camera::state.mousendcY >= segmentBottom) && (Camera::state.mousendcX >= virtualStatX && Camera::state.mousendcX <= virtualStatX + virtualStatW))
                {
                    switch (button)
                    {
                    case 1:
                        if (plantStat) plantStat = false;
                        else plantStat = true;
                        break;
                    case 2:
                        if (herbivoresStat) herbivoresStat = false;
                        else herbivoresStat = true;
                        break;
                    case 3:
                        if (hunterStat) hunterStat = false;
                        else hunterStat = true;
                        break;
                    default:
                        break;
                    }

                }
            }
        }
        mouseLeftDown = false;
    }
}void drawCursor()
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
        case type_::bear:
            texture = Textures::Texture[28].TextureResView;
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
        case type_::bear:
            texture = Textures::Texture[28].TextureResView;
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
        if (settings) {
            ShowCursor(true);
        }
        else {
            ShowCursor(false);
        }
    }
}

void mouse()
{
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
    


    if ((barBottom < Camera::state.mousendcY && Camera::state.mousendcY < barHeights) &&
        (barPositions < Camera::state.mousendcX && Camera::state.mousendcX < barPositions + 0.3)) {
        return;
    }
    if ((Camera::state.mousendcX >= typeBarX && Camera::state.mousendcX <= typeBarX + typeBarW) &&
        (Camera::state.mousendcY <= typeBarY && Camera::state.mousendcY >= typeBarY - typeBarH)) {
        return;
    }
    if ((Camera::state.mousendcX >= speedBarX && Camera::state.mousendcX <= speedBarX + speedBarW) &&
        (Camera::state.mousendcY <= speedBarY && Camera::state.mousendcY >= speedBarY - speedBarH)) {
        return;
    }
    if ((Camera::state.mousendcX >= statBTNX && Camera::state.mousendcX <= statBTNX + statBTNW) &&
        (Camera::state.mousendcY <= statBTNY+statBTNH && Camera::state.mousendcY >= statBTNY)) {
        return;
    }
    if ((Camera::state.mousendcX >= statistikX && Camera::state.mousendcX <= statistikX + statistikW) &&
        (Camera::state.mousendcY <= statistikY + statistikH && Camera::state.mousendcY >= statistikY) && statistik) {
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
        std::vector<std::shared_ptr<Bear>> new_bears;

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
            if (population.canAddWolf(static_cast<int>(new_wolfs.size())) && tick-wolfSpawnTick > hunterTick) {
                auto wolf = std::make_shared<Wolf>();
                wolf->y = Wrap(Camera::state.mouseY, base_rangey);
                wolf->x = Wrap(Camera::state.mouseX, base_rangex);
                wolf->hunger = 0;
                wolf->age = 0;
                new_wolfs.push_back(wolf);
                population.wolf_count++;
                wolfSpawnTick = tick;
            }
            break;
        }
        case type_::rabbit: {
            if (heightW(Camera::state.mouseX, Camera::state.mouseY)) {
                return;
            }
            if (population.canAddRabbit(static_cast<int>(new_rabbits.size())) && tick - rabbitSpawnTick > herbivoresTick) {
                auto rabbit = std::make_shared<Rabbit>();
                rabbit->y = Wrap(Camera::state.mouseY, base_rangey);
                rabbit->x = Wrap(Camera::state.mouseX, base_rangex);
                rabbit->hunger = 0;
                rabbit->age = 0;
                new_rabbits.push_back(rabbit);
                population.rabbit_count++;
                rabbitSpawnTick = tick;
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
            if (population.canAddEagle(static_cast<int>(new_eagles.size())) && tick - eagleSpawnTick > hunterTick) {
                auto eagle = std::make_shared<Eagle>();
                eagle->y = Wrap(Camera::state.mouseY, base_rangey);
                eagle->x = Wrap(Camera::state.mouseX, base_rangex);
                eagle->hunger = 0;
                eagle->age = 0;
                new_eagles.push_back(eagle);
                population.eagle_count++;
                eagleSpawnTick = tick;
            }
            break;
        }
        case type_::rat: {
            if (heightW(Camera::state.mouseX, Camera::state.mouseY)) {
                return;
            }
            if (population.canAddRat(static_cast<int>(new_rats.size())) && tick - ratSpawnTick > herbivoresTick) {
                auto rat = std::make_shared<Rat>();
                rat->y = Wrap(Camera::state.mouseY, base_rangey);
                rat->x = Wrap(Camera::state.mouseX, base_rangex);
                rat->hunger = 0;
                rat->age = 0;
                new_rats.push_back(rat);
                population.rat_count++;
                ratSpawnTick = tick;
            }
            break;
        }
        case type_::lightning: {
            if (tick - lightingSpawnTick > deadTick) {
                kill_creatures_in_radius(Camera::state.mouseX, Camera::state.mouseY, 50);
                seed += 1;
                lightingSpawnTick = tick;
            }
            break;
        }
        case type_::bear: {
            if (heightW(Camera::state.mouseX, Camera::state.mouseY)) {
                return;
            }
            if (population.canAddBear(static_cast<int>(new_bears.size())) && tick - bearSpawnTick > hunterTick) {
                auto bear = std::make_shared<Bear>();
                bear->y = Wrap(Camera::state.mouseY, base_rangey);
                bear->x = Wrap(Camera::state.mouseX, base_rangex);
                bear->hunger = 0;
                bear->age = 0;
                new_bears.push_back(bear);
                population.bear_count++;
                bearSpawnTick = tick;
            }
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
        add_new_entities(bears, new_bears);
    }
}



void Showpopulations() {


    switch (typeSelect)
    {
    case 1:
        Draw::drawslot(-4, 18, lightingSpawnTick, deadTick); break;
    case 2:
        Draw::drawslot(-4, 2, rabbitSpawnTick, herbivoresTick);
        Draw::drawslot(-3, 15, ratSpawnTick, herbivoresTick);
        break;
    case 3:
        Draw::drawslot(-4, 3, wolfSpawnTick, hunterTick);
        Draw::drawslot(-3, 16, eagleSpawnTick, hunterTick);
        Draw::drawslot(-2, 28, bearSpawnTick, hunterTick);
        break;
    case 4:
        Draw::drawslot(-4, 9,100,1);
        Draw::drawslot(-3, 7, 100, 1);
        Draw::drawslot(-2, 19, 100, 1);
    default:
        break;
    }

  //  if (gameSpeed == 1)
  //      Draw::drawslot(-1, 24);
  //  if (gameSpeed == 2)
  //      Draw::drawslot(-1, 25);
  //  if (gameSpeed == 3)
  //      Draw::drawslot(-1, 26);
  //  //if (gameSpeed == 4)
  //  //    Draw::drawslot(-1, 25);
  //  //if (gameSpeed == 5)
  //  //    Draw::drawslot(-1, 26);
  //  if (gameSpeed == 6)
  //      Draw::drawslot(-1, 43);

    
    Draw::DrawUIimage(41, -1, -0.9, -1, -0.25);
    Draw::DrawUIimage(42, 0.61f, 1, -1, -0.815f);
    Draw::DrawUIimage(40, statBTNX, statBTNX+statBTNW, statBTNY, statBTNY+statBTNH);
    if (statistik) {
        Draw::DrawUIimage(47, statistikX, statistikX+statistikW, statistikY, statistikY+statistikH);
    }

}



