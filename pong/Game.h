
bool initgame = false;
bool initmenu = false;
void drawWorld()
{

	switch (gameState)
	{
	case gameState_::MainMenu:
		if (!initmenu) {

			
			Textures::LoadTextureFromFile(44, L"Debug/newMenu.png");
			Textures::LoadTextureFromFile(45, L"Debug/cursor.png");
			Textures::LoadTextureFromFile(46, L"Debug/settings.png");

			Textures::LoadTextureFromFile(10, L"Debug/i.jpg");
			Textures::ReadTextureToCPU(10);
			initmenu = true;
		}
		if (settings) {
			ShowCursor(true);
		}
		else {
			ShowCursor(false);
		}

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