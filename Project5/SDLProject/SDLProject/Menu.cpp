#include "Menu.h"

#define MENU_WIDTH 11
#define MENU_HEIGHT 8

unsigned int menu_data[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 17, 18, 18, 18, 18, 18, 19, 0, 0,
    0, 0, 37, 38, 38, 38, 38, 38, 39, 0, 0,
    0, 0, 37, 38, 38, 38, 38, 38, 39, 0, 0,
    0, 0, 37, 38, 38, 38, 38, 38, 39, 0, 0,
    0, 0, 57, 58, 58, 58, 58, 58, 59, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

GLuint fontMenuTextureID;

void Menu::Initialize() {
    
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("Assets/kenney_pixelplatformer/Tilemap/tiles_packed.png");
    fontMenuTextureID = Util::LoadTexture("Assets/pixel_font.png");

    state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menu_data, mapTextureID, 1.0f, 20, 9);
    
    state.player = new Entity();
}

void Menu::Update(float deltaTime) {
    return;
}

void Menu::Render(ShaderProgram *program) {
    state.map->Render(program);
    Util::DrawText(program, fontMenuTextureID, "ALIEN JAM", 0.75f, 0.25f, glm::vec3(1.0f, -1.0f, 0));
    Util::DrawText(program, fontMenuTextureID, "Press", 0.5f, 0.25f, glm::vec3(2.5f, -3.0f, 0));
    Util::DrawText(program, fontMenuTextureID, "Enter", 0.5f, 0.25f, glm::vec3(2.5f, -3.75f, 0));
    Util::DrawText(program, fontMenuTextureID, "to Play!", 0.5f, 0.25f, glm::vec3(2.5f, -4.5f, 0));
}
