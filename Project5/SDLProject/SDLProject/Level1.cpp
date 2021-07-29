#include "Level1.h"

#define LEVEL_WIDTH 41
#define LEVEL_HEIGHT 8

#define LEVEL_ENEMY_COUNT 1

unsigned int level1_data[] =
{
        6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
        6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
        6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
        6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 153, 155, 0, 0, 146, 0, 0, 0, 0, 6,
        6, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 153, 155, 0, 0, 0, 0, 0, 0, 0, 0, 0, 110, 0, 6,
        6, 0, 0, 0, 0, 29, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 29, 0, 0, 0, 0, 0, 0, 146, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 130, 0, 6,
        6, 21, 22, 22, 22, 22, 23, 68, 0, 0, 21, 22, 22, 22, 22, 22, 22, 22, 23, 0, 0, 0, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 6,
        6, 121, 122, 122, 122, 122, 24, 22, 22, 22, 25, 122, 122, 122, 122, 122, 122, 122, 123, 0, 0, 0, 121, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 6,
};

void Level1::Initialize() {
    
    state.nextScene = -1;
    
    GLuint spriteSheet = Util::LoadTexture("Assets/kenney_pixelplatformer/Tilemap/characters_packed.png");
    GLuint mapTextureID = Util::LoadTexture("Assets/kenney_pixelplatformer/Tilemap/tiles_packed.png");

    state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, level1_data, mapTextureID, 1.0f, 20, 9);
    
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
    
    state.enemies = new Entity[LEVEL_ENEMY_COUNT];
    
    for (int i = 0; i < LEVEL_ENEMY_COUNT; i++) {
        state.enemies[i].aiState = IDLE;
        state.enemies[i].aiType = WALKER;
        
        state.enemies[i].width = 1.0f;
        state.enemies[i].height = 1.0f;

        state.enemies[i].speed = 1.0f;
        state.enemies[i].energy = 1;
        state.enemies[i].textureID = spriteSheet;

        state.enemies[i].animIndex = 0;
        state.enemies[i].animTime = 0;
        state.enemies[i].animCols = 9;
        state.enemies[i].animRows = 3;
        
        state.enemies[i].animLeft = new int[2] {21, 22};
        state.enemies[i].animRight = new int[2] {21, 22};
        state.enemies[i].animUp = new int[2] {20, 20};
        
        state.enemies[i].animFrames = 2;
        state.enemies[i].animIndices = state.enemies[i].animLeft;
        
        state.enemies[i].position = glm::vec3(27.0f, -1.0f, 0);
        state.enemies[i].acceleration = glm::vec3(0, -9.8f, 0);
        
        state.enemies[i].isActive = false;
    }
}

void Level1::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL_ENEMY_COUNT, state.map);
    for (int i = 0; i < LEVEL_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL_ENEMY_COUNT, state.map);
    }
    if (state.player->energy == 0) {
        state.gameStatus = LOSE;
    }
    if (state.player->position.y < -8.5) {
        state.gameStatus = LOSE;
    }
}

void Level1::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
    for (int i = 0; i < LEVEL_ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }
}
