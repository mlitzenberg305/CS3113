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
    
    speed = 0.0f;
}

bool Entity::CheckCollision(Entity *other)
{
    lastCollision = NULL; 
    if (!isActive || !other->isActive) return false;
    if (this == other) return false;
    
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    float zdist = fabs(position.z - other->position.z) - ((depth + other->depth) / 2.0f);
    if (xdist < 0 && ydist < 0 && zdist < 0) {
        lastCollision = other;
        return true;
    }
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

void Entity::Update(float deltaTime, Entity *player, Entity *objects, int objectCount) {
    if (!isActive) return;
    
    glm::vec3 previousPosition = position;
    
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
            // faint
        }
        for (int i = 0; i < objectCount; i++) {
            // Ignore collisions with the floor
            if (objects[i].entityType == FLOOR || objects[i].entityType == HIDE || objects[i].entityType == PAVEMENT) continue;
            if (CheckCollision(&objects[i])) {
                if (objects[i].weight >= weight) {
                    position = previousPosition;
                } else {
                    position = previousPosition;
                    velocity = velocity * (1 - (objects[i].weight / weight));
                    
                    velocity += acceleration * deltaTime;
                    position += velocity * deltaTime;
                    
                    objects[i].velocity = velocity;
                }
                break;
            }
        }
    } else if (entityType != PLAYER) {
        if (!CheckCollision(player)) {
            velocity = glm::vec3(0);
//        } else {
//            for (int i = 0; i < objectCount; i++) {
//                // Ignore collisions with the floor
//                if (objects[i].entityType == FLOOR) continue;
//                if (&objects[i] == this) continue;
//
//                if (CheckCollision(&objects[i])) {
//                    if (objects[i].weight + weight >= player->weight) {
//                        position = previousPosition;
//                        player->velocity = glm::vec3(0);
//                    } else {
//                        position = previousPosition;
//                        velocity = velocity * ((objects[i].weight / weight) - (weight/2));
//
//                        velocity += acceleration * deltaTime;
//                        position += velocity * deltaTime;
//
//                        objects[i].velocity = velocity;
//                    }
//                    break;
//                }
//            }
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
