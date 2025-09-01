void terraloop()
{
	InputAssembler::IA(InputAssembler::topology::triList);
	Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);

	Textures::RenderTarget(1, 0);
	Draw::Clear({ 0,0,0,0 });
	Draw::ClearDepth();
	Depth::Depth(Depth::depthmode::on);
	Rasterizer::Cull(Rasterizer::cullmode::front);
	Shaders::vShader(1);
	Shaders::pShader(1);
	ConstBuf::ConstToVertex(4);
	ConstBuf::ConstToPixel(4);
	Draw::NullDrawer(1);
	Draw::Present();
}


void Loop() {
	Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
	Camera::update();
	frameConst();
	Textures::RenderTarget(0, 0);
	Draw::Clear({ 0,0,0,0 });
	Draw::ClearDepth();

	ProcessCreatures(population);
	//ShowGrow();
	ShowRacketAndBall();
    
	Showpopulations();
	UpdateAllGrass();
	
	mouse();
	mouse2();

	Textures::TextureToShader(10, 0, pixel);
	//вода

	


	//рельеф
	Shaders::vShader(3);
	Shaders::pShader(3);
	Textures::RenderTarget(0, 0);
	ConstBuf::global[0] = XMFLOAT4((float)CHUNKS_PER_SIDEX, (float)CHUNKS_PER_SIDEY, 0, 0);
	ConstBuf::global[1] = XMFLOAT4(base_rangex, base_rangey, 0, 0);
	ConstBuf::ConstToVertex(5);
	ConstBuf::Update(ConstBuf::getbyname::global, ConstBuf::global);
	Textures::TextureToShader(10, 0, vertex);
	Draw::NullDrawer(32768/2);

	Depth::Depth(Depth::depthmode::off);
	Textures::RenderTarget(0, 0);
	//Depth::SetWaterRasterizer();
	Shaders::vShader(4);
	Shaders::pShader(4);

	Draw::NullDrawer(1);
	//Depth::ResetRasterizer();
	Depth::Depth(Depth::depthmode::on);
	Draw::Present();
}
