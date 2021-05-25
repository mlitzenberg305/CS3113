#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <stdlib.h>

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define NUM_OF_KIBBLES 1
#define GROUND_TILES 12
#define DEEP_GROUND_TILES 12

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, bodyMatrix, cloudMatrix, projectionMatrix, ground[GROUND_TILES], deepGround[DEEP_GROUND_TILES], deepGround2[DEEP_GROUND_TILES];

float player_x = -4;
float player_rotate = 0;
bool player_flip = false;

float cloud_x = 0;
float cloud_y = 3.0;
bool cloud_flip = false;

struct Kibble {
    float kibble_x = 0;
    float kibble_y = 0;
    float kibble_rotate = 0;
    glm::mat4 kibbleMatrix;
};

struct Kibble kibbles[NUM_OF_KIBBLES];

GLuint bodyTextureID, kibbleTextureID, groundTextureID, deepGroundTextureID, cloudTextureID;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Xoco Kibble Cloud", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    bodyMatrix = glm::mat4(1.0f);
    cloudMatrix = glm::mat4(1.0f);
    
    for (int i = 0; i < GROUND_TILES; i++){
        ground[i] = glm::mat4(1.0f);
        int x_pos = i - (GROUND_TILES / 2);
        ground[i] = glm::translate(ground[i], glm::vec3(x_pos, -1.67f, 0.0f));
    }
    for (int i = 0; i < DEEP_GROUND_TILES; i++){
        deepGround[i] = glm::mat4(1.0f);
        int x_pos = i - (DEEP_GROUND_TILES / 2);
        deepGround[i] = glm::translate(deepGround[i], glm::vec3(x_pos, -2.67f, 0.0f));
    }
    for (int i = 0; i < DEEP_GROUND_TILES; i++) {
        deepGround2[i] = glm::mat4(1.0f);
        int x_pos = i - (DEEP_GROUND_TILES / 2);
        deepGround2[i] = glm::translate(deepGround2[i], glm::vec3(x_pos, -3.67f, 0.0f));
    }
    for (int i = 0; i < NUM_OF_KIBBLES; i++) {
        kibbles[i].kibbleMatrix = glm::mat4(1.0f);
        kibbles[i].kibble_x = cloud_x * 4;
        kibbles[i].kibble_y = cloud_y * 4;
    }
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    // program.SetColor(0.2f, 0.3f, 0.4f, 1.0f);
    bodyTextureID = LoadTexture("xoco-body.png");
    kibbleTextureID = LoadTexture("kibble.png");
    groundTextureID = LoadTexture("ground.png");
    deepGroundTextureID = LoadTexture("deep-ground.png");
    cloudTextureID = LoadTexture("cloud.png");
    
    glUseProgram(program.programID);
    
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

float bounce(float rotation,  float deltaTime) {
    static bool upFlag = true;      //      there is probably a better way to do this
    if (upFlag) {
        if (rotation >= 45.0f) {
            upFlag = false;
        }
        rotation += 100 * deltaTime;
    } else {
        if (rotation <= 0.0f) {
            upFlag = true;
        }
        rotation -= 100 * deltaTime;
    }
    return rotation;
}

float goBack(float x, float deltaTime) {
    static bool back = true;        //      there is probably a better way to do this
    if (back) {
        if (x >= 4.0f) {
            back = false;
            player_flip = true;
        }
        x += 2 * deltaTime;
    } else {
        if (x <= -4.0f) {
            back = true;
            player_flip = false;
        }
        x -= 2 * deltaTime;
    }
    return x;
}

float randomCloudX(float deltaTime) {
    float random_x = rand() % 15;
    random_x = random_x * deltaTime;
    return random_x;
}

float goBackCloud(float x, float deltaTime) {
    static bool back = true;        //      there is probably a better way to do this
    if (back) {
        if (x >= 3.75f) {
            back = false;
            cloud_flip = true;
        }
        x += randomCloudX(deltaTime);
    } else {
        if (x <= -3.75f) {
            back = true;
            cloud_flip = false;
        }
        x -= randomCloudX(deltaTime);
    }
    return x;
}

float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    player_x = goBack(player_x, deltaTime);
    player_rotate = bounce(player_rotate, deltaTime);
    bodyMatrix = glm::mat4(1.0f);
    bodyMatrix = glm::translate(bodyMatrix, glm::vec3(0.0f, -1.0f, 0.0f));
    bodyMatrix = glm::translate(bodyMatrix, glm::vec3(player_x, 0.0f, 0.0f));
    bodyMatrix = glm::rotate(bodyMatrix, glm::radians(player_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    if (player_flip) {
        bodyMatrix = glm::rotate(bodyMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    cloudMatrix = glm::mat4(1.0f);
    cloudMatrix = glm::translate(cloudMatrix, glm::vec3(cloud_x, cloud_y, 0.0f));
    cloudMatrix = glm::rotate(cloudMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    cloud_x = goBackCloud(cloud_x, deltaTime);
    if (cloud_flip) {
        cloudMatrix = glm::rotate(cloudMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    
    for (int i = 0; i < NUM_OF_KIBBLES; i++) {
        kibbles[i].kibbleMatrix = glm::mat4(1.0f);
        kibbles[i].kibbleMatrix = glm::scale(kibbles[i].kibbleMatrix, glm::vec3(0.25f, 0.25f, 0.0f));
        
        kibbles[i].kibble_rotate += 3 * deltaTime;
        if (kibbles[i].kibble_y >= -5.5f) {
            kibbles[i].kibble_y -= 5 * deltaTime;
        } else {
            kibbles[i].kibble_x = cloud_x * 4;
            kibbles[i].kibble_y = cloud_y * 4;
        }
        kibbles[i].kibbleMatrix = glm::translate(kibbles[i].kibbleMatrix, glm::vec3(kibbles[i].kibble_x, kibbles[i].kibble_y, 0.0f));
        kibbles[i].kibbleMatrix = glm::rotate(kibbles[i].kibbleMatrix, kibbles[i].kibble_rotate, glm::vec3(0.0f, 0.0f, 1.0f));
    }
}

void DrawBody() {
    program.SetModelMatrix(bodyMatrix);
    glBindTexture(GL_TEXTURE_2D, bodyTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void DrawKibbles() {
    for (int i = 0; i < NUM_OF_KIBBLES; i++) {
        program.SetModelMatrix(kibbles[i].kibbleMatrix);
        glBindTexture(GL_TEXTURE_2D, kibbleTextureID);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}
void DrawGround() {
    for (int i = 0; i < GROUND_TILES; i++) {
        program.SetModelMatrix(ground[i]);
        glBindTexture(GL_TEXTURE_2D, groundTextureID);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}
void DrawDeepGround() {
    for (int i = 0; i < DEEP_GROUND_TILES; i++) {
        program.SetModelMatrix(deepGround[i]);
        glBindTexture(GL_TEXTURE_2D, deepGroundTextureID);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}
void DrawDeepGround2() {
    for (int i = 0; i < DEEP_GROUND_TILES; i++) {
        program.SetModelMatrix(deepGround2[i]);
        glBindTexture(GL_TEXTURE_2D, deepGroundTextureID);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}
void DrawCloud() {
    program.SetModelMatrix(cloudMatrix);
    glBindTexture(GL_TEXTURE_2D, cloudTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    DrawBody();
    DrawKibbles();
    DrawGround();
    DrawDeepGround();
    DrawDeepGround2();
    DrawCloud();
    
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
    
    SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
