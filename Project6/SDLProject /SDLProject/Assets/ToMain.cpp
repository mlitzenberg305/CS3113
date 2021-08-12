#include "ToMain.h"

void ToMain::Initialize() {
    
    uiViewMatrix = glm::mat4(1.0f);
    uiProjectionMatrix = glm::ortho(-6.4f, 6.4f, -3.6f, 3.6f, -1.0f, 1.0f);
    
    state.nextScene = -1;
    
    state.fontTextureID = Util::LoadTexture("Assets/font1.png");
    
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0, 0, 0);
    
    state.gameStatus = ACTIVE;
}

void ToMain::Update(float deltaTime) {
}

void ToMain::Render(ShaderProgram *program) {
    
    program->SetProjectionMatrix(uiProjectionMatrix);
    program->SetViewMatrix(uiViewMatrix);
    
    Util::DrawText(program, state.fontTextureID, "...tomorrow, the city", 0.5, -0.2f, glm::vec3(-3.5, 1, 0), 0);
    Util::DrawText(program, state.fontTextureID, "awaits you, Xoco...", 0.5, -0.2f, glm::vec3(-3.5, 0, 0), 0);
    
    Util::DrawText(program, state.fontTextureID, "make sure to eat trash", 0.25, -0.1, glm::vec3(-2.5, -1, 0), 0);
    Util::DrawText(program, state.fontTextureID, "to keep your energy up!", 0.25, -0.1, glm::vec3(-2.5, -1.5, 0), 0);

    Util::DrawText(program, state.fontTextureID, "PRESS ENTER", 0.5, -0.2, glm::vec3(-2, -2.5, 0), 0);
}
