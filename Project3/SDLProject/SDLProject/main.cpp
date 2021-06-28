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

#define PLATFORM_COUNT 6

enum GameStatus { MENU, ACTIVE, WIN, LOSE };

struct GameState {
    Entity *player;
    Entity *platforms;
    Entity *missionPlatform;
    GLuint fontTextureID;
    GameStatus gameStatus;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix, modelMatrix;

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

void Restart() {
    state.player->position = glm::vec3(0, 4.0f, 0);
    state.player->isActive = GAMEON;
    state.player->velocity = glm::vec3(0);
    state.player->movement = glm::vec3(0);
    state.player->energy = 250;
    
    float missionPlatformX = rand() % 6;
    missionPlatformX = missionPlatformX - 3;
    state.platforms[5].position = glm::vec3(missionPlatformX, -3.2f, 0.0f);
    state.platforms[5].Update(0, NULL, 0);
    
    state.gameStatus = ACTIVE;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Lunar Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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
    
    state.gameStatus = MENU;
    
    GLuint spriteSheet = LoadTexture("Assets/spritesheet.png");
    
    // INITIALIZE PLAYER
    
    state.player = new Entity();
    
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0, 4.0f, 0);
    state.player->width = 0.5f;
    state.player->height = 0.5f;
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -0.1f, 0);
    state.player->speed = 0.25f;
    state.player->energy = 250;
    state.player->textureID = spriteSheet;
    
    state.player->animLeft = new int[3] {1, 2, 4};
    state.player->animDown = new int[3] {5, 6, 8};
    state.player->animRight = new int[3] {9, 10, 3};
    state.player->animStop = new int[3] {11, 11, 11};

    state.player->animIndices = state.player->animDown;
    state.player->animFrames = 3;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 3;
    
    // INITIALIZE DEAD PLATFORMS
    
    state.platforms = new Entity[PLATFORM_COUNT];
    
    GLuint platformTextureID = LoadTexture("Assets/individual-textures/dead_tile.png");
    
    state.platforms[0].entityType = PLATFORM;
    state.platforms[0].textureID = platformTextureID;
    state.platforms[0].position = glm::vec3(0.0f, -3.25f, 0.0f);
    state.platforms[0].width = 10.0f;
    state.platforms[0].repeat = true;
    
    state.platforms[1].entityType = PLATFORM;
    state.platforms[1].textureID = platformTextureID;
    state.platforms[1].position = glm::vec3(-4.5f, 0.0f, 0.0f);
    state.platforms[1].height = 10.0f;
    state.platforms[1].repeat = true;
    state.platforms[1].rotate = true;
    
    state.platforms[2].entityType = PLATFORM;
    state.platforms[2].textureID = platformTextureID;
    state.platforms[2].position = glm::vec3(4.5f, 0.0f, 0.0f);
    state.platforms[2].height = 10.0f;
    state.platforms[2].repeat = true;
    state.platforms[2].rotate = true;
    
    state.platforms[3].entityType = PLATFORM;
    state.platforms[3].textureID = platformTextureID;
    state.platforms[3].position = glm::vec3(-2.0f, 2.0f, 0.0f);
    state.platforms[3].width = 5.0f;
    state.platforms[3].repeat = true;
    
    state.platforms[4].entityType = PLATFORM;
    state.platforms[4].textureID = platformTextureID;
    state.platforms[4].position = glm::vec3(2.5f, -0.75f, 0.0f);
    state.platforms[4].width = 3.0f;
    state.platforms[4].repeat = true;
    
    // INITIALIZE MISSION PLATFORM IN A RANDOM SPOT
    
    GLuint missionPlatformTextureID = LoadTexture("Assets/individual-textures/safe_tile.png");
    
    float missionPlatformX = rand() % 6;
    missionPlatformX = missionPlatformX - 3;
    
    state.platforms[5].entityType = SUCCESS;
    state.platforms[5].textureID = missionPlatformTextureID;
    state.platforms[5].position = glm::vec3(missionPlatformX, -3.2f, 0.0f);
    state.platforms[5].width = 2.0f;
    state.platforms[5].repeat = true;
    
    // INITIALIZE TEXT
    
    state.fontTextureID = LoadTexture("Assets/pixel_font.png");
    
    // UPDATE PLATFORMS
    
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Update(0, NULL, 0);
    }
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        if (state.gameStatus != ACTIVE) {
                            if (state.gameStatus != MENU) {
                                Restart();
                            }
                            state.gameStatus = ACTIVE;
                        }
                        break;
//                    case SDLK_RIGHT:
//                        state.player->animIndices = state.player->animDown;
//                        break;
//                    case SDLK_LEFT:
//                        state.player->animIndices = state.player->animDown;
//                        break;
                }
        }
    }
    
    state.player->movement = glm::vec3(0);
    
    // key inputs
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_LEFT] && state.player->energy > 0 && state.gameStatus == ACTIVE) {
        state.player->movement.x = 1.0f;
        state.player->energy -= 1;
        state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT] && state.player->energy > 0 && state.gameStatus == ACTIVE) {
        state.player->movement.x = -1.0f;
        state.player->energy -= 1;
        state.player->animIndices = state.player->animRight;
    }
//    if (glm::length(state.player->movement) > 1.0f) {
//        state.player->movement = glm::normalize(state.player->movement);
//    }
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    deltaTime += accumulator;
    
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    
    while (deltaTime >= FIXED_TIMESTEP) {
        if (state.gameStatus == ACTIVE) {
            state.player->Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT);
        }
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;
    
    if (state.player->isActive == GAMEOVER){
        state.gameStatus = LOSE;
        state.player->animIndices = state.player->animStop;
    } else if (state.player->isActive == COMPLETE){
        state.gameStatus = WIN;
        state.player->animIndices = state.player->animStop;
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        if (state.platforms[i].entityType == PLATFORM || state.gameStatus != MENU) {
            state.platforms[i].Render(&program);
        }
    }
    
    if (state.gameStatus != MENU) {
        
        state.player->Render(&program);
        
        DrawText(&program, state.fontTextureID, "Fuel: " + std::to_string(state.player->energy), 0.25f, 0.0f, glm::vec3(-4.5f, 3.3, 0));
        
        if (state.gameStatus == WIN) {
            DrawText(&program, state.fontTextureID, "Mission Complete!", 0.55f, 0.0f, glm::vec3(-4.0f, 1.0f, 0));
            DrawText(&program, state.fontTextureID, "Press Space", 0.5f, 0.0f, glm::vec3(-3.5f, 0.0f, 0));
            DrawText(&program, state.fontTextureID, "to Start Over", 0.5f, 0.0f, glm::vec3(-3.5f, -1.0f, 0));
        } else if (state.gameStatus == LOSE) {
            DrawText(&program, state.fontTextureID, "Mission Failed!", 0.55f, 0.0f, glm::vec3(-4.0f, 1.0f, 0));
            DrawText(&program, state.fontTextureID, "Press Space", 0.5f, 0.0f, glm::vec3(-3.5f, 0.0f, 0));
            DrawText(&program, state.fontTextureID, "to Start Over", 0.5f, 0.0f, glm::vec3(-3.5f, -1.0f, 0));
        }
    } else {
        DrawText(&program, state.fontTextureID, "Press Space", 0.5f, 0.0f, glm::vec3(-3.5f, 1.0f, 0));
        DrawText(&program, state.fontTextureID, "to Begin", 0.5f, 0.0f, glm::vec3(-3.5f, 0.0f, 0));
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
