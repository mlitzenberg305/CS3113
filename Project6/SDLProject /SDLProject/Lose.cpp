#include "Lose.h"

void Lose::Initialize() {
    
    uiViewMatrix = glm::mat4(1.0f);
    uiProjectionMatrix = glm::ortho(-6.4f, 6.4f, -3.6f, 3.6f, -1.0f, 1.0f);
    
    state.nextScene = -1;
    
    state.fontTextureID = Util::LoadTexture("Assets/font1.png");
    
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0, 0, 0);
    
    state.gameStatus = ACTIVE;
}

void Lose::Update(float deltaTime) {
}

void Lose::Render(ShaderProgram *program) {
    
    program->SetProjectionMatrix(uiProjectionMatrix);
    program->SetViewMatrix(uiViewMatrix);
    
    Util::DrawText(program, state.fontTextureID, "You fainted!", 1, -0.3f, glm::vec3(-3.5, 1, 0), 0);

    Util::DrawText(program, state.fontTextureID, "PRESS ENTER", 0.25, 0, glm::vec3(-2, -1, 0), 0);
}
