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

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix1, modelMatrix2, modelMatrix3, projectionMatrix;

//position and movement of the ball and left/right paddles
glm::vec3 ball_position = glm::vec3(0, 0, 0);
glm::vec3 ball_movement = glm::vec3(0, 0, 0);

glm::vec3 left_paddle_position = glm::vec3(-5, 0, 0);
glm::vec3 left_paddle_movement = glm::vec3(0, 0, 0);

glm::vec3 right_paddle_position = glm::vec3(5, 0, 0);
glm::vec3 right_paddle_movement = glm::vec3(0, 0, 0);

float prev_ticks = 0.0f;
float paddle_speed = 3.0f;
float ball_speed = 5.0f;

float ball_radius = 0.25f;
float paddle_w = 1.0f;
float paddle_h = 3.0f;

bool game_started = false;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Project 2 - Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix1 = glm::mat4(1.0f);
    modelMatrix2 = glm::mat4(1.0f);
    modelMatrix3 = glm::mat4(1.0f);

    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(0.0f, 0.5f, 0.8f, 1.0f);

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

bool box_box_collision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
    float x_diff = fabs(x2 - x1);
    float y_diff = fabs(y2 - y1);

    float x_dist = x_diff - ((w1 + w2) / 2);
    float y_dist = y_diff - ((h1 + h2) / 2);

    if (x_dist < 0.0f && y_dist < 0.0f) {
        return true;
    }
    return false;
}

void ProcessInput() {
    //reset ball and paddle movement
    left_paddle_movement = glm::vec3(0);
    right_paddle_movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
            
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    //if spacebar is pressed, make the ball move
                    case SDLK_SPACE:
                        if (!game_started) {
                            ball_movement.x = 1.0f;
                            ball_movement.y = 0.25f;
                            game_started = true;
                        }
                        break;
                }
                break;
        }
    }

    //keyboard state
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    
    //controls for left paddle (W and S keys)
    if (keys[SDL_SCANCODE_W] && left_paddle_position.y < 5.0f - 2.75f) {
        left_paddle_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_S] && left_paddle_position.y > -5.0f + 2.75f) {
        left_paddle_movement.y = -1.0f;
    }
    //controls for right paddle (up and down arrow keys)
    if (keys[SDL_SCANCODE_UP] && right_paddle_position.y < 5.0f - 2.75f) {
        right_paddle_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_DOWN] && right_paddle_position.y > -5.0f + 2.75f) {
        right_paddle_movement.y = -1.0f;
    }

    //bouncing off the walls
    if (ball_position.y > 5.0f - 2.0f + ball_radius * 2) {
        ball_movement.y *= -1;
    }
    if (ball_position.y < -5.0f + 2.0f - ball_radius * 2) {
        ball_movement.y *= -1;
    }

    std::cout << "GAME IN PROGRESS";
    
    //bouncing off the paddles
    if (box_box_collision(left_paddle_position.x, left_paddle_position.y, paddle_w, paddle_h, ball_position.x, ball_position.y, ball_radius*2, ball_radius*2)) { 
        ball_movement.x *= -1;
    }
    if (box_box_collision(right_paddle_position.x, right_paddle_position.y, paddle_w, paddle_h, ball_position.x, ball_position.y, ball_radius*2, ball_radius*2)) {
        ball_movement.x *= -1;
    }

    //game over - ball hits left/right walls
    if (ball_position.x > 3.75f + 1.0f) {   //player 1 (left side) wins
        ball_movement = glm::vec3(0, 0, 0); //freezes the ball in place
    }
    if (ball_position.x < -3.75f - 1.0f) {  //player 2 (right side) wins
        ball_movement = glm::vec3(0, 0, 0);
    }

    //normalizes ball and paddle movement
    if (glm::length(left_paddle_movement) > 1.0f) {
        left_paddle_movement = glm::normalize(left_paddle_movement);
    }
    if (glm::length(right_paddle_movement) > 1.0f) {
        right_paddle_movement = glm::normalize(right_paddle_movement);
    }
    if (glm::length(ball_movement) > 1.0f) {
        ball_movement = glm::normalize(ball_movement);
    }
}

void Update() {
    //calculate delta time
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float delta_time = ticks - prev_ticks;
    prev_ticks = ticks;
    
    //calculate ball and paddle positions
    ball_position += ball_movement * ball_speed * delta_time;
    left_paddle_position += left_paddle_movement * paddle_speed * delta_time;
    right_paddle_position += right_paddle_movement * paddle_speed * delta_time;

    //update ball and paddle positions
    modelMatrix1 = glm::mat4(1.0f);
    modelMatrix1 = glm::translate(modelMatrix1, ball_position);

    modelMatrix2 = glm::mat4(1.0f);
    modelMatrix2 = glm::translate(modelMatrix2, left_paddle_position);

    modelMatrix3 = glm::mat4(1.0f);
    modelMatrix3 = glm::translate(modelMatrix3, right_paddle_position);

    //paddle out of screen bounds checking
    if (modelMatrix2[0].y > 1.0f) {
        modelMatrix2[0].y = 1.0f;
    }
    if (modelMatrix2[1].y < -1.0f) {
        modelMatrix2[1].y = -1.0f;
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    //coordinates for the ball and the paddles
    float vertices_ball[] = { -ball_radius, -ball_radius, ball_radius, -ball_radius, ball_radius, ball_radius, -ball_radius, -ball_radius, ball_radius, ball_radius, -ball_radius, ball_radius };
    float vertices_left_paddle[] = { -paddle_w / 2, -paddle_h / 2, paddle_w / 2, -paddle_h / 2, paddle_w / 2, paddle_h / 2, -paddle_w / 2, -paddle_h / 2, paddle_w / 2, paddle_h / 2, -paddle_w / 2, paddle_h / 2 };
    float vertices_right_paddle[] = {-paddle_w/2, -paddle_h/2, paddle_w/2, -paddle_h/2, paddle_w/2, paddle_h/2, -paddle_w/2, -paddle_h/2, paddle_w/2, paddle_h/2, -paddle_w/2, paddle_h/2};
    
    //draws the ball
    program.SetModelMatrix(modelMatrix1);
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices_ball);
    glEnableVertexAttribArray(program.positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);

    //draws left paddle
    program.SetModelMatrix(modelMatrix2);
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices_left_paddle);
    glEnableVertexAttribArray(program.positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);

    //draws right paddle
    program.SetModelMatrix(modelMatrix3);
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices_right_paddle);
    glEnableVertexAttribArray(program.positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);

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