#include "MainMenu.h"

GLuint mainmenufontTextureID;

void MainMenu::Initialize() {
    state.nextScene = -1;
    state.player = new Entity();

    mainmenufontTextureID = Util::LoadTexture("font1.png");
    glClearColor(0.7f, 0.2f, 0.2f, 1.0f);

}

void MainMenu::Update(float deltaTime) {
}

void MainMenu::Render(ShaderProgram* program) {
    glClear(GL_COLOR_BUFFER_BIT);

    Util::DrawText(program, mainmenufontTextureID, "Project 5 - Platformer", 0.8f, -0.4f, glm::vec3(1.1f, -4.0f, 0));
}