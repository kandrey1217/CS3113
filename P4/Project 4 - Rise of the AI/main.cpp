#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"

#define PLATFORM_COUNT 15
#define ENEMIES_COUNT 3

#include <vector>

struct GameState {
    Entity* player;
    Entity* platforms;
    Entity* enemies;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint fontTextureID;
GLuint platformTextureID;
GLuint enemyTextureID;

bool missionFailed = false;
int enemiesLeft;

void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text,
    float size, float spacing, glm::vec3 position)
{
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;

    std::vector<float> vertices;
    std::vector<float> texCoords;

    for (int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;

        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
     
        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
        });

        texCoords.insert(texCoords.end(), {
            u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height,
        });
    } // end of for loop

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    
    glUseProgram(program->programID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}


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
    displayWindow = SDL_CreateWindow("Project 4 - Rise of the AI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Initialize Game Objects

    // Initialize Player
    state.player = new Entity();

    //eraer
    state.player->entityType = PLAYER;

    state.player->position = glm::vec3(-3.5f, -1.5f, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 1.5f;
    state.player->textureID = LoadTexture("character.png");


    state.player->animRight = new int[4]{ 12, 13, 14, 15 };//right
    state.player->animLeft = new int[4]{ 8, 9, 10, 11};//left
    state.player->animUp = new int[4]{ 4, 5, 6, 7 };//back
    state.player->animDown = new int[4]{ 0, 1, 2, 3 };//front

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;

    state.player->height = 0.75f;
    state.player->width = 0.4f;

    state.player->jumpPower = 5.5f;


    //initialize enemies
    enemiesLeft = ENEMIES_COUNT;
    state.enemies = new Entity[ENEMIES_COUNT];
    enemyTextureID = LoadTexture("enemy.png");

    for (int i = 0; i < ENEMIES_COUNT; ++i) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].movement = glm::vec3(0);
        state.enemies[i].acceleration = glm::vec3(0, -9.81f, 0);
        state.enemies[i].speed = 1.0f;
        state.enemies[i].textureID = enemyTextureID;

        state.enemies[i].animRight = new int[4]{ 2, 6, 10, 14 };//right
        state.enemies[i].animLeft = new int[4]{ 3, 7, 11, 15 };//left
        state.enemies[i].animUp = new int[4]{ 1, 5, 9, 13 };//back
        state.enemies[i].animDown = new int[4]{ 0, 4, 8, 12 };//front

        state.enemies[i].animIndices = state.enemies[i].animRight;
        state.enemies[i].animFrames = 4;
        state.enemies[i].animIndex = 0;
        state.enemies[i].animTime = 0;
        state.enemies[i].animCols = 4;
        state.enemies[i].animRows = 4;

        state.enemies[i].height = 1.0f;
        state.enemies[i].width = 0.5f;
    }

    //enemy 1 - walking
    state.enemies[0].position = glm::vec3(1.0f, -3.0f, 0);
    state.enemies[0].aiType = WALKER;
    state.enemies[0].aiState = WALKING;

    //enemy 2 - chasing
    state.enemies[1].position = glm::vec3(3.0f, -3.0f, 0);
    state.enemies[1].aiType = CHASER;
    state.enemies[1].aiState = IDLE;

    //enemy 3 - patrolling
    state.enemies[2].position = glm::vec3(-0.5f, 0, 0);
    state.enemies[2].aiType = PATROLLER;
    state.enemies[2].aiState = WALKING_RIGHT;

    //initialize platforms
    state.platforms = new Entity[PLATFORM_COUNT];
    platformTextureID = LoadTexture("sand.png");

    //floor
    state.platforms[0].textureID = platformTextureID;
    state.platforms[0].position = glm::vec3(-4.5f, -3.25f, 0);

    state.platforms[1].textureID = platformTextureID;
    state.platforms[1].position = glm::vec3(-3.5f, -3.25f, 0);

    state.platforms[2].textureID = platformTextureID;
    state.platforms[2].position = glm::vec3(-2.5f, -3.25f, 0);

    state.platforms[3].textureID = platformTextureID;
    state.platforms[3].position = glm::vec3(-1.5f, -3.25f, 0);

    state.platforms[4].textureID = platformTextureID;
    state.platforms[4].position = glm::vec3(-0.5f, -3.25f, 0);

    state.platforms[5].textureID = platformTextureID;
    state.platforms[5].position = glm::vec3(0.5f, -3.25f, 0);

    state.platforms[6].textureID = platformTextureID;
    state.platforms[6].position = glm::vec3(1.5f, -3.25f, 0);

    state.platforms[7].textureID = platformTextureID;
    state.platforms[7].position = glm::vec3(2.5f, -3.25f, 0);

    state.platforms[8].textureID = platformTextureID;
    state.platforms[8].position = glm::vec3(3.5f, -3.25f, 0);

    state.platforms[9].textureID = platformTextureID;
    state.platforms[9].position = glm::vec3(4.5f, -3.25f, 0);


    //platform
    state.platforms[10].textureID = platformTextureID;
    state.platforms[10].position = glm::vec3(-1.0f, 0, 0);

    state.platforms[11].textureID = platformTextureID;
    state.platforms[11].position = glm::vec3(0, 0, 0);

    state.platforms[12].textureID = platformTextureID;
    state.platforms[12].position = glm::vec3(1.0f, 0, 0);

    //obstacle
    state.platforms[13].textureID = platformTextureID;
    state.platforms[13].position = glm::vec3(4.5f, -2.25f, 0);

    //jump platform
    state.platforms[14].textureID = platformTextureID;
    state.platforms[14].position = glm::vec3(2.75f, -1.0f, 0);

    for (int i = 0; i < PLATFORM_COUNT; ++i) {
        state.platforms[i].Update(0, state.player, NULL, 0);
    } 

    for (int i = 0; i < ENEMIES_COUNT; ++i) {
        state.enemies[i].Update(0, state.player, NULL, 0);
    }

    //loads font texture
    fontTextureID = LoadTexture("font1.png");
}



void ProcessInput() {

    state.player->movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                // Move the player left
                break;

            case SDLK_RIGHT:
                // Move the player right
                break;

            case SDLK_SPACE:
                // Some sort of action
                if (state.player->collidedBottom) {
                    state.player->jump = true;
                }
                else {
                    //state.player->jump = true;
                }
                break;
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        state.player->movement.x = -1.0f;
        state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->movement.x = 1.0f;
        state.player->animIndices = state.player->animRight;
    }


    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        state.player->Update(FIXED_TIMESTEP, state.player, state.enemies, ENEMIES_COUNT);

        if (state.player->collidedBottom && state.player->lastCollided->entityType == ENEMY) {
            state.player->lastCollided->isActive = false;
            --enemiesLeft;
        }
        else if ((state.player->collidedLeft || state.player->collidedRight) && state.player->lastCollided->entityType == ENEMY) {
            state.player->isActive = false;
            missionFailed = true;
        }

        state.player->Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT);
            
        for (int i = 0; i < ENEMIES_COUNT; ++i) {
            state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT);
        }
          
        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < PLATFORM_COUNT; ++i) {
        state.platforms[i].Render(&program);
    }

    for (int i = 0; i < ENEMIES_COUNT; ++i) {
        state.enemies[i].Render(&program);
    }

    state.player->Render(&program);

    if (enemiesLeft == 0) {
        DrawText(&program, fontTextureID, "You win", 0.8f, -0.45f, glm::vec3(-1.0f, 0, 0));
    }
    else if (missionFailed) {
        DrawText(&program, fontTextureID, "You lose", 0.8f, -0.45f, glm::vec3(-1.0f, 0, 0));
    }

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
