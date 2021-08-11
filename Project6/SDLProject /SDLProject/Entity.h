#pragma once
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
#include "Mesh.h"

enum AIType { RAT, AC_OFFICER, AC_TRUCK };
enum AIState { IDLE, WALKING, ATTACKING };

enum EntityType { FLOOR, PAVEMENT, ENEMY, OBSTACLE, HIDE, PLAYER, TRASH, DUMPSTER };
class Entity {
public:
    EntityType entityType;
    
    AIType aiType;
    AIState aiState;
    
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec3 scale;
    glm::vec3 rotation;
    
    float speed;
    
    bool billboard;
    float width;
    float height;
    float depth;
    
    float weight;
    float energy;
    float health;
    bool isActive;
    Entity *lastCollision;
    
    GLuint textureID;
    Mesh *mesh;
    
    glm::mat4 modelMatrix;
    
    Entity();
    
    bool CheckCollision(Entity *other);
    void DrawBillboard(ShaderProgram *program);
    void Update(float deltaTime, Entity *player, Entity *objects, int objectCount, Entity *enemies, int enemyCount);
    void Render(ShaderProgram *program);
    
    void AI(Entity *player, Entity *dumpster);
    void AIWalker(Entity *player, Entity *dumpster);
    void AIOfficer(Entity *player);
    void AITruck(Entity *player);

};
