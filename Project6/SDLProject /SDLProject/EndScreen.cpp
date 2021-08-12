#include "EndScreen.h"

void EndScreen::Initialize() {
    
    uiViewMatrix = glm::mat4(1.0f);
    uiProjectionMatrix = glm::ortho(-6.4f, 6.4f, -3.6f, 3.6f, -1.0f, 1.0f);
    
    state.nextScene = -1;
    
    state.fontTextureID = Util::LoadTexture("Assets/font1.png");
    
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0, 0, 0);
    
    state.gameStatus = ACTIVE;
}

void EndScreen::Update(float deltaTime) {
}

void EndScreen::Render(ShaderProgram *program) {
    
    program->SetProjectionMatrix(uiProjectionMatrix);
    program->SetViewMatrix(uiViewMatrix);
    
    Util::DrawText(program, state.fontTextureID, "...and you got adopted", 0.5, -0.2f, glm::vec3(-3, 1, 0), 0);
    Util::DrawText(program, state.fontTextureID, "into a Loving Home!", 0.5, -0.2f, glm::vec3(-3, 0, 0), 0);

    Util::DrawText(program, state.fontTextureID, "PRESS ENTER", 0.25, 0, glm::vec3(-2, -1, 0), 0);
    Util::DrawText(program, state.fontTextureID, "TO RETURN TO MENU", 0.25, 0, glm::vec3(-2.25, -1.5, 0), 0);
}
