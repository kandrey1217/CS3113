#pragma once
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
#include "Map.h"
#include <SDL_mixer.h>

#define MAX_HP 15

enum EntityType { PLAYER, ENEMY, DOOR, KEY, HEALTH_PACK };
enum AIType { WALKER, CHASER, PATROLLER };
enum AIState { IDLE, WALKING, WALKING_RIGHT, ATTACKING, EATING };

class Entity {
public:
    EntityType entityType;
    AIType aiType;
    AIState aiState;

    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 acceleration;
    glm::vec3 velocity;

    Entity* lastCollided = NULL;

    float width = 1;
    float height = 1;

    float speed;

    GLuint textureID;

    glm::mat4 modelMatrix;

    bool isActive = true;

    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;

    bool has_key = false;

    int hp = 10;
    int attack_dmg;
    bool attacking = false;


    int* animRight = NULL;
    int* animLeft = NULL;
    int* animUp = NULL;
    int* animDown = NULL;

    int* animAttackRight = NULL;
    int* animAttackLeft = NULL;

    int* animIndices = NULL;
    int animFrames = 0;
    int animIndex = 0;
    float animTime = 0;
    int animCols = 0;
    int animRows = 0;

    float jumpPower;
    bool jump = false;

    Entity();

    bool CheckCollision(Entity* other);
    void CheckCollisionsY(Entity* objects, int objectCount);
    void CheckCollisionsX(Entity* objects, int objectCount);
    void CheckCollisionsX(Map* map);
    void CheckCollisionsY(Map* map);
    void CheckCollisions(Entity* object);

    void Update(float deltaTime, Entity* player, Entity* objects, int objectCount, Map* map);
    void Render(ShaderProgram* program);
    void DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index);

    void AI(Entity* player);
    void AIWalker();
    void AIChaser(Entity* player);
    void AIPatroller();
};
