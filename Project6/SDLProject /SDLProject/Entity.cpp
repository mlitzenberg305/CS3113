#include "Entity.h"

Entity::Entity()
{
    position = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    
    modelMatrix = glm::mat4(1.0f);
    
    rotation = glm::vec3(0, 0, 0);
    scale = glm::vec3(1);
    
    width = 1.0f;
    height = 1.0f;
    depth = 1.0f;
    
    weight = 1.0f;
    energy = 1.0f;
    health = 1.0f;
    
    billboard = false;
    isActive = true;
    
    lastCollision = NULL;
    
    speed = 0.0f;
}

bool Entity::CheckCollision(Entity *other)
{
    if (!isActive || !other->isActive) return false;
    if (this == other) return false;
    
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    float zdist = fabs(position.z - other->position.z) - ((depth + other->depth) / 2.0f);

    if (xdist < 0 && ydist < 0 && zdist < 0) {
        lastCollision = other;
        return true;
    }
    lastCollision = NULL;
    return false;
}


void Entity::DrawBillboard(ShaderProgram *program) {
    
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::AIWalker(Entity *player, Entity *dumpster) {
    float directionX;
    float directionZ;
    
    switch (aiState) {
        case IDLE:
            directionX = position.x - player->position.x;
            directionZ = position.z - player->position.z;
            rotation.y = glm::degrees(atan2f(directionX, directionZ));

            if (fabs(glm::distance(player->position, position)) >= 1.5) {
                aiState = WALKING;
            }
            else if (fabs(glm::distance(player->position, position)) <= 1) {
                aiState = ATTACKING;
            }
            velocity = glm::vec3(0);
            break;
        case WALKING:
            if (fabs(glm::distance(player->position, position)) < 1.5) {
                aiState = IDLE;
            } else {
                directionX = position.x - dumpster->position.x;
                directionZ = position.z - dumpster->position.z;
                rotation.y = glm::degrees(atan2f(directionX, directionZ));
                
                velocity.z = cos(glm::radians(rotation.y)) * -speed;
                velocity.x = sin(glm::radians(rotation.y)) * -speed;
                
//                if (fabs(directionX) < 0.5) {
//                    velocity.x *= -1;
//                } else if (fabs(directionX) >= 3) {
//                    velocity.x *= -1;
//                }
//                if (fabs(directionZ) < 0.5) {
//                    velocity.z *= -1;
//                } else if (fabs(directionZ) >= 3) {
//                    velocity.z *= -1;
//                }
            }
            break;
        case ATTACKING:
            if (fabs(glm::distance(player->position, position)) > 1) {
                aiState = IDLE;
            }
            
            velocity.z = cos(glm::radians(rotation.y)) * -speed;
            velocity.x = sin(glm::radians(rotation.y)) * -speed;
            break;
    }
}

void Entity::AIOfficer(Entity *player) {
    switch (aiState) {
        case IDLE:
            break;
        case WALKING:
            velocity.x = -1.0;
            break;
        case ATTACKING:
            break;
    }
}

void Entity::AITruck(Entity *player) {
    switch (aiState) {
        case IDLE:
            break;
        case WALKING:
            velocity.x = -1.0;
            break;
        case ATTACKING:
            break;
    }
}

void Entity::AI(Entity *player, Entity *dumpster) {
    switch (aiType) {
        case RAT:
            AIWalker(player, dumpster);
            break;
        case AC_OFFICER:
            AIOfficer(player);
            break;
        case AC_TRUCK:
            AITruck(player);
            break;
    }
}

void Entity::Update(float deltaTime, Entity *player, Entity *objects, int objectCount, Entity *enemies, int enemyCount) {
    if (!isActive) return;
    
    glm::vec3 previousPosition = position;
    
    if (entityType == ENEMY) {
        Entity *dumpster = NULL;

        for (int i = 0; i < objectCount; i++) {
            if (objects[i].entityType == DUMPSTER) {
                if (glm::distance(objects[i].position, position) < 3) {
                    dumpster = &objects[i];
                }
            }
        }
        
        AI(player, dumpster);
    }
    
    if (scale != glm::vec3(1)) {
        if (width == 1.0f && height == 1.0f && depth == 1.0f) {
            width = scale.x;
            height = scale.y;
            depth = scale.z;
        } else if (width == 1.0f && height == 1.0f) {
            width = scale.x;
            height = scale.y;
        } else if (height == 1.0f && depth == 1.0f) {
            height = scale.y;
            depth = scale.z;
        } else if (width == 1.0f && depth == 1.0f){
            width = scale.x;
            depth = scale.z;
        } else if (width == 1.0f) {
            width = scale.x;
        } else if (height == 1.0f) {
            height = scale.y;
        } else if (depth == 1.0f) {
            depth = scale.z;
        }
    }
    
    if (billboard) {
        float directionX = position.x - player->position.x;
        float directionZ = position.z - player->position.z;
        rotation.y = glm::degrees(atan2f(directionX, directionZ));
        
        velocity.z = cos(glm::radians(rotation.y)) * -0.5;
        velocity.x = sin(glm::radians(rotation.y)) * -0.5;
    }
    
    velocity += acceleration * deltaTime;
    position += velocity * deltaTime;
    
    if (entityType == PLAYER) {
        if (energy <= 0) {
            health += energy * 0.005;
        }
        if (health <= 0) {
            isActive = false;
        }
        for (int i = 0; i < objectCount; i++) {
            // Ignore collisions with the floor
            if (objects[i].entityType == FLOOR || objects[i].entityType == PAVEMENT) continue;

            if (CheckCollision(&objects[i])) {
//                if (objects[i].weight >= weight) {
                if (objects[i].entityType != TRASH && objects[i].entityType != HIDE) {
                    position = previousPosition;
                }
//                } else {
//                    position = previousPosition;
//                    velocity = velocity * (1 - (objects[i].weight / weight));
//
//                    velocity += acceleration * deltaTime;
//                    position += velocity * deltaTime;
//
//                    objects[i].velocity = velocity;
//                }
                break;
            }
        }
        Entity *previousObjectCollision = lastCollision;
        
        for (int i = 0; i < enemyCount; i++) {
            
            if (CheckCollision(&enemies[i])) {
                if (enemies[i].aiType == AC_TRUCK) {
                    position = previousPosition;
                }
                if (enemies[i].aiType == RAT) {
                    health -= 0.05;
                }
                break;
            } else {
                lastCollision = previousObjectCollision;
            }
        }
        
    } else if (entityType != PLAYER && entityType != ENEMY) {
        if (!CheckCollision(player)) {
            velocity = glm::vec3(0);
        }
    } else if (entityType == ENEMY) {
        for (int i = 0; i < objectCount; i++) {
            // Ignore collisions with the floor, hides, and trash
            if (objects[i].entityType == FLOOR || objects[i].entityType == HIDE || objects[i].entityType == PAVEMENT || objects[i].entityType == TRASH) continue;

            if (CheckCollision(&objects[i])) {
                position = previousPosition;
                lastCollision = &objects[i];
                break;
            }
        }
    }

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
}

void Entity::Render(ShaderProgram *program) {
    if (!isActive) return;
    
    program->SetModelMatrix(modelMatrix);
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    if (billboard) {
        DrawBillboard(program);
    } else {
        mesh->Render(program);
    }
}
