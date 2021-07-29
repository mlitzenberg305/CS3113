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
    inWater = false;
    nextLevel = false;
    
    modelMatrix = glm::mat4(1.0f);
}

void Entity::CheckCollisionsY(Map *map) {
    // Probes for tiles
    glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
    
    glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
    
    glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);
    
    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    int top_isSolid = map->IsSolid(top, &penetration_x, &penetration_y);
    int top_left_isSolid = map->IsSolid(top_left, &penetration_x, &penetration_y);
    int top_right_isSolid = map->IsSolid(top_right, &penetration_x, &penetration_y);
    
    int bottom_isSolid = map->IsSolid(bottom, &penetration_x, &penetration_y);
    int bottom_left_isSolid = map->IsSolid(bottom_left, &penetration_x, &penetration_y);
    int bottom_right_isSolid = map->IsSolid(bottom_right, &penetration_x, &penetration_y);
    
    if (inWater) {
        acceleration.y = -1.0;
        jumpPower = 0.75;
        speed = 0.5;
        collidedBottom = true;
        
        if (bottom_isSolid != 5 || bottom_left_isSolid != 5 || bottom_right_isSolid != 5) {
            inWater = false;
        }
        if (top_isSolid != 5 || top_left_isSolid != 5 || top_right_isSolid != 5) {
            if (top_isSolid > 0 && velocity.y > 0) {
                position.y -= penetration_y;
                velocity.y = 0;
                collidedTop = true;
            }
            else if (top_left_isSolid > 0 && velocity.y > 0) {
                position.y -= penetration_y;
                velocity.y = 0;
                collidedTop = true;
            }
            else if (top_right_isSolid > 0 && velocity.y > 0) {
                position.y -= penetration_y;
                velocity.y = 0;
                collidedTop = true;
            }
        }
    } else {
        acceleration.y = -9.8;
        jumpPower = 5.5;
        speed = 2.0;
    
        if (top_isSolid > 0 && velocity.y > 0 && top_isSolid != 2 && top_isSolid != 3) {
            position.y -= penetration_y;
            velocity.y = 0;
            collidedTop = true;
            if (top_isSolid == 6) {
                nextLevel = true;
            }
        }
        else if (top_left_isSolid > 0 && velocity.y > 0 && top_left_isSolid != 2 && top_left_isSolid != 3) {
            position.y -= penetration_y;
            velocity.y = 0;
            collidedTop = true;
            if (top_left_isSolid == 6) {
                nextLevel = true;
            }
        }
        else if (top_right_isSolid > 0 && velocity.y > 0 && top_right_isSolid != 2 && top_right_isSolid != 3) {
            position.y -= penetration_y;
            velocity.y = 0;
            collidedTop = true;
            if (top_right_isSolid == 6) {
                nextLevel = true;
            }
        }
        
        if (bottom_isSolid > 0 && velocity.y < 0) {
            position.y += penetration_y;
            velocity.y = 0;
            collidedBottom = true;
            if (bottom_isSolid == 4) {
                velocity.y = 4;
                energy --;
            } else if (bottom_isSolid == 5) {
                inWater = true;
            } else if (bottom_isSolid == 6) {
                nextLevel = true;
            }
        }
        else if (bottom_left_isSolid > 0 && velocity.y < 0) {
            if (bottom_left_isSolid == 5) {
                inWater = true;
            } else if (bottom_left_isSolid == 6) {
                nextLevel = true;
            } else if (bottom_left_isSolid != 4) {
                position.y += penetration_y;
                velocity.y = 0;
                collidedBottom = true;
            }
        }
        else if (bottom_right_isSolid > 0 && velocity.y < 0) {
            if (bottom_right_isSolid == 5) {
                inWater = true;
            } else if (bottom_right_isSolid == 6) {
                nextLevel = true;
            } else if (bottom_right_isSolid != 4) {
                position.y += penetration_y;
                velocity.y = 0;
                collidedBottom = true;
            }
        }
    }
}

void Entity::CheckCollisionsX(Map *map) {
    // Probes for tiles
    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    int left_isSolid = map->IsSolid(left, &penetration_x, &penetration_y);
    int right_isSolid = map->IsSolid(right, &penetration_x, &penetration_y);
    
    if (left_isSolid > 0 && velocity.x < 0) {
        if (left_isSolid == 1) {
            position.x += penetration_x;
            velocity.x = 0;
            collidedLeft = true;
        } else if (left_isSolid == 6) {
            nextLevel = true;
        }
    }
    if (right_isSolid > 0 && velocity.x > 0) {
        if (right_isSolid == 1) {
            position.x -= penetration_x;
            velocity.x = 0;
            collidedRight = true;
        } else if (right_isSolid == 6) {
            nextLevel = true;
        }
    }
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

void Entity::CheckCollisionsY(Entity *objects, int objectCount)
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
}

void Entity::CheckCollisionsX(Entity *objects, int objectCount)
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
}

void Entity::AIWalker(Entity *player) {
    switch (aiState) {
        case IDLE:

            break;
        case WALKING:

            break;
        case ATTACKING:
            break;
    }
}

void Entity::AI(Entity *player) {
    switch (aiType) {
        case WALKER:

            break;
        case WAITANDGO:

            break;
        case ATTACKER:

            break;
    }
}

void Entity::Update(float deltaTime, Entity *player, Entity *objects, int objCount, Map *map)
{
    if (isActive == false) return;
    
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
    if (entityType == ENEMY) {
        AI(player);
    }
    
    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;

            if (animTime >= 0.2f)
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
    
    modelMatrix = glm::mat4(1.0f);
    
    if (jump) {
        jump = false;
        velocity.y += jumpPower;
    }
    
    velocity.x = movement.x * speed;
    velocity += acceleration * deltaTime;
    
    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(map);
    //CheckCollisionsY(platforms, platformCount, enemies, enemyCount);
    
    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(map);
    //CheckCollisionsX(platforms, platformCount, enemies, enemyCount);
    
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
