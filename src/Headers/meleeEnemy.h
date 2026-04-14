#pragma once
#include "enemy.h"
#include <cmath>
#include <algorithm>

class MeleeEnemy : public Enemy {
public:
    MeleeEnemy(float startX, float startY, int tiSize, int maWidth, int maLength, resources* r)
        : Enemy(startX, startY, tiSize, maWidth, maLength, r)
    {
        // tuning values
        attackInterval = 3.0f;      // total time between attack starts (seconds)
        swingDisplayTime = 1.5f;    // how long the attack sprite is shown (seconds) — per your request
        attackTimer = 0.0f;
        swingTimer = 0.0f;
        swingPlayed = false;
        speed = 30.0f;
        hitRange = 32.0f;
    }

    void update(float dt, Player& player, const int* map, std::vector<Arrow>& arrows) override {
        if (!alive) return;

        checkArrowHits(arrows);

        // decrement hit timer
        if (hitTimer > 0.0f)
            hitTimer -= dt;


        // --- Movement when not currently in the "cooldown starting" phase ---
        // We'll still allow approach while attack timer counts down.
        float dx = player.getX() - x;
        float dy = player.getY() - y;
        float distance = std::sqrt(dx * dx + dy * dy);

        // decrement timers
        if (attackTimer > 0.0f) attackTimer -= dt;
        if (swingTimer > 0.0f) swingTimer -= dt;
        // when swingTimer runs out, we show idle for remainder of attackTimer

        // If player is out of hit range, move toward them
        if (distance > hitRange) {
            // move toward player
            float moveX = (dx / distance) * speed * dt;
            float moveY = (dy / distance) * speed * dt;
            moveWithCollision(moveX, moveY, map);

        }
        else {
            // Player is in hit range: possibly start an attack cycle
            if (attackTimer <= 0.0f) {
                startAttackCycle();
            }
        }

        // Play the swing sound exactly once at the moment the attack sprite is displayed
        if (swingTimer > 0.0f && !swingPlayed) {
            // Use the per-enemy sound helper from Enemy
            playMeleeSound();
            swingPlayed = true;

            player.playerHit(30.0f);

            // Optionally, you can also apply damage here (once per swing)
            // applyDamageToPlayer(); // <-- hook this up to your player-damage code if needed
        }

        // You can add additional logic here for hit reactions, death, etc.
    }

    // ---- NEW RENDER METHOD (correct signature + occlusion) ----
    void render(
        sf::RenderTarget& target,
        float playerX, float playerY,
        float playerAngle, float FOV,
        int screenWidth, int screenHeight,
        const std::vector<float>& depthBuffer
    ) override
    {
        if (!alive) return;

        // --- Relative vector to player ---
        float dx = x - playerX;
        float dy = y - playerY;
        float distance = std::sqrt(dx * dx + dy * dy);
        if (distance < 0.1f) return;

        // --- Angle enemy->player ---
        float angleToPlayer = std::atan2(dy, dx) - playerAngle;

        // Normalize angle to [-pi, pi]
        if (angleToPlayer > M_PI) angleToPlayer -= 2.f * M_PI;
        if (angleToPlayer < -M_PI) angleToPlayer += 2.f * M_PI;

        // Outside FOV?
        if (std::abs(angleToPlayer) > FOV * 0.5f)
            return;

        // Map angle to screen column
        int screenX = (angleToPlayer + FOV * 0.5f) / FOV * screenWidth;
        if (screenX < 0 || screenX >= screenWidth)
            return;

        // ---- WALL OCCLUSION ----
        if (distance > depthBuffer[screenX]) {
            // Wall is closer than the enemy
            return;
        }

        // ---- Perspective depth scaling ----
        float scale = 10.0f / distance;
        scale = std::clamp(scale, 0.0f, 3.0f);

        // ---- Build sprite ----
        sf::Sprite sprite;

        // Decide which texture index to use:
        // - If swingTimer > 0 -> show Attack frame (index 1)
        // - Else if attackTimer > 0 but swingTimer <= 0 -> show Idle frame (index 0) for the rest of the attack cycle
        // - Else if moving -> walking frames (4/5 alternating based on time)
        // - Else -> idle (0)
        int texIndex = 0;

        // --- 1. DEATH ---
        if (!alive) {
            texIndex = 3;                   // Death.png
        }
        // --- 2. HIT (pain reaction) ---
        else if (hitTimer > 0.0f) {
            texIndex = 2;                   // Hit.png overrides walking/idle/attack cooldown
        }
        // --- 3. SWING (attack animation) ---
        else if (swingTimer > 0.0f) {
            texIndex = 1;                   // Attack.png
        }
        // --- 4. ATTACK COOLDOWN (idle during cooldown) ---
        else if (attackTimer > 0.0f) {
            texIndex = 0;                   // Idle.png
        }
        // --- 5. WALKING ---
        else if (isMoving(distance)) {
            walkingFlipTimer += 1.0f / 60.0f;
            if (walkingFlipTimer > 0.6f) {
                walkingFlipTimer = 0.0f;
                walkingFrame = (walkingFrame == 4) ? 5 : 4;
            }
            texIndex = walkingFrame;
        }
        // --- 6. IDLE ---
        else {
            texIndex = 0;
        }



        sprite.setTexture(res->enemyMeleeTexture[texIndex]);

        // Set origin so feet anchor to bottom center
        auto texSize = sprite.getTexture()->getSize();
        sprite.setOrigin(texSize.x * 0.3f, texSize.y * 0.8f); // bottom-center origin

        sprite.setScale(scale, scale);

        // position: place the sprite vertically so feet rest at center of screen height (tweak as needed)
        sprite.setPosition(screenX, screenHeight * 0.5f + sprite.getTexture()->getSize().y * 0.5f * scale);

        target.draw(sprite);
    }

private:
    // Attack timing / control
    float attackInterval;     // time between attack starts
    float swingDisplayTime;   // how long to display the swing sprite (2.0s per your request)
    float attackTimer;        // counts down to next attack start
    float swingTimer;         // counts down the displayed swing duration
    bool swingPlayed;         // whether the swing sound has been played for current swing
    float hitRange;

    // walking visuals
    int walkingFrame = 4;     // start with walking1 index
    float walkingFlipTimer = 0.0f;
    float clockFloat = 0.0f;  // a tiny local clock used only if dt isn't passed to render (safe fallback)

    void startAttackCycle() {
        attackTimer = attackInterval;
        swingTimer = swingDisplayTime;
        swingPlayed = false;
        // optionally you can set other flags / states here
    }

    bool isMoving(float distance) {
        // consider the enemy moving if it's farther than a small epsilon from the target
        return distance > (hitRange + 0.5f);
    }
};
