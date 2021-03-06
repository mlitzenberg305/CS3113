#include "Effects.h"

Effects::Effects(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
   // Non textured Shader
    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    currentEffect = NONE;
    
    alpha = 0;
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
void Effects::Start(EffectType effectType)
{
   currentEffect = effectType;
   switch (currentEffect) {
       case NONE:
           break;
       case FADEIN:
           alpha = 2.5f;
           break;
   }

}

void Effects::Update(float deltaTime)
{
   switch (currentEffect) {
       case NONE:
           break;
       case FADEIN:
           alpha -= deltaTime;
           if (alpha <= 0) currentEffect = NONE;
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
        case FADEIN:
            modelMatrix = glm::scale(modelMatrix, glm::vec3(10, 10, 1));
            
            program.SetModelMatrix(modelMatrix);
            program.SetColor(0, 0, 0, alpha);
            DrawOverlay();
            break;
    }
}
