void terraloop()
{
	InputAssembler::IA(InputAssembler::topology::triList);
	Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);

	Textures::RenderTarget(1, 0);
	Draw::Clear({ 0,0,0,0 });
	Draw::ClearDepth();
	Depth::Depth(Depth::depthmode::on);
	Rasterizer::Cull(Rasterizer::cullmode::off);
	Shaders::vShader(1);
	Shaders::pShader(1);
	ConstBuf::ConstToVertex(4);
	ConstBuf::ConstToPixel(4);
	Draw::NullDrawer(1);
	Draw::Present();
}
void StartMenu() {

	Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
	Camera::Update();
	frameConst();
	Textures::RenderTarget(0, 0);
	Draw::Clear({ 1,1,1,0 });
	Draw::ClearDepth();
	drawCursor();

	Shaders::vShader(7);
	Shaders::pShader(7);

	context->PSSetShaderResources(0, 1, &Textures::Texture[30].TextureResView);
	ConstBuf::global[0] = XMFLOAT4(0.1, 0.1, 0, 0.5);
	ConstBuf::Update(5, ConstBuf::global);
	ConstBuf::ConstToVertex(5);
	Draw::NullDrawer(1);


	context->PSSetShaderResources(0, 1, &Textures::Texture[31].TextureResView);
	ConstBuf::global[0] = XMFLOAT4(0.1, 0.1, 0, 0.1);
	ConstBuf::Update(5, ConstBuf::global);
	ConstBuf::ConstToVertex(5);
	Draw::NullDrawer(1);


	context->PSSetShaderResources(0, 1, &Textures::Texture[32].TextureResView);
	ConstBuf::global[0] = XMFLOAT4(0.1, 0.1, 0, -0.2);
	ConstBuf::Update(5, ConstBuf::global);
	ConstBuf::ConstToVertex(5);
	Draw::NullDrawer(1);
	checkButtons();
	Draw::Present();

}

void Loop() {
	Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
	Camera::Update();
	frameConst();
	Textures::RenderTarget(0, 0);
	Draw::Clear({ 0,0,0,0 });
	Draw::ClearDepth();
	Rasterizer::Cull(Rasterizer::cullmode::off);
	ticloop++;
	switch (gameSpeed) {
	case 1: // 0.25x → раз в 4 кадра
		if (ticloop >= 4) { ProcessCreatures(population);  ticloop = 0; }
		break;
	case 2: // 0.5x → раз в 2 кадра
		if (ticloop >= 2) { ProcessCreatures(population);  ticloop = 0; }
		break;
	case 3: // 1x → каждый кадр
		ProcessCreatures(population);  ticloop = 0;
		break;
	case 4: // 2x → два раза за кадр
		ProcessCreatures(population); 
		ProcessCreatures(population);
		ticloop = 0;
		break;
	case 5: // 4x → четыре раза за кадр
		for (int i = 0; i < 5; i++) { ProcessCreatures(population);  }
		ticloop = 0;
		break;
	}
	//ShowGrow();

	mouse();
	ShowRacketAndBallFromVectors(
		rabbits,
		trees,
		wolves,
		bushes,
		eagles,
		rats,
		grass,
		berrys,
		bears);

	Showpopulations();
	//UpdateAllGrass();

	//mouse2();


	//вода




	//рельеф
	Shaders::vShader(3);
	Shaders::pShader(3);

	ConstBuf::global[0] = XMFLOAT4(64, 64, Camera::state.camX, Camera::state.camY);
	ConstBuf::global[1] = XMFLOAT4(1024, 1024, 0, 0);
	ConstBuf::ConstToVertex(5);
	ConstBuf::Update(5, ConstBuf::global);
	Textures::TextureToShader(1, 0, vertex);
	Draw::NullDrawer(32768*8);
	
	//Depth::Depth(Depth::depthmode::readonly);
	//Textures::RenderTarget(0, 0);
	//Depth::SetWaterRasterizer(); 


	Textures::TextureToShader(1, 0);
	Shaders::vShader(4);
	Shaders::pShader(4);

	ConstBuf::global[0] = XMFLOAT4(waterLevel, 0, 0, 0);
	ConstBuf::ConstToVertex(5);
	ConstBuf::Update(ConstBuf::getbyname::global, ConstBuf::global);
	Draw::NullDrawer(1,9);
	//Depth::ResetRasterizer();
	waterLevel = 450;
	Draw::Present();
}

void Looppause() {
	Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
	Camera::Update();
	frameConst();
	Textures::RenderTarget(0, 0);
	Draw::Clear({ 0,0,0,0 });
	Draw::ClearDepth();

	//ProcessCreatures(population);
	//ShowGrow();

	mouse();
	ShowRacketAndBallFromVectors(
		rabbits,
		trees,
		wolves,
		bushes,
		eagles,
		rats,
		grass,
		berrys,
		bears);

	Showpopulations();

	//mouse2();


	//вода




	Shaders::vShader(3);
	Shaders::pShader(3);

	ConstBuf::global[0] = XMFLOAT4(64, 64, Camera::state.camXChunk, Camera::state.camYChunk);
	ConstBuf::global[1] = XMFLOAT4(1024, 1024, 0, 0);
	ConstBuf::ConstToVertex(5);
	ConstBuf::Update(5, ConstBuf::global);
	Textures::TextureToShader(1, 0, vertex);
	Draw::NullDrawer(32768 / 8, 81);

	//Depth::Depth(Depth::depthmode::readonly);
	//Textures::RenderTarget(0, 0);
	//Depth::SetWaterRasterizer(); 


	Textures::TextureToShader(1, 0);
	Shaders::vShader(4);
	Shaders::pShader(4);

	ConstBuf::global[0] = XMFLOAT4(waterLevel, Camera::state.camXChunk, Camera::state.camYChunk, 0);
	ConstBuf::ConstToVertex(5);
	ConstBuf::Update(ConstBuf::getbyname::global, ConstBuf::global);
	Draw::NullDrawer(1, 81);
	//Depth::ResetRasterizer();
	waterLevel = 0.6;
	Draw::Present();

}


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
			Textures::LoadTextureFromFile(10, L"Debug/i.jpg");
			Textures::ReadTextureToCPU(1);
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