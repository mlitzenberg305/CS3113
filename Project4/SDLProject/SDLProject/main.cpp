#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include <iostream>
#include <vector>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"

#define GROUND_PLATFORM 11
#define FLOAT_PLATFORM 3
#define MOVING_PLATFORM 2
#define TREES 14
#define ENEMIES 3

#define OTHERS (GROUND_PLATFORM + FLOAT_PLATFORM + MOVING_PLATFORM + TREES)

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix, modelMatrix;

enum GameStatus {MENU, ACTIVE, WIN, LOSE};

struct GameState {
    Entity *player;
    Entity *enemies;
    Entity *others;
    GLuint fontTextureID;
    GameStatus gameStatus;
};

GameState state;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}

void DrawText(ShaderProgram *program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position) {
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    
    std::vector<float> vertices;
    std::vector<float> texCoords;
    
    for(int i = 0; i < text.size(); i++) {
        
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        
        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
        });
        
        texCoords.insert(texCoords.end(), {
            u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height,
        });
    }
    
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    
    glUseProgram(program->programID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Rise of the AI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    glUseProgram(program.programID);
    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_BLEND);
    // transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    modelMatrix = glm::mat4(1.0f);
    
    GLuint spriteSheet = LoadTexture("Assets/kenney_pixelplatformer/Tilemap/characters_packed.png");
    GLuint tileTextureID = LoadTexture("Assets/kenney_pixelplatformer/Tilemap/tiles_packed.png");
    
    // INITIALIZE PLAYER
    state.player = new Entity();
    
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(-4.0f, 2.0f, 0);
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
    
    // INITIALIZE OTHERS
    
    state.others = new Entity[OTHERS];
    
    // GROUND PLATFORMS
    
    for (int i = 0; i < GROUND_PLATFORM; i++) {
        state.others[i].entityType = WALL;
        state.others[i].textureID = tileTextureID;
        state.others[i].position = glm::vec3((float)(i - (GROUND_PLATFORM / 2)), -3.25f, 0.0f);
        state.others[i].animStop = new int[1] {22};
        
        state.others[i].animIndices = state.others[i].animStop;
        state.others[i].animFrames = 1;
        state.others[i].animIndex = 0;
        state.others[i].animTime = 0;
        state.others[i].animCols = 20;
        state.others[i].animRows = 9;
        
        state.others[i].isActive = true;
    }
    
    // FLOATING PLATFORMS
    
    for (int i = GROUND_PLATFORM; i < (FLOAT_PLATFORM + GROUND_PLATFORM); i++) {
        state.others[i].entityType = WALL;
        state.others[i].textureID = tileTextureID;
        state.others[i].position = glm::vec3((float)(i - GROUND_PLATFORM - 3), -1.0f, 0.0f);
        state.others[i].animStop = new int[1] {1 + (i - GROUND_PLATFORM)};
        
        state.others[i].animIndices = state.others[i].animStop;
        state.others[i].animFrames = 1;
        state.others[i].animIndex = 0;
        state.others[i].animTime = 0;
        state.others[i].animCols = 20;
        state.others[i].animRows = 9;
        
        state.others[i].isActive = true;
    }
    // CLOUD PLATFORMS
    
    for (int i = (GROUND_PLATFORM + FLOAT_PLATFORM); i < (FLOAT_PLATFORM + GROUND_PLATFORM + MOVING_PLATFORM); i++) {
        state.others[i].entityType = WALL;
        state.others[i].textureID = tileTextureID;
        state.others[i].position = glm::vec3((float)(i - (GROUND_PLATFORM + FLOAT_PLATFORM)) + 2.0f, 0.0f, 0.0f);
        // state.others[i].velocity.y = 1.0f;
        if (MOVING_PLATFORM > 2) {
            
            state.others[i].animStop = new int[1] {153 + i - (GROUND_PLATFORM + FLOAT_PLATFORM)};
            
        } else if (MOVING_PLATFORM == 2) {
            
            if (i == (GROUND_PLATFORM + FLOAT_PLATFORM)) {
                state.others[i].animStop = new int[1] {153};
            } else if (i == (GROUND_PLATFORM + FLOAT_PLATFORM) + 1) {
                state.others[i].animStop = new int[1] {155};
            }
            
        }
        
        state.others[i].animIndices = state.others[i].animStop;
        state.others[i].animFrames = 1;
        state.others[i].animIndex = 0;
        state.others[i].animTime = 0;
        state.others[i].animCols = 20;
        state.others[i].animRows = 9;
        
        // state.others[i].AImove = true;
        state.others[i].isActive = true;
    }
    
    // TREES
    
    for (int i = (GROUND_PLATFORM + FLOAT_PLATFORM + MOVING_PLATFORM); i < (FLOAT_PLATFORM + GROUND_PLATFORM + MOVING_PLATFORM + TREES); i++) {
        state.others[i].entityType = WALL;
        state.others[i].textureID = tileTextureID;
        if (i - (GROUND_PLATFORM + FLOAT_PLATFORM + MOVING_PLATFORM) < TREES / 2) {
            
            state.others[i].position = glm::vec3(-5.0f, -2.25f + i - (GROUND_PLATFORM + FLOAT_PLATFORM + MOVING_PLATFORM), 0.0f);
            
        } else {
            
            state.others[i].position = glm::vec3(5.0f, -2.25f + i - ((TREES / 2) + GROUND_PLATFORM + FLOAT_PLATFORM + MOVING_PLATFORM), 0.0f);
            
        }
        state.others[i].animStop = new int[1] {56};
        state.others[i].animIndices = state.others[i].animStop;
        state.others[i].animFrames = 1;
        state.others[i].animIndex = 0;
        state.others[i].animTime = 0;
        state.others[i].animCols = 20;
        state.others[i].animRows = 9;
        
        state.others[i].isActive = true;
    }
    
    // INITIALIZE ENEMIES
    
    state.enemies = new Entity[ENEMIES];
    
    for (int i = 0; i < ENEMIES; i++) {
        
        state.enemies[i].aiState = IDLE;
        
        if (i == 0) {
            state.enemies[i].entityType = BUG;
            
            state.enemies[i].animLeft = new int[2] {21, 22};
            state.enemies[i].animRight = new int[2] {21, 22};
            state.enemies[i].animUp = new int[2] {20, 20};
            
            state.enemies[i].animFrames = 2;
            state.enemies[i].animIndices = state.enemies[i].animLeft;
            
            state.enemies[i].position = glm::vec3(3.0f, -1.0f, 0);
            state.enemies[i].acceleration = glm::vec3(0, -9.8f, 0);
            
        } else if (i == 1) {
            state.enemies[i].entityType = FLYING;
            
            state.enemies[i].animUp = new int[3] {24, 25, 26};
            
            state.enemies[i].animFrames = 3;
            state.enemies[i].animIndices = state.enemies[i].animUp;
            
            state.enemies[i].position = glm::vec3(0.5f, 3.0f, 0);
            
        } else if (i == 2) {
            state.enemies[i].entityType = POINTY;
            
            state.enemies[i].animLeft = new int[2] {15, 16};
            state.enemies[i].animRight = new int[2] {15, 16};
            state.enemies[i].animUp = new int[2] {17, 17};
            
            state.enemies[i].animFrames = 2;
            state.enemies[i].animIndices = state.enemies[i].animLeft;
            
            state.enemies[i].position = glm::vec3(-2.5f, 3.0f, 0);
            state.enemies[i].acceleration = glm::vec3(0, -9.8f, 0);
        }
        
        state.enemies[i].width = 1.0f;
        state.enemies[i].height = 1.0f;

        state.enemies[i].speed = 0.5f;
        state.enemies[i].energy = 1;
        state.enemies[i].textureID = spriteSheet;

        state.enemies[i].animIndex = 0;
        state.enemies[i].animTime = 0;
        state.enemies[i].animCols = 9;
        state.enemies[i].animRows = 3;
        
        state.enemies[i].isActive = true;
    }
    
    // INITIALIZE TEXT
    
    state.fontTextureID = LoadTexture("Assets/pixel_font.png");
    
    // UPDATE PLATFORMS
    for (int i = 0; i < OTHERS; i++) {
        if (state.others[i].entityType == WALL) {
            state.others[i].Update(0, NULL, NULL, 0, NULL, 0);
        }
    }
    
    // SET GAME STATE
    state.gameStatus = MENU;
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        if (state.gameStatus == MENU) {
                            state.gameStatus = ACTIVE;
                        } else if (state.gameStatus == ACTIVE){
                            if (state.player->collidedBottom) {
                                state.player->jump = true;
                            }
                        }
                        break;
                    case SDLK_RIGHT:
                        break;
                    case SDLK_LEFT:
                        break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        // state.player->animIndices = state.player->animLeft;
                        break;
                    case SDLK_RIGHT:
                        break;
                    case SDLK_LEFT:
                        break;
                }
                break;
        }
    }
    
    // key inputs
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    state.player->movement = glm::vec3(0);
    
    if (keys[SDL_SCANCODE_LEFT]) {
        state.player->movement.x = -1.0f;
        if (state.player->collidedBottom) {
            state.player->animIndices = state.player->animLeft;
        }
        
    } else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->movement.x = 1.0f;
        if (state.player->collidedBottom) {
            state.player->animIndices = state.player->animRight;
        }
        
    }
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    if (state.gameStatus != ACTIVE) return;
    
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    deltaTime += accumulator;
    int activeEnemies = ENEMIES;
    
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    
    while (deltaTime >= FIXED_TIMESTEP) {
        state.player->Update(FIXED_TIMESTEP, state.player, state.others, OTHERS, state.enemies, ENEMIES);
        for (int i = 0; i < ENEMIES; i++) {
            state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.others, OTHERS, state.enemies, ENEMIES);
            if (state.enemies[i].isActive == false) {
                activeEnemies --;
            }
        }
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;
    if (!state.player->isActive) {
        state.gameStatus = LOSE;
    } else if (activeEnemies == 0) {
        state.gameStatus = WIN;
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    for (int i = 0; i < OTHERS; i++) {
        state.others[i].Render(&program);
    }
    
    // DrawText(&program, state.fontTextureID, "Health: " + std::to_string(state.player->energy), 0.25f, 0.0f, glm::vec3(-4.25f, 3.3, 0));
    switch (state.gameStatus) {
        case MENU:
            DrawText(&program, state.fontTextureID, "Press Space", 0.5f, 0.0f, glm::vec3(-2.0f, 1.0f, 0));
            break;
        case ACTIVE:
            DrawText(&program, state.fontTextureID, "Kill all the AI!", 0.25f, 0.0f, glm::vec3(-3.25f, 3.3, 0));
            
            for (int i = 0; i < ENEMIES; i++) {
                state.enemies[i].Render(&program);
            }
            
            state.player->Render(&program);
            break;
        case WIN:
            for (int i = 0; i < ENEMIES; i++) {
                state.enemies[i].Render(&program);
            }
            
            state.player->Render(&program);
            DrawText(&program, state.fontTextureID, "You Win!", 0.5f, 0.0f, glm::vec3(-1.8f, 1.0f, 0));
            break;
        case LOSE:
            for (int i = 0; i < ENEMIES; i++) {
                state.enemies[i].Render(&program);
            }
            
            state.player->Render(&program);
            DrawText(&program, state.fontTextureID, "You Lose!", 0.5f, 0.0f, glm::vec3(-1.8f, 1.0f, 0));
            break;
    }
    
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
