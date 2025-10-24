


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
	



	Draw::DrawUIimage(45, -0.56, -0.58, cursorY1, cursorY2);

	if (settings) {
		drawCursor();
		Draw::DrawUIimage(46, -0.5, 0.5, -0.5, 0.5);
	}
	if (info) {
		drawCursor();
		Draw::DrawUIimage(55, -0.5, 0.5, -0.5, 0.5);
	}


	Draw::DrawUIimage(44, -1, 1, -1, 1);

	
	Draw::Present();

}

void Loop() {
	Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
	Camera::Update();
	frameConst();
	Textures::RenderTarget(0, 0);
	Draw::Clear({ 0,0,0,0 });
	Draw::ClearDepth();
	ticloop++;
	switch (gameSpeed) {
	
	case 1: // 1x → каждый кадр
		ProcessCreatures(population);  
		ticloop = 0;
		break;
	case 2: // 2x → два раза за кадр
		ProcessCreatures(population); 
		ProcessCreatures(population);
		ticloop = 0;
		break;
	case 3: // 4x → четыре раза за кадр
		ProcessCreatures(population); 
		ProcessCreatures(population);
		ProcessCreatures(population);
		ProcessCreatures(population);
		ProcessCreatures(population);
		ProcessCreatures(population);
		ProcessCreatures(population);
		ProcessCreatures(population);
		ProcessCreatures(population);
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

	ConstBuf::global[0] = XMFLOAT4(128, 128, 0, 0);
	ConstBuf::global[1] = XMFLOAT4(base_rangex, base_rangey, 0, 0);
	ConstBuf::ConstToVertex(5);
	ConstBuf::Update(5, ConstBuf::global);
	Textures::TextureToShader(10, 0, vertex);
	Draw::NullDrawer(32768 /2);
	
	//Depth::Depth(Depth::depthmode::readonly);
	//Textures::RenderTarget(0, 0);
	//Depth::SetWaterRasterizer();


	Textures::TextureToShader(10, 0, pixel);
	Shaders::vShader(4);
	Shaders::pShader(4);

	ConstBuf::global[0] = XMFLOAT4(waterLevel, 0, 0, 0);
	ConstBuf::ConstToVertex(5);
	ConstBuf::Update(ConstBuf::getbyname::global, ConstBuf::global);
	Draw::NullDrawer(5);
	//Depth::ResetRasterizer();
	waterLevel = 0.6;
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




	//рельеф
	Shaders::vShader(3);
	Shaders::pShader(3);

	ConstBuf::global[0] = XMFLOAT4((float)CHUNKS_PER_SIDEX*2, (float)CHUNKS_PER_SIDEY*4, 0, 0);
	ConstBuf::global[1] = XMFLOAT4(base_rangex, base_rangey, 0, 0);
	ConstBuf::ConstToVertex(5);
	ConstBuf::Update(5, ConstBuf::global);
	Textures::TextureToShader(10, 0, vertex);
	Draw::NullDrawer(32768 *16);

	//Depth::Depth(Depth::depthmode::readonly);
	//Textures::RenderTarget(0, 0);3
	//Depth::SetWaterRasterizer();


	Textures::TextureToShader(10, 0, pixel);
	Shaders::vShader(4);
	Shaders::pShader(4);

	ConstBuf::global[0] = XMFLOAT4(waterLevel, 0, 0, 0);
	ConstBuf::ConstToVertex(5);
	ConstBuf::Update(ConstBuf::getbyname::global, ConstBuf::global);
	Draw::NullDrawer(5);
	//Depth::ResetRasterizer();
	waterLevel = 0.6;
	Draw::Present();

}

