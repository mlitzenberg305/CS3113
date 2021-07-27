#include "Level3.h"

#define LEVEL_WIDTH 50
#define LEVEL_HEIGHT 8

#define LEVEL_ENEMY_COUNT 1

unsigned int level3_data[] =
{
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    6, 153, 154, 155, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 41, 42, 43, 0, 0, 0, 0, 0, 0, 0, 0, 153, 154, 155, 0, 0, 0, 0, 0, 40, 0, 0, 40, 0, 0, 0, 0, 60,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 146, 0, 0, 0, 0, 0, 0, 61, 63, 0, 146, 0, 0, 0, 0, 0, 0, 61, 63, 0, 0, 146, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    6, 0, 0, 0, 0, 0, 41, 42, 43, 0, 0, 0, 60, 33, 33, 33, 33, 141, 143, 33, 33, 33, 33, 33, 33, 33, 33, 121, 24, 63, 0, 0, 0, 0, 0, 0, 0, 0, 153, 155, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    6, 0, 0, 146, 68, 68, 0, 0, 0, 0, 0, 68, 120, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 121, 122, 123, 68, 68, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    6, 61, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 123, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 121, 122, 24, 62, 62, 62, 62, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

void Level3::Initialize() {
    
    state.nextScene = -1;
    
    GLuint spriteSheet = Util::LoadTexture("Assets/kenney_pixelplatformer/Tilemap/characters_packed.png");
    GLuint mapTextureID = Util::LoadTexture("Assets/kenney_pixelplatformer/Tilemap/tiles_packed.png");

    state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, level3_data, mapTextureID, 1.0f, 20, 9);
    
    // INITIALIZE PLAYER
    state.player = new Entity();
    
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(1.0f, 0.0f, 0);
    state.player->width = 0.8f;
    state.player->height = 1.0f;
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.8f, 0);
    state.player->speed = 2.0f;
    state.player->jumpPower = 5.5f;
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

void Level3::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL_ENEMY_COUNT, state.map);
}

void Level3::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
}
