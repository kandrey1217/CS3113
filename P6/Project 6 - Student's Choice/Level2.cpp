#include "Level2.h"

#define LEVEL2_WIDTH 20
#define LEVEL2_HEIGHT 10

#define LEVEL2_ENEMIES_COUNT 1

unsigned int level2_data[] =
{
536, 536, 536, 536, 536, 536, 536, 536, 536, 536, 536, 536,536, 536, 536, 536,536, 536, 536, 536,
536, 5,5,5, 5, 5, 1106, 1106, 1106, 1106, 1106, 5, 1106, 5, 5, 5, 5, 5, 5, 536,
536, 5,5, 5, 5, 5, 1106,5,5,1106,1106, 1106, 5, 5, 5, 5, 5, 5,5,536,
536, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1106, 5, 5, 5, 5, 5, 5, 536,
536, 5, 5, 5, 5, 573, 1106, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 536,
536, 5, 5, 573, 5, 5, 5, 5, 5, 1106, 5, 5, 5, 573, 5, 5, 5, 5, 5, 536,
536, 5, 5, 5, 5, 5, 5, 5, 1106, 1106, 1106, 5, 1106, 5, 5, 5, 5, 5, 5, 536,
536, 5, 5, 5, 5, 5, 5, 5, 5, 573, 5, 5, 1106, 5, 5, 5, 5, 5, 5, 536,
536, 5, 5, 573, 5, 1106, 5, 1106, 1106, 1106, 1106,1106, 5, 5, 5, 5, 5, 5, 5, 536,
536, 536, 536, 536, 536, 536, 536, 536, 536, 536, 536, 536,536, 536, 536, 536,536, 536, 536, 536 };

GLuint level2fontTextureID;

Mix_Chunk* enemy_hit2;
Mix_Chunk* enemy_dead_sfx2;
Mix_Chunk* zombie_eating_sfx2;

void Level2::Initialize() {
    state.nextScene = -1;
    state.curScene = 2;

    glClearColor(0.3f, 0.6f, 0.2f, 1.0f);

    GLuint mapTextureID = Util::LoadTexture("spritesheet.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 0.8f, 57, 31);
    // Move over all of the player and enemy code from initialization.
    // Initialize Game Objects
    state.num_enemies = LEVEL2_ENEMIES_COUNT;

    //health_pack
    state.health_pack = new Entity();
    state.health_pack->entityType = HEALTH_PACK;
    state.health_pack->position = glm::vec3(8.5, -2.5f, 0);
    state.health_pack->textureID = Util::LoadTexture("health_pack.png");

    //key
    state.key = new Entity();
    state.key->entityType = KEY;
    state.key->position = glm::vec3(7.5, -2.5, 0);
    state.key->textureID = Util::LoadTexture("keyYellow.png");

    //door
    state.door = new Entity();
    state.door->entityType = DOOR;
    state.door->position = glm::vec3(14, -3.5, 0);
    state.door->textureID = Util::LoadTexture("door.jpg");


    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;

    state.player->position = glm::vec3(1, -4.0f, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, 0, 0);
    state.player->speed = 3.5f;
    state.player->textureID = Util::LoadTexture("player.png");

    state.player->animRight = new int[8]{ 13,14,15,16,17,18,19,20 };//right
    state.player->animLeft = new int[8]{ 117, 118, 119, 120,121,122,123,124 };//left
    state.player->animUp = new int[8]{ 117, 118, 119, 120,121,122,123,124 };
    state.player->animDown = new int[8]{ 0, 1, 2, 3 };//front

    state.player->animAttackLeft = new int[8]{ 156,157,158,159,160,161,162,163 };
    state.player->animAttackRight = new int[8]{ 52, 53,54, 55,56,57,58, 59 };

    //player hp/attack
    state.player->hp = 15;
    state.player->attack_dmg = 3;

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 8;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 13;
    state.player->animRows = 16;

    state.player->height = 0.4f;
    state.player->width = 0.2f;

    state.player->jumpPower = 13.0f;

    //initialize enemies
    state.enemies = new Entity[LEVEL2_ENEMIES_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("zombiesheet.png");

    for (int i = 0; i < LEVEL2_ENEMIES_COUNT; ++i) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].movement = glm::vec3(0);
        state.enemies[i].acceleration = glm::vec3(0, 0, 0);
        state.enemies[i].speed = 0.8f;
        state.enemies[i].textureID = enemyTextureID;

        state.enemies[i].animRight = new int[3]{ 27,28,29 };//right
        state.enemies[i].animLeft = new int[3]{ 15,16,17 };//left
        state.enemies[i].animUp = new int[3]{ 39,40,41 };//back
        state.enemies[i].animDown = new int[3]{ 3,4,5 };//front

        //enemy hp/attack
        state.enemies[i].hp = 5;
        state.enemies[i].attack_dmg = 4;

        state.enemies[i].animIndices = state.enemies[i].animRight;
        state.enemies[i].animFrames = 3;
        state.enemies[i].animIndex = 0;
        state.enemies[i].animTime = 0;
        state.enemies[i].animCols = 12;
        state.enemies[i].animRows = 8;

        state.enemies[i].height = 1.0f;
        state.enemies[i].width = 0.5f;
    }

    //enemy 1
    state.enemies[0].position = glm::vec3(7.0f, -3.5f, 0);
    state.enemies[0].aiType = CHASER;
    state.enemies[0].aiState = IDLE;


    for (int i = 0; i < LEVEL2_ENEMIES_COUNT; ++i) {
        state.enemies[i].Update(0, state.player, state.enemies, LEVEL2_ENEMIES_COUNT, state.map);
    }

    level2fontTextureID = Util::LoadTexture("font1.png");

    enemy_hit2 = Mix_LoadWAV("enemy_hit.wav");
    Mix_VolumeChunk(enemy_hit2, MIX_MAX_VOLUME / 4);

    enemy_dead_sfx2 = Mix_LoadWAV("enemy_dead.wav");
    Mix_VolumeChunk(enemy_dead_sfx2, MIX_MAX_VOLUME / 2);

    zombie_eating_sfx2 = Mix_LoadWAV("zombie_chew.wav");
    Mix_VolumeChunk(enemy_dead_sfx2, MIX_MAX_VOLUME / 10);
}

void Level2::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMIES_COUNT, state.map);

    //health pack
    state.health_pack->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMIES_COUNT, state.map);

    if (state.health_pack->isActive && state.health_pack->lastCollided && state.health_pack->lastCollided->entityType == PLAYER) {
        if (state.player->hp + 10 < MAX_HP) {
            state.player->hp += 10;
            state.health_pack->isActive = false;
        }
        else if (state.player->hp != MAX_HP) {
            state.player->hp = MAX_HP;
            state.health_pack->isActive = false;
        }
    }

    //key
    state.key->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMIES_COUNT, state.map);

    if (state.key->isActive && state.key->lastCollided && state.key->lastCollided->entityType == PLAYER) {
        state.key->isActive = false;
        state.player->has_key = true;
    }

    //door
    state.door->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMIES_COUNT, state.map);

    if (state.door->isActive && state.door->lastCollided && state.door->lastCollided->entityType == PLAYER) {
        if (state.player->has_key) {
            state.nextScene = 3;

        }
    }

    if (state.player->hp <= 0) {   //game over, despawn player/enemies
        state.player->isActive = false;

        for (int i = 0; i < LEVEL2_ENEMIES_COUNT; ++i) {
            state.enemies[i].isActive = false;
        }
    }
    else if (state.player->lastCollided && state.player->lastCollided->isActive == true) {  //player collides with enemy
        if (state.player->attacking) {
            Mix_PlayChannel(-1, enemy_hit2, 0); //attack sfx

            state.player->lastCollided->hp -= state.player->attack_dmg;
            state.player->attacking = false;

            if (state.player->lastCollided->hp <= 0) {
                state.player->lastCollided->isActive = false;
                --state.num_enemies;
                Mix_PlayChannel(-1, enemy_dead_sfx2, 0);
            }
        }
    }

    for (int i = 0; i < LEVEL2_ENEMIES_COUNT; ++i) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMIES_COUNT, state.map);

        if (state.enemies[i].isActive && state.enemies[i].aiState == EATING) {
            Mix_PlayChannel(-1, zombie_eating_sfx2, 0);
        }

        //scaling
        state.enemies[i].modelMatrix = glm::scale(state.enemies[i].modelMatrix, glm::vec3(0.7, 0.7, 0));
    }

    //scaling health_pack
    state.health_pack->modelMatrix = glm::scale(state.health_pack->modelMatrix, glm::vec3(0.7, 0.7, 0));

    //scaling key
    state.key->modelMatrix = glm::scale(state.key->modelMatrix, glm::vec3(0.7, 0.7, 0));

    //scaling door
    state.door->modelMatrix = glm::scale(state.door->modelMatrix, glm::vec3(0.7, 0.7, 0));
}

void Level2::Render(ShaderProgram* program) {
    state.map->Render(program);
    state.player->Render(program);
    state.health_pack->Render(program);
    state.key->Render(program);
    state.door->Render(program);

    //player hp ui
    if (state.player->isActive) {
        Util::DrawText(program, level2fontTextureID, "HP: " + std::to_string(state.player->hp), 0.3f, -0.2f, glm::vec3(state.player->position.x - 0.22f, state.player->position.y + 0.4f, state.player->position.z));
    }

    //player dead, game over
    if (state.player->hp <= 0) {
        Util::DrawText(program, level2fontTextureID, "You lose", 0.8f, -0.45f, state.player->position);
    }

    for (int i = 0; i < LEVEL2_ENEMIES_COUNT; ++i) {
        state.enemies[i].Render(program);

        //enemy hp ui
        if (state.enemies[i].isActive) {
            Util::DrawText(program, level2fontTextureID, "HP: " + std::to_string(state.enemies[i].hp), 0.3f, -0.2f, glm::vec3(state.enemies[i].position.x - 0.2f, state.enemies[i].position.y + 0.5f, state.enemies[i].position.z));
        }
    }
}