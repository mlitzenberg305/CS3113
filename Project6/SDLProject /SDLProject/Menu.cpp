#include "Menu.h"

#define SIGN_COUNT 1

void Menu::Initialize() {
    
    uiViewMatrix = glm::mat4(1.0f);
    uiProjectionMatrix = glm::ortho(-6.4f, 6.4f, -3.6f, 3.6f, -1.0f, 1.0f);
    
    state.nextScene = -1;
    
    state.fontTextureID = Util::LoadTexture("Assets/font1.png");
    
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0, 0, 0);
    
    // INITIALIZE SIGNS
    state.objects = new Entity[SIGN_COUNT];
    
    GLuint signTextureID = Util::LoadTexture("Assets/sign.png");

    state.objects[0].billboard = true;
    state.objects[0].textureID = signTextureID;
    state.objects[0].position = state.player->position;
    state.objects[0].position.z -= 1;
    state.objects[0].scale = glm::vec3(0.5);
    
    state.gameStatus = ACTIVE;
}

void Menu::Update(float deltaTime) {
    
    for(int i = 0; i < SIGN_COUNT; i++) {
        state.objects[i].Update(deltaTime, state.player, NULL, 0, NULL, 0);
    }
}

void Menu::Render(ShaderProgram *program) {
    
    for(int i = 0; i < SIGN_COUNT; i++) {
        state.objects[i].Render(program);
    }
    
    program->SetProjectionMatrix(uiProjectionMatrix);
    program->SetViewMatrix(uiViewMatrix);
    
    Util::DrawText(program, state.fontTextureID, "PRESS ENTER", 0.5, -0.2f, glm::vec3(-1.5, 1, 0), 0);
    Util::DrawText(program, state.fontTextureID, "TO PLAY", 0.5, -0.2f, glm::vec3(-0.9, 0.5, 0), 0);
    
    Util::DrawText(program, state.fontTextureID, "press Q to QUIT", 0.25, -0.1f, glm::vec3(-1, 0, 0), 0);
    Util::DrawText(program, state.fontTextureID, "at anytime", 0.25, -0.1f, glm::vec3(-0.75, -0.25, 0), 0);
}
