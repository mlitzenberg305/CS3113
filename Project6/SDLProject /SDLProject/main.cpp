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
#include "Menu.h"
#include "IntroLevel.h"
#include "Win.h"
#include "Lose.h"
#include "Saved.h"
#include "Captured.h"
#include "EndScreen.h"
#include "ToMain.h"

#define WORLD_BUILDING 0

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix, uiViewMatrix, uiProjectionMatrix, modelMatrix;

Scene *currentScene;
Scene *sceneList[8];
Effects *effects;
Effects *effects2;

Mix_Music *music;
Mix_Chunk *breathing;
Mix_Chunk *foodCrunch;
Mix_Chunk *bite;
Mix_Chunk *steps;
Mix_Chunk *whimper;

void SwitchToScene(Scene *scene) {
    currentScene = scene;
    currentScene->Initialize();
}

void Initialize() {
    // AUDIO
    
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    music = Mix_LoadMUS("Assets/Sounds/kmart-lot.mp3");       // replace mp3
    Mix_VolumeMusic(MIX_MAX_VOLUME);

    breathing = Mix_LoadWAV("Assets/Sounds/panting.wav");   // replace wav w sound effect
    Mix_VolumeChunk(breathing, MIX_MAX_VOLUME / 8);
    
    foodCrunch = Mix_LoadWAV("Assets/Sounds/crunchy-bite.wav");   // replace wav w sound effect
    Mix_VolumeChunk(foodCrunch, MIX_MAX_VOLUME);
    
    bite = Mix_LoadWAV("Assets/Sounds/attacking-dog.wav");   // replace wav w sound effect
    Mix_VolumeChunk(bite, MIX_MAX_VOLUME / 4);
    
    steps = Mix_LoadWAV("Assets/Sounds/steps.wav");   // replace wav w sound effect
    Mix_VolumeChunk(steps, MIX_MAX_VOLUME / 6);
    
    whimper = Mix_LoadWAV("Assets/Sounds/whimper.wav");   // replace wav w sound effect
    Mix_VolumeChunk(whimper, MIX_MAX_VOLUME);
    
    // VIDEO
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
        
    sceneList[0] = new Menu();
    sceneList[1] = new IntroLevel();
    sceneList[2] = new Win();
    sceneList[3] = new Lose();
    sceneList[4] = new Saved();
    sceneList[5] = new Captured();
    sceneList[6] = new EndScreen();
    sceneList[7] = new ToMain();
    //     sceneList[8] = new MainLevel();
    
    SwitchToScene(sceneList[0]);
    
    effects = new Effects(uiProjectionMatrix, uiViewMatrix);
    effects2 = new Effects(uiProjectionMatrix, uiViewMatrix);
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
                        if (currentScene->state.player->lastCollision != NULL) {
                            if (currentScene->state.player->lastCollision->entityType == TRASH) {
                                
                                effects->Start(SHAKE_Y, 20.0f);
                                Mix_PlayChannel(-1, foodCrunch, 0);
                                
                                currentScene->state.tutorial = DONE;
                                currentScene->state.player->lastCollision->isActive = false;
                                
                                if (currentScene->state.player->energy < 50.f) {
                                    
                                    currentScene->state.player->energy += currentScene->state.player->lastCollision->energy;
                                    
                                    if (currentScene->state.player->energy > 50.f) {
                                        
                                        float temp = ceil(currentScene->state.player->energy) - 50;
                                        currentScene->state.player->energy -= temp;
                                        
                                        if (currentScene->state.player->health < 10.f) {
                                            
                                            currentScene->state.player->health += temp * 0.1;
                                            
                                            if (currentScene->state.player->health > 10.f) {
                                                
                                                temp = ceil(currentScene->state.player->health) - 10;
                                                currentScene->state.player->health -= temp;
                                            }
                                        }
                                    }
                                }
                            } else if (currentScene->state.player->lastCollision->entityType == ENEMY
                                       && currentScene->state.player->lastCollision->aiType == RAT) {
                                
                                effects->Start(SHAKE, 5.0f);
                                Mix_PlayChannel(-1, bite, 0);
                                
                                currentScene->state.tutorial = DONE;
                                currentScene->state.player->energy -= 1;
                                currentScene->state.player->lastCollision->isActive = false;
                            }
                        }
                        break;
                    case SDLK_RETURN:
                        if (sceneList[2] == currentScene) {
                            effects->Start(FADEOUT, 1.0f);
                            currentScene->state.nextScene = 0;
                        
                        } else if (sceneList[3] == currentScene) {
                            effects->Start(FADEOUT, 1.0f);
                            currentScene->state.nextScene = 4;
                            // industrial sounds
                        
                        } else if (sceneList[4] == currentScene) {
                            effects->Start(FADEOUT, 1.0f);
                            currentScene->state.nextScene = 6;
                            // doctor sounds
                        
                        } else if (sceneList[5] == currentScene) {
                            effects->Start(FADEOUT, 1.0f);
                            currentScene->state.nextScene = 6;
                            // industrial sounds
                        } else if (sceneList[6] == currentScene) {
                            effects->Start(FADEOUT, 1.0f);
                            currentScene->state.nextScene = 0;
                            // home sounds
                        } else if (sceneList[7] == currentScene) {
                            effects->Start(FADEOUT, 1.0f);
                            currentScene->state.nextScene = 0;
                            // night time sounds
                        } else if (sceneList[0] == currentScene) {
                            effects->Start(FADEOUT, 1.0f);
                            currentScene->state.nextScene = 1;
                            Mix_PlayMusic(music, -1);
                        }
                        break;
                }
                break;
                
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        break;
                    case SDLK_q:
                        gameIsRunning = false;
                        break;
                    case SDLK_UP:
                    case SDLK_DOWN:
                    case SDLK_RIGHT:
                    case SDLK_LEFT:
                    case SDLK_w:
                    case SDLK_s:
                    case SDLK_a:
                    case SDLK_d:
                        Mix_HaltChannel(-1);
                        break;
                    case SDLK_LSHIFT:
                    case SDLK_RSHIFT:
                        currentScene->state.player->speed = 1.0f;
                        Mix_VolumeChunk(breathing, MIX_MAX_VOLUME / 8);
                        break;
                }
                break;
        }
    }
    
    // key inputs
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A]) {
        
        if (currentScene->state.tutorialActive) {
            currentScene->state.tutorial = MOVE;
        }
        
        currentScene->state.player->energy -= 0.0025;
        
        currentScene->state.player->rotation.y += currentScene->state.player->speed / 2;
        
        Mix_PlayChannel(-1, steps, 0);
        
    } else if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) {
        
        if (currentScene->state.tutorialActive) {
            currentScene->state.tutorial = MOVE;
        }

        currentScene->state.player->energy -= 0.0025;
        
        currentScene->state.player->rotation.y -= currentScene->state.player->speed / 2;
        
        Mix_PlayChannel(-1, steps, 0);
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
        Mix_VolumeChunk(breathing, MIX_MAX_VOLUME / 3);
        
        if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_DOWN] ||
            keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_RIGHT]) {
            
            currentScene->state.tutorial = EAT;
        }
    }
    
    if (keys[SDL_SCANCODE_W] || (WORLD_BUILDING == 0 && keys[SDL_SCANCODE_UP])) {
        
        if (currentScene->state.tutorialActive) {
            currentScene->state.tutorial = HIDDEN;
        }
        currentScene->state.player->energy -= 0.005 * currentScene->state.player->speed;

        currentScene->state.player->velocity.z = cos(glm::radians(currentScene->state.player->rotation.y)) * -currentScene->state.player->speed;
        currentScene->state.player->velocity.x = sin(glm::radians(currentScene->state.player->rotation.y)) * -currentScene->state.player->speed;
        
        effects->Start(DOG_MOVE, currentScene->state.player->speed);
        Mix_PlayChannel(-1, breathing, 0);
        Mix_PlayChannel(-1, steps, 0);
        
    } else if (keys[SDL_SCANCODE_S] || (WORLD_BUILDING == 0 && keys[SDL_SCANCODE_DOWN])) {
        
        if (currentScene->state.tutorialActive) {
            currentScene->state.tutorial = HIDDEN;
        }


        currentScene->state.player->energy -= 0.005 * currentScene->state.player->speed;
        
        currentScene->state.player->velocity.z = cos(glm::radians(currentScene->state.player->rotation.y)) * currentScene->state.player->speed;
        currentScene->state.player->velocity.x = sin(glm::radians(currentScene->state.player->rotation.y)) * currentScene->state.player->speed;
        
        effects->Start(DOG_MOVE, currentScene->state.player->speed);
        Mix_PlayChannel(-1, breathing, 0);
        Mix_PlayChannel(-1, steps, 0);
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
        }
        if (currentScene->state.player->lastCollision != NULL && currentScene->state.player->lastCollision->entityType == ENEMY) {
            effects2->Start(RED, 0);
            if (currentScene->state.player->enemyDistance < 0.25) {
                Mix_PlayChannel(-1, whimper, 0);
            }
        }
        
        // tutorial
        if (currentScene->state.player->lastCollision == NULL || currentScene->state.player->lastCollision->entityType != HIDE) {
            if (currentScene->state.tutorialActive) {
                currentScene->state.tutorial = RUN;
                currentScene->state.tutorialActive = false;
            }
        }
        
        // game state
        if (currentScene->state.tutorial == DONE) {
            currentScene->state.nextScene = 7;
        }
        if (currentScene->state.gameStatus == WIN) {
            
            effects->Start(FADEOUT, 0.5);
            Mix_PauseMusic();
            currentScene->state.nextScene = 2;
        }
        if (currentScene->state.gameStatus == LOSE) {
            effects->Start(FADEOUT, 1.0);
            Mix_PauseMusic();
            currentScene->state.nextScene = 3;
        }
        if (currentScene->state.gameStatus == CAPTURE) {
            effects->Start(FADEOUT, 0.5);
            Mix_PauseMusic();
            currentScene->state.nextScene = 5;
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
    viewMatrix = glm::translate(viewMatrix, effects2->viewOffset);
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    currentScene->Render(&program);
    
    program.SetProjectionMatrix(uiProjectionMatrix);
    program.SetViewMatrix(uiViewMatrix);
    
    effects->Render();
    effects2->Render();
    
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    
    Mix_FreeChunk(breathing);
    Mix_FreeChunk(foodCrunch);
    Mix_FreeChunk(bite);
    Mix_FreeChunk(steps);
    
    Mix_FreeMusic(music);
    
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        if (currentScene->state.nextScene >= 0) {
            SwitchToScene(sceneList[currentScene->state.nextScene]);
            effects->Start(FADEIN, 1.0f);
        }
        Render();
    }
    
    Shutdown();
    return 0;
}
