#pragma once
#define GL_SILENCE_DEPRECATION
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <math.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "ShaderProgram.h"
#include "Util.h"
#include "Entity.h"

enum GameStatus { ACTIVE, WIN, LOSE, CAPTURE };
enum Tutorial { VIEW, MOVE, HIDDEN, RUN, EAT, DONE };

struct GameState {
    Entity *player;
    Entity *enemies;
    Entity *objects;
    
    GLuint fontTextureID;
    GLuint leavesTextureID;
    
    int nextScene;
    GameStatus gameStatus;
    bool tutorialActive;
    Tutorial tutorial;
};

class Scene {
public:
    GameState state;
    GameStatus gameStatus;
    
    glm::mat4 uiViewMatrix;
    glm::mat4 uiProjectionMatrix;

    virtual void Initialize() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render(ShaderProgram *program) = 0;
};
