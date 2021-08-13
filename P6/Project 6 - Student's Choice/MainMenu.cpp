#include "MainMenu.h"

GLuint mainmenufontTextureID;

void MainMenu::Initialize() {
    state.nextScene = -1;
    state.player = new Entity();

    mainmenufontTextureID = Util::LoadTexture("font1.png");
    glClearColor(0.2f, 0.2f, 0.6f, 1.0f);

}

void MainMenu::Update(float deltaTime) {
}

void MainMenu::Render(ShaderProgram* program) {
    glClear(GL_COLOR_BUFFER_BIT);

    Util::DrawText(program, mainmenufontTextureID, "Project 6 - Student's Choice", 0.7f, -0.4f, glm::vec3(1, -3.5f, 0));
}