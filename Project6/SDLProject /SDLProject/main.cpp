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
#include <SDL_mixer.h>

#include "Entity.h"
#include "Effects.h"
#include "Util.h"

#include "Scene.h"
#include "IntroLevel.h"

#define WORLD_BUILDING 0

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix, uiViewMatrix, uiProjectionMatrix, modelMatrix;

Scene *currentScene;
Scene *sceneList[1];
Effects *effects;
Effects *effects2;

GLuint fontTextureID;

void SwitchToScene(Scene *scene) {
    currentScene = scene;
    currentScene->Initialize();
}

void Initialize() {
    // AUDIO
    
//    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
//    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
//
//    Mix_Music *music;
//    music = Mix_LoadMUS("music.mp3");       // replace mp3
//
//    Mix_PlayMusic(music, -1);               // loops for ever
//    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
//
//    Mix_Chunk *breathing;
//    breathing = Mix_LoadWAV("sound.wav");   // replace wav w sound effect
//
//    Mix_VolumeChunk(breathing, MIX_MAX_VOLUME / 4);
    
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("3D!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 1280, 720);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::perspective(glm::radians(45.0f), 1.777f, 0.1f, 100.0f);
    
    uiViewMatrix = glm::mat4(1.0f);
    uiProjectionMatrix = glm::ortho(-6.4f, 6.4f, -3.6f, 3.6f, -1.0f, 1.0f);
    fontTextureID = Util::LoadTexture("Assets/font1.png");
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

    glClearColor(0.3f, 0.4f, 0.3f, 1.0f);
        
    sceneList[0] = new IntroLevel();
    SwitchToScene(sceneList[0]);
    
    effects = new Effects(projectionMatrix, viewMatrix);
    effects2 = new Effects(projectionMatrix, viewMatrix);
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
                        if (WORLD_BUILDING == 1){
                            currentScene->state.player->position.y += 1.0f;
                        }
                        effects2->Start(SHAKE, 20.0f);
                        
                        if (currentScene->state.player->lastCollision != NULL) {
                            if (currentScene->state.player->lastCollision->entityType == TRASH) {
                                
                                currentScene->state.player->lastCollision->isActive = false;
                                
                                if (currentScene->state.player->energy < 50.f) {
                                    
                                    currentScene->state.player->energy += currentScene->state.player->lastCollision->energy;
                                    
                                    if (currentScene->state.player->energy > 50.f) {
                                        
                                        float temp = currentScene->state.player->energy - 50;
                                        currentScene->state.player->energy -= temp;
                                    }
                                }
                            }
                        }
                        //effects->Start(FADEIN, 1.0f);
                        break;
                    case SDLK_RETURN:
                    case SDLK_RIGHT:
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
                    case SDLK_LSHIFT:
                    case SDLK_RSHIFT:
                        currentScene->state.player->speed = 1.0f;
                        break;
                }
                break;
        }
    }
    
    // key inputs
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A]) {
        
        currentScene->state.player->energy -= 0.0025;
        
        currentScene->state.player->rotation.y += currentScene->state.player->speed / 2;
        
    } else if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) {
        
        currentScene->state.player->energy -= 0.0025;
        
        currentScene->state.player->rotation.y -= currentScene->state.player->speed / 2;
    }
    if (WORLD_BUILDING == 1){
        if (keys[SDL_SCANCODE_UP]) {
            currentScene->state.player->rotation.x += 1.0f;
        } else if (keys[SDL_SCANCODE_DOWN]) {
            currentScene->state.player->rotation.x -= 1.0f;
        }
    }
    
    currentScene->state.player->velocity.x = 0;
    currentScene->state.player->velocity.z = 0;
    if (keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT]) {
        currentScene->state.player->speed = 2.0f;
    }
    if (keys[SDL_SCANCODE_W] || (WORLD_BUILDING == 0 && keys[SDL_SCANCODE_UP])) {
        
        currentScene->state.player->energy -= 0.005 * currentScene->state.player->speed;

        currentScene->state.player->velocity.z = cos(glm::radians(currentScene->state.player->rotation.y)) * -currentScene->state.player->speed;
        currentScene->state.player->velocity.x = sin(glm::radians(currentScene->state.player->rotation.y)) * -currentScene->state.player->speed;
        
        effects->Start(DOG_MOVE, currentScene->state.player->speed);
    } else if (keys[SDL_SCANCODE_S] || (WORLD_BUILDING == 0 && keys[SDL_SCANCODE_DOWN])) {
        
        currentScene->state.player->energy -= 0.005 * currentScene->state.player->speed;
        
        currentScene->state.player->velocity.z = cos(glm::radians(currentScene->state.player->rotation.y)) * currentScene->state.player->speed;
        currentScene->state.player->velocity.x = sin(glm::radians(currentScene->state.player->rotation.y)) * currentScene->state.player->speed;
        
        effects->Start(DOG_MOVE, currentScene->state.player->speed);
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
        
        if (currentScene->state.player->position.x >= 9.5f) {
            currentScene->state.player->position.x = -9;
        }
        currentScene->Update(FIXED_TIMESTEP);
        
        if (currentScene->state.player->lastCollision != NULL && currentScene->state.player->lastCollision->entityType == TRASH) {
            effects2->Start(GREEN, 0);
        } else {
            effects2->Start(NONE, 0);
        }

        effects->Update(FIXED_TIMESTEP, currentScene->state.player);
        effects2->Update(FIXED_TIMESTEP, currentScene->state.player);
        
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;
    
    viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::rotate(viewMatrix, glm::radians(currentScene->state.player->rotation.y), glm::vec3(0, -1.0f, 0));
    viewMatrix = glm::rotate(viewMatrix, glm::radians(currentScene->state.player->rotation.x), glm::vec3(-1.0f, 0.0f, 0));
    viewMatrix = glm::translate(viewMatrix, -currentScene->state.player->position);
    viewMatrix = glm::translate(viewMatrix, effects->viewOffset);
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    currentScene->Render(&program);
    
    effects->Render();
    effects2->Render();
    
    glUseProgram(program.programID);

    program.SetProjectionMatrix(uiProjectionMatrix);
    program.SetViewMatrix(uiViewMatrix);
    Util::DrawText(&program, fontTextureID, "HEALTH: " + std::to_string(int(ceil(currentScene->state.player->health))), 0.5, -0.3f, glm::vec3(-5, 3, 0));
    Util::DrawText(&program, fontTextureID, "energy: " + std::to_string(int(ceil(currentScene->state.player->energy))), 0.5, -0.3f, glm::vec3(-5, 2, 0));
    
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    
//    Mix_FreeChunk(breathing);
//    Mix_FreeMusic(music);
    
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        if (currentScene->state.nextScene >= 0) SwitchToScene(sceneList[currentScene->state.nextScene]);
        Render();
    }
    
    Shutdown();
    return 0;
}
