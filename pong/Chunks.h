struct Chunk {
    std::vector<std::weak_ptr<Creature>> trees;
    std::vector<std::weak_ptr<Creature>> Plants;
    std::vector<std::weak_ptr<Creature>> Animals;
    std::vector<std::weak_ptr<Creature>> grass;
    std::vector<std::weak_ptr<Creature>> rabbits;
    std::vector<std::weak_ptr<Creature>> wolves;
    std::vector<std::weak_ptr<Creature>> bushes;
    std::vector<std::weak_ptr<Creature>> eagles;
    std::vector<std::weak_ptr<Creature>> rats;
    //Grass grass;
    bool water;
    // Поиск ближайшего существа указанного типа
    std::pair<float, float> nearest_creature(type_ creatureType, float x, float y, bool matureOnly,gender_ gender) const {
        switch (creatureType) {
        case type_::rabbit: return nearest_mature_creature(rabbits, x, y, matureOnly,gender);
        case type_::wolf:   return nearest_mature_creature(wolves, x, y, matureOnly, gender);
        case type_::tree:   return nearest_mature_creature(trees, x, y, matureOnly, gender);
        case type_::bush: return nearest_mature_creature(bushes, x, y, matureOnly, gender);
        case type_::eagle: return nearest_mature_creature(eagles, x, y, matureOnly, gender);
        case type_::rat: return nearest_mature_creature(rats, x, y, matureOnly, gender);
        case type_::grass: return nearest_mature_creature(grass, x, y, matureOnly, gender);
        default: return { -5000.0f, -5000.0f };
        }
    }

    template<typename T>
    std::pair<float, float> nearest_mature_creature(
        const std::vector<std::weak_ptr<T>>& creatures,
        float x, float y,
        bool matureOnly,
        gender_ gender
    ) const {
        float best_dx = 0, best_dy = 0;
        float best_dist2 = 100000000;
        bool found = false;

        for (auto& w : creatures) {
            if (auto c = w.lock()) {
                if (matureOnly) {

                    // Проверки только для поиска партнера
                    if (c->age < c->maturity_age ||
                        gender == c->gender ||
                        (tick - c->birth_tick) < 200.0f) {
                        continue;
                    }
                }
                                        
                if (c->blossoming_age != 0) {
                    if (c->berry_count == 0) {
                        return std::make_pair(-5000.0f, -5000.0f);
                    }
                }
                ///

                ///
                float dx = torusDelta(x, c->x, base_rangex);
                float dy = torusDelta(y, c->y, base_rangey);
                float dist2 = dx * dx + dy * dy;

                if (dist2 > 0.0f && dist2 < best_dist2) {
                    best_dx = dx;
                    best_dy = dy;
                    best_dist2 = dist2;
                    found = true;
                }
            }
        }

        return found ? std::make_pair(best_dx, best_dy) : std::make_pair(-5000.0f, -5000.0f);
    }

    // Проверка на близость для избегания (квадратная область)
    template<typename T>
    std::tuple<int, float, float> nearly_creature_square(
        const std::vector<std::weak_ptr<T>>& creatures,
        float x, float y,
        float half_side
    ) const {
        float avoidX = 0, avoidY = 0;
        int nearby = 0;

        for (auto& w : creatures) {
            if (auto c = w.lock()) {
                float dx = torusDelta(x, c->x, base_rangex);
                float dy = torusDelta(y, c->y, base_rangey);

                if (std::abs(dx) < half_side && std::abs(dy) < half_side) {
                    avoidX += -dx;
                    avoidY += -dy;
                    ++nearby;
                }
            }
        }
        return { nearby, avoidX, avoidY };
    }

    template<typename T>
    int countCreatures(const std::vector<std::weak_ptr<T>>& creatures) const {
        int count = 0;
        for (auto& w : creatures) if (!w.expired()) ++count;
        return count;
    }

    //bool Grassneightboor() {
    //    return grass.growth > 80;
    //}
    //
    //void UpdateGrassGrowth(int x, int y) {
    //    float growthSpeed = 1.0f;
    //    int N = 0;
    //    if (grass.growth <= 0) {
    //        // Получаем мировые координаты центра текущего чанка
    //        float world_center_x = (x + 0.5f) * CHUNK_SIZE - base_rangex;
    //        float world_center_y = (y + 0.5f) * CHUNK_SIZE - base_rangey;
    //
    //        for (int i = -1; i <= 1; ++i) {
    //            for (int j = -1; j <= 1; ++j) {
    //                // Пропускаем текущий чанк
    //                if (i == 0 && j == 0) continue;
    //                if (i == 1 && j == 1) continue;
    //                if (i == -1 && j == -1) continue;
    //                if (i == -1 && j == 1) continue;
    //                if (i == 1 && j == -1) continue;
    //                // Вычисляем мировые координаты соседнего чанка
    //                float neighbor_x = world_center_x + i * CHUNK_SIZE;
    //                float neighbor_y = world_center_y + j * CHUNK_SIZE;
    //
    //                // Оборачиваем координаты, если они выходят за границы мира
    //                neighbor_x = Wrap(neighbor_x, base_rangex);
    //                neighbor_y = Wrap(neighbor_y, base_rangey);
    //
    //                // Преобразуем мировые координаты в индексы чанков
    //                int ncx = coord_to_chunkx(neighbor_x);
    //                int ncy = coord_to_chunky(neighbor_y);
    //
    //                // Проверяем, что индексы в пределах массива
    //                if (ncx >= 0 && ncx < CHUNKS_PER_SIDEX &&
    //                    ncy >= 0 && ncy < CHUNKS_PER_SIDEY) {
    //
    //                    if (chunk_grid[ncx][ncy].Grassneightboor()) {
    //                        N += 1;
    //                    }
    //                }
    //            }
    //        }
    //
    //        grass.growth += growthSpeed * N;
    //        return;
    //    }
    //
    //    if (grass.growth < grass.maxGrowth && grass.growth>0) grass.growth += growthSpeed;
    //
    //}
};
// Функция для проверки и уничтожения существ в радиусе от курсора
void kill_creatures_in_radius(float center_x, float center_y, float radius) {
    // Определяем, какие чанки попадают в радиус
    int center_cx = coord_to_chunkx(center_x);
    int center_cy = coord_to_chunky(center_y);

    // Вычисляем количество чанков, которые нужно проверить
    int chunk_radius = static_cast<int>(ceil(radius / CHUNK_SIZE)) + 1;

    // Проходим по всем чанкам в области
    for (int dx = -chunk_radius; dx <= chunk_radius; ++dx) {
        for (int dy = -chunk_radius; dy <= chunk_radius; ++dy) {
            // Вычисляем координаты чанка с учетом тороидальности
            int cx = (center_cx + dx + CHUNKS_PER_SIDEX) % CHUNKS_PER_SIDEX;
            int cy = (center_cy + dy + CHUNKS_PER_SIDEY) % CHUNKS_PER_SIDEY;

            // Получаем ссылку на чанк
            Chunk& chunk = chunk_grid[cx][cy];

            // Проверяем все типы существ в чанке
            auto check_creatures = [&](auto& creatures) {
                for (auto& weak_creature : creatures) {
                    if (auto creature = weak_creature.lock()) {
                        // Проверяем расстояние до курсора с учетом тороидальности
                        float dx = torusDelta(center_x, creature->x, base_rangex);
                        float dy = torusDelta(center_y, creature->y, base_rangey);
                        float distance_sq = dx * dx + dy * dy;

                        // Если существо в радиусе, помечаем его как мертвое
                        if (distance_sq <= radius * radius) {
                            creature->dead = true;
                        }
                    }
                }
                };

            // Проверяем все типы существ
            check_creatures(chunk.trees);
            check_creatures(chunk.rabbits);
            check_creatures(chunk.wolves);
            check_creatures(chunk.bushes);
            check_creatures(chunk.eagles);
            check_creatures(chunk.rats);
        }
    }
}
// возвращает абсолютные координаты ближайшего существа или (-5000,-5000) если не найдено
std::pair<float, float> searchNearestCreature(
    float x, float y,
    type_ creatureType,
    int max_chunk_radius,
    bool matureOnly,
    gender_ gender
) {
    int center_cx = coord_to_chunkx(x);
    int center_cy = coord_to_chunky(y);

    float best_dx = 0.0f, best_dy = 0.0f;
    float best_dist2 = 1e18f;
    bool found = false;

    auto checkChunk = [&](const Chunk& chunk) {
        auto p = chunk.nearest_creature(creatureType, x, y, matureOnly,gender);
        float dx = p.first;
        float dy = p.second;

        if (dx != -5000.0f) {
            float dist2 = dx * dx + dy * dy;
            if (dist2 < best_dist2) {
                best_dist2 = dist2;
                best_dx = dx;
                best_dy = dy;
                found = true;
            }
        }
        };

    // Проверяем центр
    checkChunk(chunk_grid[center_cx][center_cy]);

    // Кольца вокруг центра
    for (int ring = 1; ring <= max_chunk_radius; ++ring) {
        int R = CHUNK_SIZE * ring;
        float angle_step = 360.0f / (8 * ring);

        for (float angle = 0.0f; angle < 360.0f; angle += angle_step) {
            float rad = angle * (PI / 180.0f);
            int dotX = Wrap(x + R * cos(rad), base_rangex);
            int dotY = Wrap(y + R * sin(rad), base_rangey);
            int cx = coord_to_chunkx(dotX);
            int cy = coord_to_chunky(dotY);

            checkChunk(chunk_grid[cx][cy]);
            if (found) {
                float targetX = Wrap(x + best_dx, base_rangex);
                float targetY = Wrap(y + best_dy, base_rangey);
                return { targetX, targetY };
            }


        }
    }

    if (!found)
        return { -5000.0f, -5000.0f };

    float targetX = Wrap(x + best_dx, base_rangex);
    float targetY = Wrap(y + best_dy, base_rangey);
    return { targetX, targetY };
}

