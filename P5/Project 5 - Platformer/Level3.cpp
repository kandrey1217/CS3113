#include "Level3.h"

#define LEVEL3_WIDTH 20
#define LEVEL3_HEIGHT 8

#define LEVEL3_ENEMIES_COUNT 3  

unsigned int level3_data[] =
{
3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0,
3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3, 4, 0, 0, 0, 0, 0,
3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3, 3, 3, 4, 0, 0, 0, 0,
3, 0, 0, 0, 0, 0, 0, 4, 0, 0, 4, 3, 3, 3, 3, 3, 4, 0, 0, 7,
3, 4, 4, 0, 4, 4, 0, 0, 0, 4, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4,
3, 3, 3, 0, 3, 3, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
};

GLuint level3fontTextureID;

void Level3::Initialize() {
    state.nextScene = -1;

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);


    GLuint mapTextureID = Util::LoadTexture("tile_sheet.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 8, 1);
    // Move over all of the player and enemy code from initialization.
    // Initialize Game Objects

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;

    state.player->position = glm::vec3(1, 0, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -40.0f, 0);
    state.player->speed = 3.0f;
    state.player->textureID = Util::LoadTexture("character.png");


    state.player->animRight = new int[4]{ 12, 13, 14, 15 };//right
    state.player->animLeft = new int[4]{ 8, 9, 10, 11 };//left
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

    state.player->jumpPower = 11.5f;

    //initialize enemies
    state.enemies = new Entity[LEVEL3_ENEMIES_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("enemy.png");

    for (int i = 0; i < LEVEL3_ENEMIES_COUNT; ++i) {
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

    //enemy 1 - chasing
    state.enemies[0].position = glm::vec3(9.0f, -3.5f, 0);
    state.enemies[0].aiType = CHASER;
    state.enemies[0].aiState = IDLE;

    //enemy 2 - chasing
    state.enemies[1].position = glm::vec3(13.0f, 0, 0);
    state.enemies[1].aiType = CHASER;
    state.enemies[1].aiState = IDLE;

    //enemy 3 - chasing
    state.enemies[2].position = glm::vec3(17.0f, -3.5f, 0);
    state.enemies[2].aiType = CHASER;
    state.enemies[2].aiState = IDLE;

    for (int i = 0; i < LEVEL3_ENEMIES_COUNT; ++i) {
        state.enemies[i].Update(0, state.player, state.enemies, LEVEL3_ENEMIES_COUNT, state.map);
    }

    level3fontTextureID = Util::LoadTexture("font1.png");
}

void Level3::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMIES_COUNT, state.map);

    if (state.lives <= 0) {   //game over, despawn player/enemies
        state.player->isActive = false;

        for (int i = 0; i < LEVEL3_ENEMIES_COUNT; ++i) {
            state.enemies[i].isActive = false;
        }
    }
    else if (state.player->lastCollided && state.player->lastCollided->isActive == true) {  //player collided with an enemy
        state.player->lastCollided->isActive = false;

        if (state.player->collidedTop || state.player->collidedLeft || state.player->collidedRight) {   //player was hit, loses a life
            --state.lives;
        }
    }
    else if (state.player->position.x >= 18) {  //player won, despawn enemies
        for (int i = 0; i < LEVEL3_ENEMIES_COUNT; ++i) {
            state.enemies[i].isActive = false;
        }
    }

    if (state.player->position.y <= -10.0f) {    //player fell, respawn at start
        state.player->position = glm::vec3(1, -4.0f, 0);
        --state.lives;
    }

    for (int i = 0; i < LEVEL3_ENEMIES_COUNT; ++i) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMIES_COUNT, state.map);
    }
}

void Level3::Render(ShaderProgram* program) {
    state.map->Render(program);
    state.player->Render(program);

    if (state.lives <= 0) {
        Util::DrawText(program, level3fontTextureID, "You lose", 0.8f, -0.45f, state.player->position);
    }

    for (int i = 0; i < LEVEL3_ENEMIES_COUNT; ++i) {
        state.enemies[i].Render(program);
    }

    if (state.player->position.x >= 18) {
        Util::DrawText(program, level3fontTextureID, "You win", 0.8f, -0.45f, state.player->position);
    }
}