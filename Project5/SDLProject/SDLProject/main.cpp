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

#include "Entity.h"
#include "Map.h"
#include "Util.h"

#include "Scene.h"
#include "Menu.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix, uiViewMatrix, uiProjectionMatrix, modelMatrix;

GLuint heartTextureID;
GLuint deadHeartTextureID;

Scene *currentScene;
Scene *sceneList[4];

void SwitchToScene(Scene *scene) {
    currentScene = scene;
    currentScene->Initialize();
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("PLATFORMER", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    uiViewMatrix = glm::mat4(1.0f);
    uiProjectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    glUseProgram(program.programID);
    
    glClearColor(0.0f, 0.6f, 1.0f, 1.0f);
    glEnable(GL_BLEND);
    // transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    modelMatrix = glm::mat4(1.0f);
    
    heartTextureID = Util::LoadTexture("Assets/kenney_pixelplatformer/Tiles/tile_0044.png");
    deadHeartTextureID = Util::LoadTexture("Assets/kenney_pixelplatformer/Tiles/tile_0046.png");
    
    sceneList[0] = new Menu();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    
    SwitchToScene(sceneList[0]);

}

void ProcessInput() {
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
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
                        if (currentScene->state.player->collidedBottom) {
                            currentScene->state.player->jump = true;
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
                        break;
                    case SDLK_RIGHT:
                        break;
                    case SDLK_LEFT:
                        break;
                    case SDLK_RETURN:
                        if (currentScene == sceneList[0]) {
                            currentScene->state.nextScene = 1;
                        }
                        break;
                }
                
                break;
        }
    }
    
    // key inputs
    
    currentScene->state.player->movement = glm::vec3(0);
    
    if (keys[SDL_SCANCODE_LEFT]) {
        currentScene->state.player->movement.x = -1.0f;
        if (currentScene->state.player->collidedBottom) {
            currentScene->state.player->animIndices = currentScene->state.player->animLeft;
        }
        
    } else if (keys[SDL_SCANCODE_RIGHT]) {
        currentScene->state.player->movement.x = 1.0f;
        if (currentScene->state.player->collidedBottom) {
            currentScene->state.player->animIndices = currentScene->state.player->animRight;
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
        
        currentScene->Update(FIXED_TIMESTEP);
        
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;
    
    viewMatrix = glm::mat4(1.0f);
    
    if (currentScene->state.player->position.x > 5) {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
    } else {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    currentScene->Render(&program);
    
    program.SetProjectionMatrix(uiProjectionMatrix);
    program.SetViewMatrix(uiViewMatrix);
    
    if (sceneList[0] != currentScene) {
        
        if (currentScene->state.player->energy == 3) {
            for (int i = 0; i < 3; i++) {
                Util::DrawIcon(&program, heartTextureID, glm::vec3(-2 - i, 3.2, 0));
            }
        } else if (currentScene->state.player->energy == 2) {
            
            for (int i = 0; i < 2; i++) {
                Util::DrawIcon(&program, heartTextureID, glm::vec3(-2 - i, 3.2, 0));
            }
            Util::DrawIcon(&program, deadHeartTextureID, glm::vec3(-2 - 3, 3.2, 0));
            
        }  else if (currentScene->state.player->energy == 1) {
            
            Util::DrawIcon(&program, heartTextureID, glm::vec3(-2, 3.2, 0));
            
            for (int i = 1; i < 3; i++) {
                Util::DrawIcon(&program, deadHeartTextureID, glm::vec3(-2 - i, 3.2, 0));
            }
        } else {
            for (int i = 0; i < 3; i++) {
                Util::DrawIcon(&program, deadHeartTextureID, glm::vec3(-2 - i, 3.2, 0));
            }
        }
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
        
        if (currentScene->state.nextScene >=0) SwitchToScene(sceneList[currentScene->state.nextScene]);
        
        Render();
    }
    
    Shutdown();
    return 0;
}
