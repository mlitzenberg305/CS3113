#include "Entity.h"

Entity::Entity()
{
    position = glm::vec3(0);
    movement = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    width = 1.0f;
    height = 1.0f;
    speed = 0;
    jumpPower = 0;
    energy = 0;
    
    isActive = false;
    
    modelMatrix = glm::mat4(1.0f);
}

bool Entity::CheckCollision(Entity *other) {
    
    if (!isActive || !other->isActive) return false;
    if (this == other) return false;
    
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    
    if (xdist < 0 && ydist < 0) {
        
        lastCollision = other->entityType;
        return true;
    }
    return false;
}

void Entity::CheckCollisionsY(Entity *objects, int objectCount, Entity *enemies, int enemyCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity *object = &objects[i];
        if (CheckCollision(object))
        {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
            }
        }
    }
    for (int i = 0; i < enemyCount; i++)
    {
            Entity* enemy = &enemies[i];

            if (CheckCollision(enemy)) {
                if (velocity.y > 0) {
                    velocity.y = 0;
                    collidedTop = true;
                }
                else if (velocity.y < 0) {
                    velocity.y = 0;
                    collidedBottom = true;
                }
                if (collidedTop) {
                    isActive = false;
                }
                else if (collidedBottom) {
                    enemy->isActive = false;
                    velocity.y = 2.0f;
                }
            }
        }
}

void Entity::CheckCollisionsX(Entity *objects, int objectCount, Entity *enemies, int enemyCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity *object = &objects[i];
        if (CheckCollision(object))
        {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
            }
        }
    }
    for (int i = 0; i < enemyCount; i++)
    {
            Entity* enemy = &enemies[i];

            if (CheckCollision(enemy))
            {
                
                if (velocity.x > 0) {
                    velocity.x = 0;
                    collidedRight = true;
                }
                else if (velocity.x < 0) {
                    velocity.x = 0;
                    collidedLeft = true;
                }
                if (collidedRight || collidedLeft) {
                    isActive = false;
                }
            }
        }
}

void Entity::AIBug(Entity *player) {
    switch (aiState) {
            
        case IDLE:
            movement = glm::vec3(0);
            if (glm::distance(position, player->position) < 3.0f) {
                aiState = WALKING;
            }
            break;
        case WALKING:

            if (position.x > player->position.x) {
                movement.x = -1;
            } else if (position.x - player->position.x < 0.1 || position.x - player->position.x > -0.1){
                movement.x = 0;
            } else {
                movement.x = 1;
            }
            if (glm::distance(position, player->position) > 4.0f) {
                aiState = IDLE;
            }
            break;
        case ATTACKING:
            break;
    }
}

void Entity::AIPointy(Entity *player) {
    
    switch (aiState) {
            
        case IDLE:
            movement.x = 1.0f;
            aiState = WALKING;
            break;
        case WALKING:
            
            speed = 0.5f;
            
            if (position.y <= player->position.y) {
                aiState = ATTACKING;
            }
            if (position.x <= -3.0f || position.x >= -1.0f) { // not sure how to implement the pit check here
                movement.x = movement.x * -1;
            }
            break;
        case ATTACKING:
            speed = 1;
            
            if (player->position.y < position.y) {
                aiState = WALKING;
            }
            if (player->position.x > position.x){
                movement.x = 1;
            } else {
                movement.x = -1;
            }
            break;
    }
}

void Entity::AIFlying(Entity *player) {
    switch (aiState) {
        case IDLE:
            movement.y = -1.0f;
            aiState = WALKING;
            break;
        case WALKING:
            if (position.y > 3.0) {
                movement.y = movement.y * -1;
            } else if (position.y < -1.0) {
                movement.y = movement.y * -1;
            }
            break;
        case ATTACKING:
            break;
    }
}

void Entity::AI(Entity *player) {
    switch (entityType) {
        case BUG:
            AIBug(player);
            break;
        case POINTY:
            AIPointy(player);
            break;
        case FLYING:
            AIFlying(player);
            break;
        case WALL:
            break;
        case PLAYER:
            break;
    }
}

void Entity::Update(float deltaTime, Entity *player, Entity *platforms, int platformCount, Entity* enemies, int enemyCount)
{
    if (isActive == false) return;
    
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
    if (entityType != PLAYER || entityType != WALL) {
        AI(player);
    }
    
    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;

            if (animTime >= 0.25f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        } else {
            animIndex = 0;
        }
    }
    
    if (jump) {
        jump = false;
        velocity.y += jumpPower;
    }
    
    velocity.x = movement.x * speed;
    if (entityType == FLYING) {
        velocity.y = movement.y * speed;
    }
    velocity += acceleration * deltaTime;
    
    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(platforms, platformCount, enemies, enemyCount);// Fix if needed
    
    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(platforms, platformCount, enemies,
                     enemyCount);// Fix if needed
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    if (movement.x == 1 || animIndices == animRight) {
        modelMatrix = glm::scale(modelMatrix, glm::vec3(-1.0f, 1.0f, 1.0f));
    }
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;
    
    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;
    
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v};
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram *program) {
    
    if (!isActive && this->entityType != PLAYER) return;
    
    program->SetModelMatrix(modelMatrix);
    
    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        
        return;
        
    } else {
        
        float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
        float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        
        glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program->positionAttribute);
        
        glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(program->texCoordAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program->positionAttribute);
        glDisableVertexAttribArray(program->texCoordAttribute);
    }
}
