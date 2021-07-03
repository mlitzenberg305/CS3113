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
    energy = 0;
    
    isActive = GAMEON;
    repeat = false;
    rotate = false;
    
    modelMatrix = glm::mat4(1.0f);
}

bool Entity::CheckCollision(Entity *other) {
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    
    if (xdist < 0 && ydist < 0) return true;
    
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
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
            }
            if (object->entityType == PLATFORM) {
                isActive = GAMEOVER;
            } else if (object->entityType == SUCCESS){
                isActive = COMPLETE;
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
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
            }
            if (object->entityType == PLATFORM) {
                isActive = GAMEOVER;
            } else if (object->entityType == SUCCESS){
                isActive = COMPLETE;
            }
        }
    }
}

void Entity::Update(float deltaTime, Entity *platforms, int platformCount)
{
    if (entityType == PLAYER) {
        if (animIndices != NULL && isActive == GAMEON) {
            if (glm::length(movement) != 0 && glm::length(velocity) != 0 ) {
                
                animTime += deltaTime;
                
                if (animTime >= 0.25f) {
                    animTime = 0.0f;
                    animIndex++;
                    if (animIndex >= animFrames) {
                        animIndex = 0;
                    }
                }
            } else if (glm::length(velocity) == 0) {
                animIndices = animStop;
                
            } else {
                animIndices = animDown;
                
                animTime += deltaTime;
                
                if (animTime >= 0.25f) {
                    animTime = 0.0f;
                    animIndex++;
                    if (animIndex >= animFrames) {
                        animIndex = 0;
                    }
                }
                // animIndex = 0;
            }
        }
    }
    if (entityType == PLATFORM) {
        if (animIndices != NULL){
            animIndices = animStop;
        }
    }
    if (entityType == PLAYER && isActive == GAMEON) {
        acceleration.x = movement.x * speed;
        velocity += acceleration * deltaTime;
        
        position.y += velocity.y * deltaTime;           // Move on Y
        CheckCollisionsY(platforms, platformCount);     // Fix if needed
        
        position.x += velocity.x * deltaTime;           // Move on X
        CheckCollisionsX(platforms, platformCount);     // Fix if needed
    }
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    if (entityType == PLATFORM) {
        if (rotate) {
            modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        }
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
    program->SetModelMatrix(modelMatrix);
    
    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }
    
    if (repeat) {
        if (height != 1) {
            float vertices[]  = { -1 * (height / 2), -0.5, (height / 2), -0.5, (height / 2), 0.5, -1 * (height / 2), -0.5, (height / 2), 0.5, -1 * (height / 2), 0.5 };
            float texCoords[] = { 0.0, 1.0, height, 1.0, height, 0.0, 0.0, 1.0, height, 0.0, 0.0, 0.0 };
            
            glBindTexture(GL_TEXTURE_2D, textureID);
            
            glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
            glEnableVertexAttribArray(program->positionAttribute);
            
            glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
            glEnableVertexAttribArray(program->texCoordAttribute);
            
            glDrawArrays(GL_TRIANGLES, 0, 6);
            
            glDisableVertexAttribArray(program->positionAttribute);
            glDisableVertexAttribArray(program->texCoordAttribute);
            
        } else if (width != 1) {
            
            float vertices[]  = { -1 * (width / 2), -0.5, (width / 2), -0.5, (width / 2), 0.5, -1 * (width / 2), -0.5, (width / 2), 0.5, -1 * (width / 2), 0.5 };
            float texCoords[] = { 0.0, 1.0, width, 1.0, width, 0.0, 0.0, 1.0, width, 0.0, 0.0, 0.0 };
            
            glBindTexture(GL_TEXTURE_2D, textureID);
            
            glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
            glEnableVertexAttribArray(program->positionAttribute);
            
            glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
            glEnableVertexAttribArray(program->texCoordAttribute);
            
            glDrawArrays(GL_TRIANGLES, 0, 6);
            
            glDisableVertexAttribArray(program->positionAttribute);
            glDisableVertexAttribArray(program->texCoordAttribute);
        }
        
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
