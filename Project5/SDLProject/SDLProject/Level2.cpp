#include "Level2.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

#define LEVEL_ENEMY_COUNT 1

unsigned int level2_data[] =
{
        47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        47, 0, 0, 0, 153, 154, 155, 0, 0, 0, 0, 0, 0, 0,
        47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        47, 101, 102, 103, 0, 0, 0, 101, 102, 102, 102, 102, 102, 103,
        47, 121, 122, 123, 0, 0, 0, 121, 122, 122, 122, 122, 122, 123,
};

void Level2::Initialize() {
    
    state.nextScene = -1;
    
    GLuint spriteSheet = Util::LoadTexture("Assets/kenney_pixelplatformer/Tilemap/characters_packed.png");
    GLuint mapTextureID = Util::LoadTexture("Assets/kenney_pixelplatformer/Tilemap/tiles_packed.png");

    state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, level2_data, mapTextureID, 1.0f, 20, 9);
    
    // INITIALIZE PLAYER
    state.player = new Entity();
    
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(1.0f, 0.0f, 0);
    state.player->width = 0.8f;
    state.player->height = 1.0f;
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.8f, 0);
    state.player->speed = 2.0f;
    state.player->jumpPower = 7.0f;
    state.player->energy = 3;
    state.player->textureID = spriteSheet;
    
    state.player->animLeft = new int[2] {0, 1};
    state.player->animRight = new int[2] {0, 1};
    state.player->animUp = new int[2] {1, 0};

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 2;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 9;
    state.player->animRows = 3;
    
    state.player->isActive = true;
    
    // add enemies
}

void Level2::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL_ENEMY_COUNT, state.map);
}

void Level2::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
}
