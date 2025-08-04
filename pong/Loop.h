
void mainLoop()
{
	frameConst();

	InputAssembler::IA(InputAssembler::topology::triList);
	Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);

	Textures::RenderTarget(0, 0);
	Draw::Clear({ 0,0,0,0 });
	Draw::ClearDepth();
	Depth::Depth(Depth::depthmode::off);
	Rasterizer::Cull(Rasterizer::cullmode::front);
	Camera::update();

	ConstBuf::ConstToVertex(4);
	ConstBuf::ConstToPixel(4);

	Shaders::vShader(1);     // фон
	Shaders::pShader(1);
	Draw::NullDrawer(1, 1);

	
	
	Shaders::vShader(0);
	Shaders::pShader(0);
	UpdateChunks();
	ProcessCreatures(population);
	ShowRacketAndBall();
	Showpopulations();
	
	Draw::Present();
}
