#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "resources.h"
#include "player.h"
#include "arrow.h"
#include "enemyDeathIcon.h"

class Player;
class Enemy {
protected:
    float x, y;
    float health;
    float speed;
    float angle;
    bool alive;
    float radius = 0.5f;    // enemy hitbox size
    float hitTimer = 0.0f;
    float seeTimer = 0.0f;
    float seeDuration = 2.0f;


    resources* res;

    // ---- NEW: each enemy has its own Sound instance ----
    sf::Sound meleeSound;
    sf::Sound hitSound;
    sf::Sound deathSound;
    sf::Sound walkSound;

    enum EnemyState {
        ENEMY_IDLE,
        ENEMY_WALKING,
        ENEMY_ATTACK,
        ENEMY_HIT,
        ENEMY_DEAD,
        ENEMY_RELOAD // ranged only
    };

    EnemyState state = ENEMY_IDLE;
    int animFrame = 0;
    float animTimer = 0.0f;
    float animSpeed = 0.2f; // adjust as needed

public:

    static void processDeath(std::vector<std::unique_ptr<Enemy>>& enemies,
        std::vector<std::unique_ptr<EnemyDeathIcon>>& enemyDeaths,
        resources* resource);

    // Map info
    int tileSize, mapWidth, mapLength;

    bool canChase(bool seesPlayer, float dt)
    {
        if (seesPlayer)
        {
            seeTimer = seeDuration;
            return true;
        }

        seeTimer -= dt;
        if (seeTimer < 0.0f)
            seeTimer = 0.0f;

        return seeTimer > 0.0f;
    }

    bool hasLineOfSight(float ex, float ey, float px, float py, const int* map, int tileSize, int mapW, int mapH)
    {
        float dx = px - ex;
        float dy = py - ey;

        float steps = std::max(std::abs(dx), std::abs(dy)) / (tileSize * 0.5f);

        for (int i = 0; i < (int)steps; i++)
        {
            float t = (float)i / steps;
            float x = ex + dx * t;
            float y = ey + dy * t;

            int tileX = (int)(x / tileSize);
            int tileY = (int)(y / tileSize);

            if (tileX < 0 || tileX >= mapW || tileY < 0 || tileY >= mapH)
                return false;

            if (map[tileY * mapW + tileX] == 1)
                return false; // wall blocks vision
        }

        return true;
    }

    Enemy(int startX, int startY, int tSize, int mWidth, int mLength, resources* r)
        : x(startX), y(startY), health(100.f), tileSize(tSize), mapWidth(mWidth), mapLength(mLength), speed(30.f), angle(0.f), alive(true), res(r)
    {
        // bind each enemy's local sf::Sound to the shared buffer
        meleeSound.setBuffer(res->attackBuffer[0]);
        hitSound.setBuffer(res->enemyBuffer[1]);
        deathSound.setBuffer(res->enemyBuffer[0]);
        walkSound.setBuffer(res->playerBuffer[2]);

    }

    bool canMoveTo(float nx, float ny, const int* map) {
        int tileX = int(nx / tileSize);
        int tileY = int(ny / tileSize);
        if (tileX < 0 || tileX >= mapWidth || tileY < 0 || tileY >= mapLength)
            return false;
        return map[tileY * mapWidth + tileX] == 0;
    }

    void moveWithCollision(float dx, float dy, const int* map)
    {
        float newX = x + dx;
        if (canMoveTo(newX, y, map))
            x = newX;

        float newY = y + dy;
        if (canMoveTo(x, newY, map))
            y = newY;
    }


    virtual ~Enemy() {}

    // Must be overridden by all enemies
    virtual void update(float dt, Player& player, const int* map, std::vector<Arrow>& arrows) = 0;

    static void processPlayerMelee(
        Player& player,
        std::vector<std::unique_ptr<Enemy>>& enemies);


    // Must be overridden by all enemies (depth-buffer aware)
    virtual void render(
        sf::RenderTarget& target,
        float playerX, float playerY,
        float playerAngle, float FOV,
        int screenWidth, int screenHeight,
        const std::vector<float>& depthBuffer
    ) = 0;

    // ---- NEW helper to play the enemy’s own melee sound ----
    void playMeleeSound() {
        meleeSound.play();   // plays independently for each enemy
    }

    // ---- Basics ----
    bool isAlive() const { return alive; }
    float getX() const { return x; }
    float getY() const { return y; }
    float getRadius() const { return radius; }
    float getHealth() const { return health; }

    // Position Setters
    void setX(float nx) { x = nx; }
    void setY(float ny) { y = ny; }
    void setHealth(float newH) { health = newH; }
    void setIsAlive(bool newA) { alive = newA; }
    void setState(EnemyState newState) { state = newState; }


    void takeDamage(float dmg) {
        health -= dmg;

        if (health <= 0) {
            alive = false;
            deathSound.play();
            hitTimer = 0.0f;     // make sure hit animation stops
            return;
        }

        // trigger hit animation
        hitTimer = 0.4f;

        hitSound.play();
    }

    void checkArrowHits(std::vector<Arrow>& arrows)
    {
        if (!alive) return;

        for (auto& a : arrows)
        {
            if (!a.alive) continue;

            float dx = a.x - x;
            float dy = a.y - y;
            float dist = std::sqrt(dx * dx + dy * dy);



            if (dist <= 15.0f)   // hit radius
            {
                takeDamage(30.0f);
                a.alive = false;
                state = ENEMY_HIT;
            }
        }
    }

    void updateAnimation(float dt) {
        animTimer += dt;

        if (animTimer >= animSpeed) {
            animTimer = 0.0f;

            switch (state) {
            case ENEMY_IDLE:
                animFrame = 0;
                break;
            case ENEMY_ATTACK:
                animFrame = 1;
                break;
            case ENEMY_HIT:
                animFrame = 2;
                break;
            case ENEMY_DEAD:
                animFrame = 3;
                break;
            case ENEMY_WALKING:
                animFrame = (animFrame == 4 ? 5 : 4);
                break;
            case ENEMY_RELOAD:
                animFrame = 6; // ranged only
                break;
            }
        }
    }

    

};
