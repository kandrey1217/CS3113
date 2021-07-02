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

#define PLATFORM_COUNT 29

#include <vector>

struct GameState {
    Entity* player;
    Entity* platforms;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint fontTextureID;
GLuint wallTextureID;
GLuint platformTextureID;

bool missionSuccess = false;
bool missionFailed = false;

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
    displayWindow = SDL_CreateWindow("Project 3 - Lunar Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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
    state.player->position = glm::vec3(0, 3.5f, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -0.15f, 0);
    state.player->speed = 1.5f;
    state.player->textureID = LoadTexture("player.png");

    state.player->height = 0.9f;
    state.player->width = 0.7f;

    state.platforms = new Entity[PLATFORM_COUNT];
    wallTextureID = LoadTexture("platformPack_tile041.png");
    platformTextureID = LoadTexture("platformPack_tile042.png");

    //floor
    state.platforms[0].textureID = wallTextureID;
    state.platforms[0].position = glm::vec3(-4.5f, -3.25f, 0);

    state.platforms[1].textureID = wallTextureID;
    state.platforms[1].position = glm::vec3(-3.5f, -3.25f, 0);

    state.platforms[2].textureID = wallTextureID;
    state.platforms[2].position = glm::vec3(-2.5f, -3.25f, 0);

    state.platforms[3].textureID = platformTextureID;
    state.platforms[3].position = glm::vec3(-1.5f, -3.25f, 0);

    state.platforms[4].textureID = platformTextureID;
    state.platforms[4].position = glm::vec3(-0.5f, -3.25f, 0);

    state.platforms[5].textureID = platformTextureID;
    state.platforms[5].position = glm::vec3(0.5f, -3.25f, 0);

    state.platforms[6].textureID = platformTextureID;
    state.platforms[6].position = glm::vec3(1.5f, -3.25f, 0);

    state.platforms[7].textureID = wallTextureID;
    state.platforms[7].position = glm::vec3(2.5f, -3.25f, 0);

    state.platforms[8].textureID = wallTextureID;
    state.platforms[8].position = glm::vec3(3.5f, -3.25f, 0);

    state.platforms[9].textureID = wallTextureID;
    state.platforms[9].position = glm::vec3(4.5f, -3.25f, 0);

    //left wall
    state.platforms[10].textureID = wallTextureID;
    state.platforms[10].position = glm::vec3(-4.5f, -2.25f, 0);

    state.platforms[11].textureID = wallTextureID;
    state.platforms[11].position = glm::vec3(-4.5f, -1.25f, 0);

    state.platforms[12].textureID = wallTextureID;
    state.platforms[12].position = glm::vec3(-4.5f, -0.25f, 0);

    state.platforms[13].textureID = wallTextureID;
    state.platforms[13].position = glm::vec3(-4.5f, 0.75f, 0);

    state.platforms[14].textureID = wallTextureID;
    state.platforms[14].position = glm::vec3(-4.5f, 1.75f, 0);

    state.platforms[15].textureID = wallTextureID;
    state.platforms[15].position = glm::vec3(-4.5f, 2.75f, 0);

    state.platforms[16].textureID = wallTextureID;
    state.platforms[16].position = glm::vec3(-4.5f, 3.75f, 0);

    //left ledge
    state.platforms[17].textureID = wallTextureID;
    state.platforms[17].position = glm::vec3(-3.5f, 1.75f, 0);

    state.platforms[18].textureID = wallTextureID;
    state.platforms[18].position = glm::vec3(-2.5f, 1.75f, 0);

    //middle ledge
    state.platforms[19].textureID = wallTextureID;
    state.platforms[19].position = glm::vec3(0);

    state.platforms[20].textureID = wallTextureID;
    state.platforms[20].position = glm::vec3(1, 0, 0);

    state.platforms[21].textureID = wallTextureID;
    state.platforms[21].position = glm::vec3(2, 0, 0);

    //right wall
    state.platforms[22].textureID = wallTextureID;
    state.platforms[22].position = glm::vec3(4.5f, -2.25f, 0);

    state.platforms[23].textureID = wallTextureID;
    state.platforms[23].position = glm::vec3(4.5f, -1.25f, 0);

    state.platforms[24].textureID = wallTextureID;
    state.platforms[24].position = glm::vec3(4.5f, -0.25f, 0);

    state.platforms[25].textureID = wallTextureID;
    state.platforms[25].position = glm::vec3(4.5f, 0.75f, 0);

    state.platforms[26].textureID = wallTextureID;
    state.platforms[26].position = glm::vec3(4.5f, 1.75f, 0);

    state.platforms[27].textureID = wallTextureID;
    state.platforms[27].position = glm::vec3(4.5f, 2.75f, 0);

    state.platforms[28].textureID = wallTextureID;
    state.platforms[28].position = glm::vec3(4.5f, 3.75f, 0);


    for (int i = 0; i < PLATFORM_COUNT; ++i) {
        state.platforms[i].Update(0, NULL, 0);
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
                break;
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        if (state.player->collidedBottom == false) {
            state.player->acceleration.x += -0.1f;
        }
        else {
            state.player->acceleration.x = 0;
        }
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        if (state.player->collidedBottom == false) {
            state.player->acceleration.x += 0.1f;
        }
        else {
            state.player->acceleration.x = 0;
        }    
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
        state.player->Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT);

        deltaTime -= FIXED_TIMESTEP;
    }

    accumulator = deltaTime;

    //if player reaches landing zone
    if (state.player->collidedBottom && state.player->lastCollided->textureID == platformTextureID) {
        missionSuccess = true;
    }

    //if player hits a wall/things outside the landing zone
    if ((state.player->collidedLeft || state.player->collidedRight || state.player->collidedBottom) && state.player->lastCollided->textureID == wallTextureID) {
        missionFailed = true;
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < PLATFORM_COUNT; ++i) {
        state.platforms[i].Render(&program);
    }

    state.player->Render(&program);
    //failure message
    if (missionFailed) {
        DrawText(&program, fontTextureID, "Mission Failed", 0.8f, -0.45f, glm::vec3(-2.25f, 0, 0));

    }
    
    //success message
    else if (missionSuccess) {
        DrawText(&program, fontTextureID, "Mission Successful", 0.8f, -0.45f, glm::vec3(-2.75, 0, 0));
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
