static int lastTick = -1;
int colorType = 1;
struct DiamondSquare {
    int size;
    float* heightmap;
    float roughness;

    DiamondSquare(int gridSize, float rough = 0.5f) : size(gridSize), roughness(rough) {
        heightmap = new float[size * size];
        memset(heightmap, 0, size * size * sizeof(float));
    }

    ~DiamondSquare() {
        delete[] heightmap;
    }

    float& at(int x, int y) {
        x = (x + size) % size;
        y = (y + size) % size;
        return heightmap[y * size + x];
    }

    void normalize() {
        float minH = 1.0f, maxH = 0.0f;

        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                minH = min(minH, at(x, y));
                maxH = max(maxH, at(x, y));
            }
        }

        float range = maxH - minH;
        if (range > 0) {
            for (int y = 0; y < size; y++) {
                for (int x = 0; x < size; x++) {
                    at(x, y) = (at(x, y) - minH) / range;
                }
            }
        }
    }

    void applyErosion(float strength = 0.3f, int iterations = 3) {
        for (int iter = 0; iter < iterations; iter++) {
            std::vector<float> erosion(size * size, 0.0f);

            for (int y = 1; y < size - 1; y++) {
                for (int x = 1; x < size - 1; x++) {
                    float dx = at(x + 1, y) - at(x - 1, y);
                    float dy = at(x, y + 1) - at(x, y - 1);
                    float slope = std::sqrt(dx * dx + dy * dy);

                    erosion[y * size + x] = slope * strength;
                }
            }

            for (int y = 1; y < size - 1; y++) {
                for (int x = 1; x < size - 1; x++) {
                    at(x, y) = max(0.0f, at(x, y) - erosion[y * size + x]);
                }
            }
        }
    }

    void applyRealisticCurve() {
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                float height = at(x, y);
                if (height < 0.3f) {
                    at(x, y) = std::pow(height / 0.3f, 1.5f) * 0.3f;
                }
                else if (height < 0.4f) {
                    at(x, y) = 0.3f + (height - 0.3f) * 0.7f;
                }
                else if (height < 0.7f) {
                    at(x, y) = 0.37f + (height - 0.4f) * 0.8f;
                }
                else {
                    at(x, y) = 0.61f + std::pow((height - 0.7f) / 0.3f, 0.6f) * 0.39f;
                }
            }
        }
    }

    void generateRealistic() {
        at(0, 0) = Random::Float(0, 0.2f);        
        at(0, size - 1) = Random::Float(0.1f, 0.3f);
        at(size - 1, 0) = Random::Float(0.1f, 0.3f);
        at(size - 1, size - 1) = Random::Float(0.3f, 0.5f); 

        int step = size - 1;
        float scale = 0.8f;

        while (step > 1) {
            int half = step / 2;

            for (int y = 0; y < size - 1; y += step) {
                for (int x = 0; x < size - 1; x += step) {
                    float avg = (at(x, y) + at(x + step, y) +
                        at(x, y + step) + at(x + step, y + step)) * 0.25f;
                    at(x + half, y + half) = avg + Random::Float(-1, 1) * scale;
                }
            }

            for (int y = 0; y < size; y += half) {
                for (int x = (y + half) % step; x < size; x += step) {
                    if ((x % step == half) && (y % step == half)) continue;

                    float sum = at((x - half + size) % size, y) +
                        at((x + half) % size, y) +
                        at(x, (y - half + size) % size) +
                        at(x, (y + half) % size);

                    at(x, y) = (sum / 4) + Random::Float(-1, 1) * scale;
                }
            }

            step = half;
            scale *= roughness;

            for (int x = 0; x < size; x++) {
                at(x, 0) = at(x, size - 1);
            }
            for (int y = 0; y < size; y++) {
                at(0, y) = at(size - 1, y);
            }
        }

        normalize();
        applyRealisticCurve();
        applyErosion(0.2f, 2);
        normalize(); 
    }
};

void FillHeightmapBuffer(XMFLOAT4* buffer, int bufferSize, int terrainType = 0) {
    const int gridSize = 65;
    const int totalPoints = gridSize * gridSize;

    DiamondSquare ds(gridSize, 0.65f); 
    ds.generateRealistic();

    for (int i = 0; i < bufferSize; i++) {
        XMFLOAT4& element = buffer[i];

        for (int comp = 0; comp < 4; comp++) {
            int idx = i * 4 + comp;
            float height = (idx < totalPoints) ?
                ds.heightmap[idx] : 0.0f;

            height = max(0.0f, min(1.0f, height));

            switch (comp) {
            case 0: element.x = height; break;
            case 1: element.y = height; break;
            case 2: element.z = height; break;
            case 3: element.w = height; break;
            }
        }
    }
}


void terraloop()
{


	InputAssembler::IA(InputAssembler::topology::triList);
	Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
	frameConst();
	Textures::RenderTarget(1, 0);
	Draw::Clear({ 0,0,0,0 });
	Draw::ClearDepth();
	Depth::Depth(Depth::depthmode::on);
	Rasterizer::Cull(Rasterizer::cullmode::off);
	Shaders::vShader(1);
	Shaders::pShader(1);
	ConstBuf::ConstToVertex(4);
	ConstBuf::ConstToPixel(4);




	ConstBuf::ConstToPixel(5);
	ConstBuf::Update(5, ConstBuf::global);
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
	if (tick - lastTick > 2) {
		lastTick = tick;
		colorType = rand() % 6;

	}
	//switch (colorType) {
	//case 0: Draw::Clear({ 1.0f, 0.0f, 0.0f, 1.0f }); break; // Красный
	//case 1: Draw::Clear({ 0.0f, 1.0f, 0.0f, 1.0f }); break; // Зеленый
	//case 2: Draw::Clear({ 0.0f, 0.0f, 1.0f, 1.0f }); break; // Синий
	//case 3: Draw::Clear({ 1.0f, 0.0f, 1.0f, 1.0f }); break; // Пурпурный
	//case 4: Draw::Clear({ 1.0f, 1.0f, 0.0f, 1.0f }); break; // Желтый
	//case 5: Draw::Clear({ 0.0f, 1.0f, 1.0f, 1.0f }); break; // Голубой
	//}
	Draw::Clear({ 0.0f, 0.0f, 0.0f, 1.0f });
	Draw::ClearDepth();
	Rasterizer::Cull(Rasterizer::cullmode::back);
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
	const int SOURCE_SIZE = 256;
	const int DEST_SIZE = 50;
	const float BLOCK_SIZE = SOURCE_SIZE / (float)DEST_SIZE; // 5.12

	for (int blockX = 0; blockX < DEST_SIZE; blockX++)
		for (int blockY = 0; blockY < DEST_SIZE; blockY++)
		{
			float sum = 0.0f;
			int count = 0;

			// Используем float для точного расчета границ
			int startX = (int)(blockX * BLOCK_SIZE);
			int endX = (int)((blockX + 1) * BLOCK_SIZE);
			int startY = (int)(blockY * BLOCK_SIZE);
			int endY = (int)((blockY + 1) * BLOCK_SIZE);

			for (int x = startX; x < endX; x++)
				for (int y = startY; y < endY; y++)
				{
					if (x < SOURCE_SIZE && y < SOURCE_SIZE)
					{
						ChunkWorld& currentChunk = population.getChunkByIndex(x, y);
						sum += currentChunk.grass_sum;
						count++;
					}
				}

			int index = blockX + blockY * DEST_SIZE;
			ConstBuf::global[index + 2].x =  sum ; // Усредняем!
		}
	ConstBuf::ConstToVertex(5);
	ConstBuf::ConstToPixel(5);
	ConstBuf::Update(5, ConstBuf::global);
	Textures::TextureToShader(1, 0, vertex);
	Draw::NullDrawer(32768*8);
	
	//Depth::Depth(Depth::depthmode::readonly);
	//Textures::RenderTarget(0, 0);
	//Depth::SetWaterRasterizer(); 


	Textures::TextureToShader(1, 0);
	Shaders::vShader(4);
	Shaders::pShader(4);

	ConstBuf::global[0] = XMFLOAT4(600, 0, 0, 0);
	ConstBuf::ConstToVertex(5);
	ConstBuf::Update(ConstBuf::getbyname::global, ConstBuf::global);
	Draw::NullDrawer(1,9);
	//Depth::ResetRasterizer();
	waterLevel = 600;
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

