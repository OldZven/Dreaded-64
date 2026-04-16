#include "Headers/player.h"
#include "Headers/raycaster.h"
#include <iostream>
#include <cmath>

Player::Player(float startX, float startY, float startAngle, int tSize, int mWidth, int mLength, int sWidth, int sHeight, resources* res)
    : x(startX), y(startY), angle(startAngle), tileSize(tSize), mapWidth(mWidth), mapLength(mLength), screenWidth(sWidth), screenHeight(sHeight), resourcesInstance(res)
{
    health = 100.0f;
    arrowCount = 20;
    idleTimer = 0.0f;
    walkTimer = 0.0f;
    isFirstAttack = true;
    isReloaded = true;
    currentlyBlocking = false;
    meleeType = 1;
    endGame = false;
    isRunning = false;
    isWalking = false;
    openTempMenu = false;
    menuKeyHeld = false;
}

bool Player::canMoveTo(float nx, float ny, const int* map) {
    int tileX = int(nx / tileSize);
    int tileY = int(ny / tileSize);
    if (tileX < 0 || tileX >= mapWidth || tileY < 0 || tileY >= mapLength)
        return false;
    int tile = map[tileY * mapWidth + tileX];
    return tile == 0 || tile == 2;
}
bool Player::collidesWithEnemy(float oldX, float oldY,
    float newX, float newY,
    const std::vector<std::unique_ptr<Enemy>>& enemies)
{
    for (const auto& e : enemies)
    {
        if (!e->isAlive()) continue;

        float dxOld = oldX - e->getX();
        float dyOld = oldY - e->getY();
        float distOld = std::sqrt(dxOld * dxOld + dyOld * dyOld);

        float dxNew = newX - e->getX();
        float dyNew = newY - e->getY();
        float distNew = std::sqrt(dxNew * dxNew + dyNew * dyNew);

        // If new distance is smaller, AND inside collision radius → block
        if (distNew < distOld && distNew < (radius + e->getRadius()))
            return true;
    }
    return false;
}

void Player::move(float dx, float dy, const int* map,
    const std::vector<std::unique_ptr<Enemy>>& enemies)
{
    float oldX = x;
    float oldY = y;

    // Try X movement
    float tryX = x + dx;
    if (canMoveTo(tryX, y, map) &&
        !collidesWithEnemy(oldX, oldY, tryX, y, enemies))
    {
        x = tryX;
    }

    // Try Y movement
    float tryY = y + dy;
    if (canMoveTo(x, tryY, map) &&
        !collidesWithEnemy(oldX, oldY, x, tryY, enemies))
    {
        y = tryY;
    }
}



void Player::updateArrows(const int* map, float dt) {
    for (auto& arrow : arrows) {
        if (!arrow.alive) continue;
        arrow.x += std::cos(arrow.angle) * arrow.speed * dt;
        arrow.y += std::sin(arrow.angle) * arrow.speed * dt;
        int tileX = int(arrow.x / tileSize);
        int tileY = int(arrow.y / tileSize);
        if (tileX < 0 || tileX >= mapWidth || tileY < 0 || tileY >= mapLength || map[tileY * mapWidth + tileX] == 1)
            arrow.alive = false;
    }
    arrows.erase(std::remove_if(arrows.begin(), arrows.end(), [](const Arrow& a) { return !a.alive; }), arrows.end());
}

void Player::handlerUpdater(float dt, const int* map, const std::vector<std::unique_ptr<Enemy>>& enemies) {
    if (isDead == false) {
        handleInput(dt, map, enemies);
    }
    else {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            endGame = true;
        }
    }
    update(dt, map);
    handleIdleTimer(dt);
    handleCaps();
}

void Player::walking(float dt) {
    if (resourcesInstance == nullptr) return;
    walkTimer += dt;
    if (walkTimer > 0.4f) {
        if (resourcesInstance->playerWalkSound.getStatus() != sf::Sound::Playing) {
            resourcesInstance->playerWalkSound.play();
        }
        walkTimer = 0.0f;
    }
}
void Player::running(float dt) {
    if (resourcesInstance == nullptr) return;
    walkTimer += dt;
    if (walkTimer > 0.2f) {
        if (resourcesInstance->playerWalkSound.getStatus() != sf::Sound::Playing) {
            resourcesInstance->playerWalkSound.play();
        }
        walkTimer = 0.0f;
    }
}


//Handling of inputs, timers and caps
void Player::handleInput(float dt, const int* map, const std::vector<std::unique_ptr<Enemy>>& enemies) {
    float playerSpeed = speed[0];
    isWalking = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
        playerSpeed = speed[1];
        running(dt);
        isWalking = true;
    }
    

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        move(std::cos(angle) * playerSpeed * dt, std::sin(angle) * playerSpeed * dt, map, enemies);
        playerIdleTexture();
        walking(dt);
        isWalking = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        move(-std::cos(angle) * playerSpeed * dt, -std::sin(angle) * playerSpeed * dt, map, enemies);
        playerIdleTexture();
        walking(dt);
        isWalking = true;
    }
        
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        walking(dt);
        move(std::sin(angle) * speed[0] * dt, -std::cos(angle) * speed[0] * dt, map, enemies);
        isWalking = true;
    }

       
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        walking(dt);
        move(-std::sin(angle) * speed[0] * dt, std::cos(angle) * speed[0] * dt, map, enemies);
        isWalking = true;
    }
        

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        angle -= speed[2] * dt;
        playerLeftTexture();
    }
        
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        angle += speed[2] * dt;
        playerRightTexture();
    }
        
    if (!isWalking && resourcesInstance->playerWalkSound.getStatus() == sf::Sound::Playing) {
        resourcesInstance->playerWalkSound.stop();
    }

    normalizeAngle(angle);


    //---Attacking Controls---
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
        startRangedAttack();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::U) && !isMeleeAttacking) {
        startMeleeAttack();//1 - Standard Attack
        meleeType = 1;
        resourcesInstance->meleeStandardSound.play();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::H) && !isMeleeAttacking) {
        startMeleeAttack();
        meleeType = 2;//2 - Heavy Attack
        resourcesInstance->meleeHeavySound.play();
    }

    //End game
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        endGame = true;
    }
    //Change Level
    /*
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
        changingLevel = true;
    }
    */
    
}
void Player::handleIdleTimer(float dt) {
    if (idleTimer < 0) idleTimer = 0;
    idleTimer -= dt;
}
void Player::handleCaps() {
    if (health > 100) {
        health = 100;
    }
    else if (health < 0) {
        health = 0;
    }
    if (arrowCount > 999) {
        arrowCount = 999;
    }
    else if (arrowCount < 0) {
        arrowCount = 0;
    }
}

//Starting of Attakcs
void Player::startMeleeAttack() {
    isMeleeAttacking = true;
    attackMeleeTimer = 0.0f;
    hasDealtMeleeDamage = false; 

}
void Player::startRangedAttack() {
    isRangedAttacking = true;
    attackRangedTimer = 0.0f;
}

//Handling of Right Arm
void Player::handleSword(float dt, float idleTimer) {
    // --- Melee Attacking Animation----
    if (isMeleeAttacking == true) {
        attackMeleeTimer += dt;
        if (meleeType == 1) {
            if (isFirstAttack == true) {
                if (attackMeleeTimer < 0.4f) {
                    rightArmFrame = 1;
                    playerAttackTexture();
                }       // Attack start
                else if (attackMeleeTimer < 0.8f) rightArmFrame = 2;  // Mid swing
                else if (attackMeleeTimer < 1.2f) rightArmFrame = 3;  // End swing
                else {
                    isMeleeAttacking = false;
                    isFirstAttack = false;
                    idleTimer = 1.8f;
                }
            }
            else {
                if (attackMeleeTimer < 0.4f) {
                    playerAttackTexture();
                    rightArmFrame = 4;
                }     // Attack start
                else if (attackMeleeTimer < 0.8f) rightArmFrame = 5;  // Mid swing
                else if (attackMeleeTimer < 1.2f) rightArmFrame = 6;  // End swing
                else {
                    isMeleeAttacking = false;
                    isFirstAttack = true;
                    idleTimer = 1.8f;
                }
            }
        }
        else if (meleeType == 2) {
            if (attackMeleeTimer < 0.8f) {
                playerAttackTexture();
                rightArmFrame = 7;
            }       // Attack start
            else if (attackMeleeTimer < 2.0f) rightArmFrame = 8;  // Attack End
            else {
                isMeleeAttacking = false;
                idleTimer = 2.2f;
            }
        }
    }
    bool inLightHitWindow = (attackMeleeTimer > 0.4f && attackMeleeTimer < 0.8f);
    bool inHeavyHitWindow = (attackMeleeTimer > 0.8f && attackMeleeTimer < 1.6f);

    if (isMeleeAttacking == false && idleTimer <= 0.0f) {
        rightArmFrame = 0;
    }
        
}
void Player::handleRightArm(float dt, float idleTimer) {
    handleSword(dt, idleTimer);
}

//Handling of Left Arm
void Player::handleBlock() {
    currentlyBlocking = false;
    if (isRangedAttacking == false) {
        if (isReloaded == true) {
            leftArmFrame = 0;
        }
        else {
            leftArmFrame = 1;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
            currentlyBlocking = true;
            leftArmFrame = 2;
        }
    }
}
void Player::handleCrossbow(float dt) {
    if (isRangedAttacking == true && currentlyBlocking == false) {
        attackRangedTimer += dt;
        if (isReloaded == true && arrowCount > 0) {
            resourcesInstance->rangedAttackSound.play();
            leftArmFrame = 1;
            playerAttackTexture();
            //---Arrow Processing---
            if (arrows.size() < 1000 && arrowCount > 0) {
                Arrow a;
                a.alive = true;
                a.x = x;
                a.y = y;
                a.angle = angle;
                a.speed = 80.0f;
                arrows.push_back(a);
                arrowCount--;
                if (arrowCount < 0) {
                    arrowCount = 0;
                }
            }
            isRangedAttacking = false;
            isReloaded = false;
        } else if (isReloaded == false && arrowCount >0){
            handleReload();
        }
    }
}
void Player::handleLeftArm(float dt) {
    handleBlock();
    handleCrossbow(dt);
}
void Player::handleInteract() {

}

//Uses both arms
void Player::handleReload() {
    if (attackRangedTimer < 0.8f) {
        rightArmFrame = 9;
    }
    else if (attackRangedTimer < 1.5f) {
        resourcesInstance->reloadSound.play();
        leftArmFrame = 0;
        isRangedAttacking = false;
        isReloaded = true;
    }
}


//Checks to turn into Idle Player Texture
void Player::checkPlayerTexture(float dt) {
        playerTextureTimer += dt;
        if (playerTextureTimer >= 1.0f && playerFrame != 0) {
            playerIdleTexture();
        }
}

//Handling of Player Textures
void Player::playerAttackTexture() {
        playerResetTexture();
        playerFrame = 3;
}
void Player::playerIdleTexture() {
        playerResetTexture();
        playerFrame = 0;
}
void Player::playerHitPlayerTexture() {
        playerResetTexture();
        playerFrame = 5;
        if (health > 0) {
               resourcesInstance->hitPlayerSound.play();
        }
}
void Player::playerHitEnemyTexture() {
        playerResetTexture();
        playerFrame = 4;
        resourcesInstance->hitEnemySound.play();
}
void Player::playerDeathTexture() {
        playerResetTexture();
        playerFrame = 6;
        resourcesInstance->playerDeathSound.play();
}
void Player::playerLeftTexture() {
    playerResetTexture();
    playerFrame = 1;
}
void Player::playerRightTexture() {
    playerResetTexture();
    playerFrame = 2;
}
void Player::playerResetTexture() {
    playerTextureTimer = 0.0f;
}

void Player::playerHit(float damage) {
    health -= damage;
    playerHitPlayerTexture();
}
void Player::playerHeal(float heal) {
    health += heal;
}
void Player::pickUpArrows(int pickedArrows) {
    arrowCount += pickedArrows;
}
void Player::playerStatus() {
    if (health <= 0) {
        if (isDead == false) {
            playerDeathTexture();
            isDead = true;
        }
    }
}

void Player::handleMenu() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab)) {
        if (!menuKeyHeld) {
            openTempMenu = !openTempMenu; // toggle once per press
            menuKeyHeld = true;            // mark as pressed
        }
    }
    else {
        menuKeyHeld = false; // reset when key is released
    }
    
}

void Player::detectDoor(const int* map) {
    int tileX = int(x / tileSize);
    int tileY = int(y / tileSize);

    if (map[tileY * mapWidth + tileX] == 2)
    {
        changingLevel = true;
    }
}

void Player::update(float dt, const int* map) {
    handleIdleTimer(dt);
    playerStatus();
    detectDoor(map);

    checkPlayerTexture(dt);

    updateArrows(map, dt);

    handleRightArm(dt, idleTimer);
    handleLeftArm(dt);

    handleMenu();
}



