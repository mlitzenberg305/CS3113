#include "Win.h"

#define WIN_WIDTH 11
#define WIN_HEIGHT 8

unsigned int win_data[] =
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

GLuint fontWinTextureID;

void Win::Initialize() {
    
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("Assets/kenney_pixelplatformer/Tilemap/tiles_packed.png");
    fontWinTextureID = Util::LoadTexture("Assets/pixel_font.png");

    state.map = new Map(WIN_WIDTH, WIN_HEIGHT, win_data, mapTextureID, 1.0f, 20, 9);
    
    state.player = new Entity();
}

void Win::Update(float deltaTime) {
    return;
}

void Win::Render(ShaderProgram *program) {
    state.map->Render(program);
    Util::DrawText(program, fontWinTextureID, "ALIEN JAM", 0.75f, 0.25f, glm::vec3(1.0f, -1.0f, 0));
    Util::DrawText(program, fontWinTextureID, "You", 0.5f, 0.25f, glm::vec3(3.0f, -2.25f, 0));
    Util::DrawText(program, fontWinTextureID, "Win!", 0.5f, 0.25f, glm::vec3(3.0f, -3.5f, 0));
    Util::DrawText(program, fontWinTextureID, "Press Enter to", 0.25f, 0.1f, glm::vec3(2.75f, -5.0f, 0));
    Util::DrawText(program, fontWinTextureID, "Go to Menu", 0.25f, 0.1f, glm::vec3(2.75f, -5.5f, 0));
}
