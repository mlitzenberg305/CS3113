#include "Lose.h"

#define LOSE_WIDTH 11
#define LOSE_HEIGHT 8

unsigned int lose_data[] =
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

GLuint fontLoseTextureID;

void Lose::Initialize() {
    
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("Assets/kenney_pixelplatformer/Tilemap/tiles_packed.png");
    fontLoseTextureID = Util::LoadTexture("Assets/pixel_font.png");

    state.map = new Map(LOSE_WIDTH, LOSE_HEIGHT, lose_data, mapTextureID, 1.0f, 20, 9);
    
    state.player = new Entity();
}

void Lose::Update(float deltaTime) {
    return;
}

void Lose::Render(ShaderProgram *program) {
    state.map->Render(program);
    Util::DrawText(program, fontLoseTextureID, "ALIEN JAM", 0.75f, 0.25f, glm::vec3(1.0f, -1.0f, 0));
    Util::DrawText(program, fontLoseTextureID, "You", 0.5f, 0.25f, glm::vec3(3.0f, -2.5f, 0));
    Util::DrawText(program, fontLoseTextureID, "Lose!", 0.5f, 0.25f, glm::vec3(3.0f, -3.25f, 0));
    Util::DrawText(program, fontLoseTextureID, "Press Enter to", 0.25f, 0.1f, glm::vec3(2.75f, -5.0f, 0));
    Util::DrawText(program, fontLoseTextureID, "Go to Menu", 0.25f, 0.1f, glm::vec3(2.75f, -5.5f, 0));
}
