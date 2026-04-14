#pragma once
#include "enemy.h"
#include <cmath>
#include <algorithm>
#include <vector>

struct EnemyProjectile {
    float x, y;
    float angle;
    float speed;
    float life;
    bool alive;
};

class RangedEnemy : public Enemy {
public:
    RangedEnemy(float startX, float startY, int tiSize, int maWidth, int maLength, resources* r)
        : Enemy(startX, startY, tiSize, maWidth, maLength, r)
    {
        shootCooldown = 0.0f;
        reloadTimer = 0.0f;
        reloadDuration = 2.0f;      // show reload animation for 2 seconds
        hitTimer = 0.0f;            // inherited from Enemy
        walkingFrame = 4;
        walkingFlipTimer = 0.0f;
        speed = 20.0f;
        attackRange = 250.0f;
    }

    void update(float dt, Player& player, const int* map,std::vector<Arrow>& arrows) override {
        if (!alive) return;

        checkArrowHits(arrows);

        float playerX = player.getX();
        float playerY = player.getY();

        // --- timers ---
        if (hitTimer > 0.0f) hitTimer -= dt;
        if (shootCooldown > 0.0f) shootCooldown -= dt;
        if (reloadTimer > 0.0f) reloadTimer -= dt;

        // --- movement ---
        float dx = player.getX() - x;
        float dy = player.getY() - y;
        float dist = std::sqrt(dx * dx + dy * dy);

        // If reloading or shooting, stay still
        bool busy = (reloadTimer > 0.0f);

        if (!busy && dist > 120.0f && dist > 0.001f) {
            float moveX = (dx / dist) * speed * dt;
            float moveY = (dy / dist) * speed * dt;
            moveWithCollision(moveX, moveY, map);
        }

        // --- attack logic ---
        if (!busy && dist < attackRange && shootCooldown <= 0.0f) {
            fireProjectile(player.getX(), player.getY());
            shootCooldown = 3.0f;
            reloadTimer = reloadDuration; // show reload animation
        }

        // --- update projectiles ---
        for (auto& p : projectiles) {
            if (!p.alive) continue;

            p.x += std::cos(p.angle) * p.speed * dt;
            p.y += std::sin(p.angle) * p.speed * dt;

            // --- PLAYER COLLISION ---
            float dx = p.x - player.getX();
            float dy = p.y - player.getY();
            float dist = std::sqrt(dx * dx + dy * dy);

            if (dist < 20.0f) {
                player.playerHit(15.0f);   // damage
                p.alive = false;
            }
        }

        // remove dead projectiles
        projectiles.erase(
            std::remove_if(projectiles.begin(), projectiles.end(),
                [](const EnemyProjectile& p) { return !p.alive; }),
            projectiles.end()
        );
    }

    // ---- RENDER ----
    void render(
        sf::RenderTarget& target,
        float playerX, float playerY,
        float playerAngle, float FOV,
        int screenWidth, int screenHeight,
        const std::vector<float>& depthBuffer
    ) override
    {
        if (!alive) return;

        // --- position relative to player ---
        float dx = x - playerX;
        float dy = y - playerY;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist < 0.1f) return;

        float angleToPlayer = std::atan2(dy, dx) - playerAngle;

        if (angleToPlayer > M_PI) angleToPlayer -= 2 * M_PI;
        if (angleToPlayer < -M_PI) angleToPlayer += 2 * M_PI;

        if (std::abs(angleToPlayer) > FOV * 0.5f)
            return;

        int screenX = (angleToPlayer + FOV * 0.5f) / FOV * screenWidth;
        if (screenX < 0 || screenX >= screenWidth)
            return;

        // wall occlusion
        if (dist > depthBuffer[screenX])
            return;

        // scale
        float scale = 10.0f / dist;
        scale = std::clamp(scale, 0.0f, 3.0f);

        // ---- choose animation frame ----
        int texIndex = 0;

        // 1. death
        if (!alive) {
            texIndex = 3;
        }
        // 2. hit
        else if (hitTimer > 0.0f) {
            texIndex = 2;
        }
        // 3. reload animation
        else if (reloadTimer > 0.0f) {
            texIndex = 6;
        }
        // 4. shooting uses the attack frame for 0.2 seconds
        else if (shootCooldown > 1.0f) {  // first 0.2s of cooldown
            texIndex = 1;
        }
        // 5. walking
        else if (std::sqrt(dx * dx + dy * dy) > 120.5f) {
            walkingFlipTimer += 1.0f / 60.0f;
            if (walkingFlipTimer > 0.6f) {
                walkingFlipTimer = 0.0f;
                walkingFrame = (walkingFrame == 4 ? 5 : 4);
            }
            texIndex = walkingFrame;
        }
        // 6. idle
        else {
            texIndex = 1;
        }

        // ---- draw enemy ----
        sf::Sprite sprite;
        sprite.setTexture(res->enemyRangedTexture[texIndex]);

        auto ts = sprite.getTexture()->getSize();
        sprite.setOrigin(ts.x * 0.3f, ts.y*0.8f);  // feet on ground

        sprite.setScale(scale, scale);
        sprite.setPosition(screenX, screenHeight * 0.5f + ts.y * 0.5f * scale);

        target.draw(sprite);

        // -----------------------------------------------
        // RENDER PROJECTILES
        // -----------------------------------------------
        for (auto& p : projectiles) {
            if (!p.alive) continue;

            float pdx = p.x - playerX;
            float pdy = p.y - playerY;
            float pdist = std::sqrt(pdx * pdx + pdy * pdy);
            if (pdist < 0.01f) continue;

            float pang = std::atan2(pdy, pdx) - playerAngle;

            if (pang > M_PI) pang -= 2 * M_PI;
            if (pang < -M_PI) pang += 2 * M_PI;

            if (std::abs(pang) > FOV * 0.5f) continue;

            int projX = (pang + FOV * 0.5f) / FOV * screenWidth;
            if (projX < 0 || projX >= screenWidth) continue;

            if (pdist > depthBuffer[projX]) continue;

            float pscale = 150.0f / pdist;
            pscale = std::clamp(pscale, 0.05f, 2.0f);

            sf::Sprite proj;
            proj.setTexture(res->arrowsTexture[1]);
            auto ps = proj.getTexture()->getSize();
            proj.setOrigin(ps.x * 0.5f, ps.y * 0.5f);

            proj.setScale(pscale, pscale);
            proj.setPosition(projX, screenHeight * 0.5f);
            proj.setRotation(p.angle * 180.f / M_PI);

            target.draw(proj);
        }
    }

private:
    float shootCooldown;
    float reloadTimer;
    float reloadDuration;

    float walkingFlipTimer;
    int walkingFrame;

    float attackRange;

    std::vector<EnemyProjectile> projectiles;

    void fireProjectile(float playerX, float playerY) {
        float dx = playerX - x;
        float dy = playerY - y;
        float angle = std::atan2(dy, dx);

        EnemyProjectile p;
        p.x = x;
        p.y = y;
        p.angle = angle;
        p.speed = 160.0f;
        p.life = 3.0f;
        p.alive = true;

        projectiles.push_back(p);
    }
};
