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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"

#define PLATFORM_COUNT 3

enum GameStatus { MENU, ACTIVE, SUCCESS, FAIL };

struct GameState {
    Entity *player;
    Entity *platforms;
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
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Xoco Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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
    state.player->position = glm::vec3(0, 3.0f, 0);
    state.player->width = 0.5f;
    state.player->height = 0.5f;
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -0.25f, 0);
    state.player->speed = 0.5f;
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
    
    // INITIALIZE PLATFORMS
    
    state.platforms = new Entity[PLATFORM_COUNT];
    
    state.platforms[0].textureID = spriteSheet;
    state.platforms[0].position = glm::vec3(-1.0f, -3.25f, 0.0f);
    state.platforms[0].animIndices = new int[1] {0};
    state.platforms[0].animIndex = 0;
    state.platforms[0].animCols = 4;
    state.platforms[0].animRows = 3;
    
    state.platforms[1].textureID = spriteSheet;
    state.platforms[1].position = glm::vec3(0.0f, -3.25f, 0.0f);
    
    state.platforms[2].textureID = spriteSheet;
    state.platforms[2].position = glm::vec3(1.0f, -3.25f, 0.0f);
    
    // INITIALIZE TEXT
    
    GLuint textTextureID = LoadTexture("Assets/pixel_font.png");
    
    // make the text happen here
    
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
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        break;
                    case SDLK_RIGHT:
                        break;
                }
        }
    }
    
    state.player->movement = glm::vec3(0);
    
    // key inputs
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_LEFT]) {
        state.player->movement.x = 1.0f;
        state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->movement.x = -1.0f;
        state.player->animIndices = state.player->animRight;
    }
    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
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
        state.player->Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT);
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Render(&program);
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
