
bool initgame = false;
bool initmenu = false;
void drawWorld()
{

	switch (gameState)
	{
	case gameState_::MainMenu:
		if (!initmenu) {

			Textures::LoadTextureFromFile(30, L"Debug/menu.png");
			Textures::LoadTextureFromFile(31, L"Debug/start.png");
			Textures::LoadTextureFromFile(32, L"Debug/exit.png");
			Textures::LoadTextureFromFile(44, L"Debug/newMenu.png");

			Textures::LoadTextureFromFile(10, L"Debug/i.jpg");
			Textures::ReadTextureToCPU(10);
			initmenu = true;
		}
		ShowCursor(true);
		StartMenu();
		// gameState = gameState_::game;
		break;

	case gameState_::game:
		if (!initgame) {
			InitGame();
			initgame = true;
		}
		ShowCursor(false);
		Loop();
		break;
	case gameState_::pause:
		if (!initgame) {
			InitGame();
			initgame = true;
		}
		ShowCursor(false);
		Looppause();
		break;
	}
}