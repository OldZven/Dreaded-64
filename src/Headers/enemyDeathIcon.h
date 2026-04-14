#pragma once
#include "enemy.h"
#include <cmath>
#include <SFML/Graphics.hpp>
#include <vector>
#include "resources.h"
#include <algorithm>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


class EnemyDeathIcon {
protected:
	float x, y;
	bool active;
	
	resources* res;
public:
	EnemyDeathIcon(float xPosition, float yPosition, resources* r) :
		x(xPosition), y(yPosition), active(true), res(r)
	{
	}
    ~EnemyDeathIcon() {}

    void render(
        sf::RenderTarget& target,
        float playerX, float playerY,
        float playerAngle, float FOV,
        int screenWidth, int screenHeight,
        const std::vector<float>& depthBuffer
    )
    {
        if (active == false) return;

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
        // 200.f determines sprite size; tweak to your liking
        float scale = 10.0f / distance;
        scale = std::clamp(scale, 0.0f, 3.0f); // avoid tiny/huge sprites

        // ---- Build sprite ----
        sf::Sprite sprite;
        sprite.setTexture(res->enemyMeleeTexture[3]);  // your melee enemy texture

        // Center origin
        sprite.setOrigin(
            sprite.getTexture()->getSize().x * 0.3f,
            sprite.getTexture()->getSize().y * 0.30f
        );

        sprite.setScale(scale, scale);
        sprite.setPosition(screenX, screenHeight * 0.5f);

        target.draw(sprite);
    }
};
