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

#define NUM_OF_TILES 12

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool gameOver = false;

ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix;
int numOfHits = 0;

struct CollisionBox {
    glm::vec3 position;
    glm::vec3 size;
};
struct Paddle {
    glm::vec3 position;
    glm::vec3 movement;
    struct CollisionBox box;
    float speed;
    glm::mat4 matrix;
    GLuint textureID;
};
struct Ball {
    glm::vec3 position;
    glm::vec3 movement;
    struct CollisionBox box;
    float speed;
    float rotation;
    glm::mat4 matrix;
    GLuint textureID;
};
struct Xoco {
    glm::vec3 position;
    glm::vec3 movement;
    struct CollisionBox box;
    float speed;
    float rotation;
    glm::mat4 matrix;
    GLuint textureID;
};
struct Tile {
    glm::vec3 position;
    glm::mat4 matrix;
    GLuint textureID;
};

struct Paddle paddleOne;
struct Paddle paddleTwo;
struct Ball ball;
struct Xoco xoco;
struct Tile tileTop[NUM_OF_TILES];
struct Tile tileBottom[NUM_OF_TILES];

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
    displayWindow = SDL_CreateWindow("Kibble Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    // initializing background
    for (int i = 0; i < NUM_OF_TILES; i++) {
        float x_pos = i - (NUM_OF_TILES / 2);
        tileTop[i].position = glm::vec3(x_pos, 3.3f, 0.0f);
        
        tileTop[i].matrix = glm::mat4(1.0f);
        tileTop[i].matrix = glm::translate(tileTop[i].matrix, tileTop[i].position);
        tileTop[i].matrix = glm::rotate(tileTop[i].matrix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        
        tileTop[i].textureID = LoadTexture("ground.png");
    }
    for (int i = 0; i < NUM_OF_TILES; i++) {
        float x_pos = i - (NUM_OF_TILES / 2);
        tileBottom[i].position = glm::vec3(x_pos, -3.3f, 0.0f);
        
        tileBottom[i].matrix = glm::mat4(1.0f);
        tileBottom[i].matrix = glm::translate(tileBottom[i].matrix, tileBottom[i].position);
        
        tileBottom[i].textureID = LoadTexture("ground.png");
    }
    // initializing Xoco
    xoco.matrix = glm::mat4(1.0f);
    xoco.position = glm::vec3(-4.0f, 0.0f, 0.0f);
    xoco.box.position = xoco.position;
    xoco.box.size = glm::vec3(0.5f, 1.0f, 0.0f);
    xoco.speed = 1;
    xoco.rotation = 0;
    xoco.movement = glm::vec3(1.0f, 1.0f, 0.0f);
    xoco.textureID = LoadTexture("xoco.png");
    
    // initializing ball
    ball.matrix = glm::mat4(1.0f);
    ball.position = glm::vec3(-4.0f, 0.0f, 0.0f);
    ball.box.position = ball.position;
    ball.box.size = glm::vec3(0.75f, 0.75f, 0.0f);
    ball.speed = 3;
    ball.rotation = 0.0f;
    ball.movement = glm::vec3(1.0f, -0.25f, 0.0f);
    ball.textureID = LoadTexture("kibble.png");
    
    // initializing paddle one
    paddleOne.matrix = glm::mat4(1.0f);
    paddleOne.position = glm::vec3(5.3f, 0.0f, 0.0f);
    paddleOne.box.position = paddleOne.position;
    paddleOne.box.size = glm::vec3(0.7f, 0.55f, 0.0f);
    paddleOne.speed = 3;
    paddleOne.textureID = LoadTexture("dog-bowlR.png");
    
    // initializing paddle two
    paddleTwo.matrix = glm::mat4(1.0f);
    paddleTwo.position = glm::vec3(-5.3f, 0.0f, 0.0f);
    paddleTwo.box.position = paddleTwo.position;
    paddleTwo.box.size = glm::vec3(0.7f, 0.55f, 0.0f);
    paddleTwo.speed = 3;
    paddleTwo.textureID = LoadTexture("dog-bowlL.png");
    
    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(1.0f, 0.9f, 0.55f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
        
        paddleOne.movement = glm::vec3(0, 0, 0);
        paddleTwo.movement = glm::vec3(0, 0, 0);
        if (!gameOver) {
            const Uint8 *keys = SDL_GetKeyboardState(NULL);
            
            if (keys[SDL_SCANCODE_UP]) {
                paddleOne.movement.y = 1.0f;
            }
            else if (keys[SDL_SCANCODE_DOWN]) {
                paddleOne.movement.y = -1.0f;
            }
            if (keys[SDL_SCANCODE_W]) {
                paddleTwo.movement.y = 1.0f;
            }
            else if (keys[SDL_SCANCODE_S]) {
                paddleTwo.movement.y = -1.0f;
            }
            if (glm::length(paddleOne.movement) > 1.0f) {
                paddleOne.movement = glm::normalize(paddleOne.movement);
            }
            if (glm::length(paddleTwo.movement) > 1.0f) {
                paddleTwo.movement = glm::normalize(paddleTwo.movement);
            }
        }
    }
}

float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    // ball collision test paddle one
    float xdist1 = fabs(ball.box.position.x - paddleOne.box.position.x) - ((ball.box.size.x + paddleOne.box.size.x) / 2.0f);
    float ydist1 = fabs(ball.box.position.y - paddleOne.box.position.y) - ((ball.box.size.y + paddleOne.box.size.y) / 2.0f);
    if (xdist1 < 0 && ydist1 < 0) {
        numOfHits += 1;                             // counts hits
        ball.movement.x = ball.movement.x * -1;     // reverses direction
        float hit1 = ball.box.position.y - paddleOne.box.position.y;
        ball.movement.y = hit1 * 1.5;               // changes angle
    }
    
    // ball collision test paddle two
    float xdist2 = fabs(ball.box.position.x - paddleTwo.box.position.x) - ((ball.box.size.x + paddleTwo.box.size.x) / 2.0f);
    float ydist2 = fabs(ball.box.position.y - paddleTwo.box.position.y) - ((ball.box.size.y + paddleTwo.box.size.y) / 2.0f);
    if (xdist2 < 0 && ydist2 < 0) {
        numOfHits += 1;                             // counts hits
        ball.movement.x = ball.movement.x * -1;     // reverses direction
        float hit2 = ball.box.position.y - paddleTwo.box.position.y;
        ball.movement.y = hit2 * 1.5;               // changes angle
    }
    // ball collision top & bottom wall
    if (ball.box.position.y + ball.box.size.y / 2 > 3.25f) {
        ball.movement.y = ball.movement.y * -1;
    } else if (ball.box.position.y - ball.box.size.y / 2 < -3.25f) {
        ball.movement.y = ball.movement.y * -1;
    }
    // ball collision side walls
    if (ball.position.x + ball.box.size.x / 2 > 5.1) {
        ball.movement = glm::vec3(0.0f, 0.0f, 0.0f);
        gameOver = true;
    } else if (ball.position.x - ball.box.size.x / 2 < -5.1) {
        ball.movement = glm::vec3(0.0f, 0.0f, 0.0f);
        gameOver = true;
    }
    // paddle one collision top & bottom wall
    if (paddleOne.box.position.y + paddleOne.box.size.y / 2 > 2.75f) {
        if (paddleOne.movement.y != -1) {
            paddleOne.movement.y = 0;
        }
    } else if (paddleOne.box.position.y - paddleOne.box.size.y / 2 < -2.75f) {
        if (paddleOne.movement.y != 1) {
            paddleOne.movement.y = 0;
        }
    }
    // paddle two collision top & bottom wall
    if (paddleTwo.box.position.y + paddleTwo.box.size.y / 2 > 2.75f) {
        if (paddleTwo.movement.y != -1) {
            paddleTwo.movement.y = 0;
        }
    } else if (paddleTwo.box.position.y - paddleTwo.box.size.y / 2 < -2.75f) {
        if (paddleTwo.movement.y != 1) {
            paddleTwo.movement.y = 0;
        }
    }
    
    // player positions
    paddleOne.position += paddleOne.movement * paddleOne.speed * deltaTime;
    paddleOne.box.position = paddleOne.position;
    paddleTwo.position += paddleTwo.movement * paddleTwo.speed * deltaTime;
    paddleTwo.box.position = paddleTwo.position;
    
    paddleOne.matrix = glm::mat4(1.0f);
    paddleOne.matrix = glm::translate(paddleOne.matrix, paddleOne.position);
    
    paddleTwo.matrix = glm::mat4(1.0f);
    paddleTwo.matrix = glm::translate(paddleTwo.matrix, paddleTwo.position);
    
    // ball positions
    ball.position += ball.movement * ball.speed * deltaTime;
    ball.box.position = ball.position;
    ball.matrix = glm::mat4(1.0f);
    ball.matrix = glm::translate(ball.matrix, ball.position);
    
    // ball rotation
    if (ball.movement.x == 1) {
        ball.rotation -= glm::length(ball.movement) + 0.5;
    } else if (ball.movement.x == -1) {
        ball.rotation += glm::length(ball.movement) + 0.5;
    }
    ball.matrix = glm::rotate(ball.matrix, glm::radians(ball.rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    
    // ball scale
    ball.matrix = glm::scale(ball.matrix, glm::vec3(0.5f, 0.5f, 1.0f));
    
    // ball speed
    if (numOfHits == 5) {
        ball.speed += 0.25f;
        numOfHits = 0;
    }
    
     //XOCO THINGS
    if (!gameOver) {
        // movement
        int xocoMove = rand() % 100;
        if (xocoMove == 5) {
            int xDir = rand() % 2;
            int yDir = rand() % 2;
            int xPos = rand() % 2;
            int yPos = rand() % 2;

            xoco.movement.x = xDir;
            xoco.movement.y = yDir;
            if (xPos == 0) {
                xoco.movement.x = xoco.movement.x * -1;
            }
            if (yPos == 0) {
                xoco.movement.y = xoco.movement.y * -1;
            }
        }
        // collision top & bottom
        if (xoco.box.position.y + xoco.box.size.y / 2 > 3.0f) {
            xoco.movement.y = xoco.movement.y * -1;
        } else if (xoco.box.position.y - xoco.box.size.y / 2 < -3.0f) {
            xoco.movement.y = xoco.movement.y * -1;
        }
        // collision sides
        if (xoco.box.position.x + xoco.box.size.x / 2 > 5.0f) {
            xoco.movement.x = xoco.movement.x * -1;
        } else if (xoco.box.position.x - xoco.box.size.x / 2 < -5.0f) {
            xoco.movement.x = xoco.movement.x * -1;
        }
        // collision with kibble
        float xdistX = fabs(xoco.box.position.x - ball.box.position.x) - ((xoco.box.size.x + ball.box.size.x) / 2.0f);
        float ydistX = fabs(xoco.box.position.y - ball.box.position.y) - ((xoco.box.size.y + ball.box.size.y) / 2.0f);
        if (xdistX < 0 && ydistX < 0) {
            ball.movement.x = ball.movement.x * -1;
            ball.movement.y = ball.movement.y * -1;      // reverses direction of ball
            xoco.movement.x = xoco.movement.x * -1;
            xoco.movement.y = xoco.movement.y * -1;      // reverses direction of xoco
        }
        // position
        xoco.position += xoco.movement * xoco.speed * deltaTime;
        xoco.box.position = xoco.position;
        xoco.matrix = glm::mat4(1.0f);
        xoco.matrix = glm::translate(xoco.matrix, xoco.position);
        // rotation
        if (xoco.movement.y == -1 && xoco.movement.x == 0) {
            xoco.rotation = 180.0f;
            xoco.box.size = glm::vec3(0.5f, 1.0f, 0.0f);
        } else if (xoco.movement.y == 1 && xoco.movement.x == 0) {
            xoco.rotation = 0.0f;
            xoco.box.size = glm::vec3(0.5f, 1.0f, 0.0f);
        } else if (xoco.movement.y == 1 && xoco.movement.x == 1) {
            xoco.rotation = 315.0f;
            xoco.box.size = glm::vec3(0.5f, 0.5f, 0.0f);
        } else if (xoco.movement.y == -1 && xoco.movement.x == 1) {
            xoco.rotation = 225.0f;
            xoco.box.size = glm::vec3(0.5f, 0.5f, 0.0f);
        }
        if (xoco.movement.x == -1 && xoco.movement.y == 0) {
            xoco.rotation = 90.0f;
            xoco.box.size = glm::vec3(1.0f, 0.5f, 0.0f);
        } else if (xoco.movement.x == 1 && xoco.movement.y == 0) {
            xoco.rotation = 270.0f;
            xoco.box.size = glm::vec3(1.0f, 0.5f, 0.0f);
        } else if (xoco.movement.x == -1 && xoco.movement.y == 1) {
            xoco.rotation = 45.0f;
            xoco.box.size = glm::vec3(0.5f, 0.5f, 0.0f);
        } else if (xoco.movement.x == -1 && xoco.movement.y == -1) {
            xoco.rotation = 135.0f;
            xoco.box.size = glm::vec3(0.5f, 0.5f, 0.0f);
        }
        xoco.matrix = glm::rotate(xoco.matrix, glm::radians(xoco.rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        // scale
        xoco.matrix = glm::scale(xoco.matrix, glm::vec3(1.5f, 1.5f, 1.0f));
    } else {
        xoco.movement = glm::vec3(0.0f, 0.0f, 0.0f);
    }
}

void drawPaddle(struct Paddle paddle) {
    program.SetModelMatrix(paddle.matrix);
    glBindTexture(GL_TEXTURE_2D, paddle.textureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void drawBall(struct Ball ball) {
    program.SetModelMatrix(ball.matrix);
    glBindTexture(GL_TEXTURE_2D, ball.textureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void drawXoco(struct Xoco xoco) {
    program.SetModelMatrix(xoco.matrix);
    glBindTexture(GL_TEXTURE_2D, xoco.textureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void drawGround(struct Tile tile) {
    program.SetModelMatrix(tile.matrix);
    glBindTexture(GL_TEXTURE_2D, tile.textureID);
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
    
    drawPaddle(paddleOne);
    drawPaddle(paddleTwo);
    drawBall(ball);
    drawXoco(xoco);
    for (int i = 0; i < NUM_OF_TILES; i++) {
        drawGround(tileTop[i]);
        drawGround(tileBottom[i]);
    }
    
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
