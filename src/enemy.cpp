#include "Headers/enemy.h"
#include "Headers/player.h"
#include <cmath>

void Enemy::processPlayerMelee(
    Player& player,
    std::vector<std::unique_ptr<Enemy>>& enemies)
{
    if (!player.getIsMeleeAttacking())
        return;

    bool light = (player.getMeleeType() == 1 &&
        player.getAttackMeleeTimer() > 0.4f &&
        player.getAttackMeleeTimer() < 0.8f);

    bool heavy = (player.getMeleeType() == 2 &&
        player.getAttackMeleeTimer() > 0.8f &&
        player.getAttackMeleeTimer() < 1.6f);

    if ((light || heavy) && !player.getHasDealtMeleeDamage())
    {
        float damage = (player.getMeleeType() == 1) ? 40.0f : 80.0f;

        for (auto& e : enemies)
        {
            if (!e->isAlive()) continue;

            float dx = e->getX() - player.getX();
            float dy = e->getY() - player.getY();
            float dist = std::sqrt(dx * dx + dy * dy);

            if (dist < 45.0f)
            {
                e->takeDamage(damage);
                player.setHasDealtMeleeDamage(true);
            }
        }
    }    
}

void Enemy::processDeath(std::vector<std::unique_ptr<Enemy>>& enemies,
    std::vector<std::unique_ptr<EnemyDeathIcon>>& enemyDeaths,
    resources* resource) {
    for (auto& e : enemies) {
        if (e->getHealth() <= 0)
        {
            enemyDeaths.push_back(
                std::make_unique<EnemyDeathIcon>(
                    e->getX(), e->getY(), resource));
        }
    }
}