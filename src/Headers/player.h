#pragma once
#include <SFML/Graphics.hpp>
#include "resources.h"
#include "enemy.h"
#include "arrow.h"
#include "menu.h"
#include <vector>
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

inline void normalizeAngle(float& angle) {
    const float TWO_PI = 2.0f * M_PI;
    while (angle < 0) angle += TWO_PI;
    while (angle >= TWO_PI) angle -= TWO_PI;
}

class Enemy;

class Player {
    std::vector<Arrow> arrows;
public:
    Player(float startX, float startY, float startAngle, int tSize, int mWidth, int mLength, int sWidth, int sHeight, resources* res);

    void handlerUpdater(float dt, const int* map, const std::vector<std::unique_ptr<Enemy>>& enemies);
    
    bool exitGame() const {return endGame; }
    bool changeLevel() const { return changingLevel; }


    std::vector<Arrow>& getArrows() { return arrows; }

    // Frame Getters
    int getLeftArmFrame() const { return leftArmFrame; }
    int getRightArmFrame() const { return rightArmFrame; }
    int getPlayerFrame() const { return playerFrame; }
    bool getOpenMenu() const { return openTempMenu; }

    // Position Getters
    float getX() const { return x; }
    float getY() const { return y; }
    float getAngle() const { return angle; }
    float getRadius() const { return radius; }

    float getIsMeleeAttacking() const { return isMeleeAttacking; }
    float getMeleeType() const { return meleeType; }
    bool getHasDealtMeleeDamage() const { return hasDealtMeleeDamage; }
    float getAttackMeleeTimer() const { return attackMeleeTimer; }




    // Position Setters
    void setX(float nx) { x = nx; }
    void setY(float ny) { y = ny; }

    void setHasDealtMeleeDamage(bool flag) { hasDealtMeleeDamage = flag; }





    // Player variable getters
    float getHealth() const { return health; }
    int getArrowCount() const { return arrowCount; }

    void playerHit(float damage);
    void playerHeal(float heal);
    void playerHitPlayerTexture();

private:

    resources* resourcesInstance;

    // Player state
    float x, y;
    float angle;
    float health;
    float radius = 0.3f;   // player hitbox size


    //Exit variable
    bool endGame;
    bool changingLevel;

    bool menuKeyHeld;
    bool openTempMenu;

    //Render frames
    int leftArmFrame = 0;
    int rightArmFrame = 0;
    int playerFrame = 0;

    // Map info
    int tileSize, mapWidth, mapLength;
    int screenWidth, screenHeight;

    // Movement
    float speed[4] = { 100.0f, 200.0f, 2.0f, 25.0f };

    // Weapons
    int arrowCount;

    // Melee/Attack timers
    float idleTimer;
    float walkTimer;
    bool isFirstAttack;
    bool isReloaded;
    bool isRunning;
    int meleeType;
    bool isMeleeAttacking = false;
    bool isRangedAttacking = false;
    float attackMeleeTimer = 0.0f;
    float attackRangedTimer = 0.0f;
    bool isWalking;
    bool hasDealtMeleeDamage = false;
    bool isDead = false;


    //Player Texture
    bool playerRecentlyChanged = false;
    float playerTextureTimer = 0.0f;

    // Blocking
    bool currentlyBlocking;

    // Helpers
    bool canMoveTo(float nx, float ny, const int* map);
    void move(float dx, float dy, const int* map, const std::vector<std::unique_ptr<Enemy>>& enemies);
    bool collidesWithEnemy(float oldX, float oldY, float newX, float newY,const std::vector<std::unique_ptr<Enemy>>& enemies);
    void updateArrows(const int* map, float dt);

    void startMeleeAttack();
    void startRangedAttack();
    void pickUpArrows(int pickedArrows);

    //Handling of controls and timers
    void handleInput(float dt, const int* map, const std::vector<std::unique_ptr<Enemy>>& enemies);
    void handleIdleTimer(float dt);
    void update(float dt, const int* map);
    void handleCaps();
    void handleMenu();

    //Handling of arms
    void handleBlock();
    void handleSword(float dt, float idleTimer);
    void handleCrossbow(float dt);
    void handleReload();
    void handleRightArm(float dt, float idleTimer);
    void handleLeftArm(float dt);
    void handleInteract();

    //handling of player
    void checkPlayerTexture(float dt);

    void playerAttackTexture();
    void playerIdleTexture();
    void playerHitEnemyTexture();
    
    void playerLeftTexture();
    void playerRightTexture();
    void playerDeathTexture();
    void playerResetTexture();

    
    void playerStatus();

    void walking(float dt);
    void running(float dt);
};
