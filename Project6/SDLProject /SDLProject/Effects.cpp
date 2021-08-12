#include "Effects.h"

Effects::Effects(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
   // Non textured Shader
    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    currentEffect = NONE;
    
    alpha = 0;
    speed = 1;
    size = 20;

    viewOffset = glm::vec3(0);
    up = true;
}
void Effects::DrawOverlay()
{
    glUseProgram(program.programID);
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
}
void Effects::Start(EffectType effectType, float effectSpeed)
{
    currentEffect = effectType;
    speed = effectSpeed;
    
   switch (currentEffect) {
       case NONE:
           break;
       case FADEIN:
           alpha = 1.0f;
           break;
       case FADEOUT:
           alpha = 0;
           break;
       case GROW:
           size = 0;
           break;
       case SHRINK:
           break;
       case SHAKE:
       case SHAKE_Y:
           timeLeft = 1.0f;
           break;
       case GREEN:
       case RED:
       case DOG_MOVE:
           break;
   }

}

void Effects::Update(float deltaTime, Entity *player)
{
   switch (currentEffect) {
       case NONE:
           break;
       case FADEIN:
           alpha -= deltaTime * speed;
           if (alpha <= 0) currentEffect = NONE;
           break;
       case FADEOUT:
           alpha += deltaTime * speed;
           break;
       case GROW:
           size += deltaTime * speed;
           break;
       case SHRINK:
           size -= deltaTime * speed;
           if (size <= 0) currentEffect = NONE;
           break;
       case SHAKE:
           timeLeft -= deltaTime * speed;
           if (timeLeft <= 0) {
               viewOffset = glm::vec3(0);
               currentEffect = NONE;
           } else {
               float max = 0.1f;
               float min = -0.1f;
               float r = ((float)rand() / RAND_MAX) * (max - min) + min;
               viewOffset = glm::vec3(r, 0, r);
           }
           break;
       case SHAKE_Y:
           timeLeft -= deltaTime * speed;
           if (timeLeft <= 0) {
               viewOffset = glm::vec3(0);
               currentEffect = NONE;
           } else {
               float max = 0.05f;
               float min = -0.05f;
               float r = ((float)rand() / RAND_MAX) * (max - min) + min;
               viewOffset = glm::vec3(0, r, 0);
           }
           break;
       case GREEN:
       case RED:
           if (player->lastCollision == NULL) {
               currentEffect = NONE;
           }
           break;
       case DOG_MOVE:
           if (glm::length(player->velocity) == 0) {
               viewOffset = glm::vec3(0);
               currentEffect = NONE;
           } else {
               float r = viewOffset.y;
               if (up) {
                   r += -0.075 * deltaTime / speed;
                   if (r < -0.015){
                       up = false;
                   }
               } else {
                   if (viewOffset.y > 0) {
                       up = true;
                   } else {
                       r += 0.075 * deltaTime / speed;
                   }
               }
               viewOffset = glm::vec3(0, r, 0);
           }
           break;
   }
}

void Effects::Render()
{
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    
    switch (currentEffect) {
        case NONE:
            return;
            break;
        case FADEOUT:
        case FADEIN:
            modelMatrix = glm::scale(modelMatrix, glm::vec3(size, size * 0.75f, 1));
            
            program.SetModelMatrix(modelMatrix);
            program.SetColor(0, 0, 0, alpha);
            DrawOverlay();
            break;
        case SHRINK:
        case GROW:
            modelMatrix = glm::scale(modelMatrix, glm::vec3(size, size * 0.75f, 1));
            program.SetModelMatrix(modelMatrix);
            program.SetColor(0, 0, 0, 1);
            DrawOverlay();
            break;
        case SHAKE:
        case SHAKE_Y:
            break;
        case GREEN:
            modelMatrix = glm::scale(modelMatrix, glm::vec3(size, size * 0.75f, 1));
            
            program.SetModelMatrix(modelMatrix);
            program.SetColor(0, 0.5, 0.1, 0.1);
            DrawOverlay();
            break;
        case RED:
            modelMatrix = glm::scale(modelMatrix, glm::vec3(size, size * 0.75f, 1));
            
            program.SetModelMatrix(modelMatrix);
            program.SetColor(0.5, 0, 0.1, 0.1);
            DrawOverlay();
            break;
        case DOG_MOVE:
            break;
    }
}
