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
#define FLOAT_PLATFORM 2
#define MOVING_PLATFORM 3
#define ENEMIES 3

#define OTHERS 4

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix, modelMatrix;

enum GameStatus {MENU, ACTIVE, WIN, LOSE};

struct GameState {
    Entity *player;
    Entity *ground;
    Entity *floating;
    Entity *moving;
    Entity *enemies;
    Entity *not_player;
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
    state.player->position = glm::vec3(0, 4.0f, 0);
    state.player->width = 1.0f;
    state.player->height = 1.0f;
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.8f, 0);
    state.player->speed = 1.0f;
    state.player->jumpPower = 5.0f;
    state.player->energy = 300;
    state.player->textureID = spriteSheet;
    
    state.player->animLeft = new int[2] {0, 1};
    state.player->animRight = new int[2] {0, 1};
    state.player->animUp = new int[2] {1, 0};

    state.player->animIndices = state.player->animLeft;
    state.player->animFrames = 2;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 9;
    state.player->animRows = 3;
    
    state.player->isActive = true;
    
    // INITIALIZE ENEMIES
    
    // INITIALIZE GROUND PLATFORMS
    
    state.ground = new Entity[GROUND_PLATFORM];
    
    for (int i = 0; i < GROUND_PLATFORM; i++) {
        state.ground[i].entityType = WALL;
        state.ground[i].textureID = tileTextureID;
        state.ground[i].position = glm::vec3((float)(i - (GROUND_PLATFORM / 2)), -3.25f, 0.0f);
        state.ground[i].animStop = new int[1] {22};
        
        state.ground[i].animIndices = state.ground[i].animStop;
        state.ground[i].animFrames = 1;
        state.ground[i].animIndex = 0;
        state.ground[i].animTime = 0;
        state.ground[i].animCols = 20;
        state.ground[i].animRows = 9;
        
        state.ground[i].isActive = true;
    }
    
    // INITIALIZE FLOATING PLATFORMS
    
    state.floating = new Entity[FLOAT_PLATFORM];
    
    for (int i = 0; i < FLOAT_PLATFORM; i++) {
        state.floating[i].entityType = WALL;
        state.floating[i].textureID = tileTextureID;
        state.floating[i].position = glm::vec3((float)(i - 2), -1.5f, 0.0f);
        state.floating[i].animStop = new int[1] {0};
        
        state.floating[i].animIndices = state.ground[i].animStop;
        state.floating[i].animFrames = 1;
        state.floating[i].animIndex = 0;
        state.floating[i].animTime = 0;
        state.floating[i].animCols = 20;
        state.floating[i].animRows = 9;
        
        state.floating[i].isActive = true;
    }
    
    // INITIALIZE TEXT
    
    state.fontTextureID = LoadTexture("Assets/pixel_font.png");
    
    // UPDATE GROUND PLATFORMS
    for (int i = 0; i < GROUND_PLATFORM; i++) {
        state.ground[i].Update(0, NULL, 0);
    }
    
    // UPDATE FLOATING PLATFORMS
    for (int i = 0; i < FLOAT_PLATFORM; i++) {
        state.floating[i].Update(0, NULL, 0);
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
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        if (state.player->collidedBottom) {
                            state.player->jump = true;
                            state.player->animIndices = state.player->animUp;
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
                        state.player->animIndices = state.player->animLeft;
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
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    deltaTime += accumulator;
    
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    
    while (deltaTime >= FIXED_TIMESTEP) {
        state.player->Update(FIXED_TIMESTEP, state.ground, GROUND_PLATFORM);
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;
    
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    for (int i = 0; i < GROUND_PLATFORM; i++) {
        state.ground[i].Render(&program);
    }
    for (int i = 0; i < FLOAT_PLATFORM; i++) {
        state.floating[i].Render(&program);
    }
    
    state.player->Render(&program);
    
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
