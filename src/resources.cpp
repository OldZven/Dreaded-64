#include "Headers/resources.h"
#include <iostream>

//---Loading of Individual Resources---
bool resources::loadFont(sf::Font& font, const std::string& path) {
    if (!font.loadFromFile(texturePath+path)) {
        std::cerr << "Failed to load font: " << path << "\n";
        return false;
    }
    return true;
}
bool resources::loadTexture(sf::Texture& texture, const std::string& path) {
    if (!texture.loadFromFile(texturePath+path)) {
        std::cerr << "Failed to load texture: " << path << "\n";
        return false;
    }
    texture.setSmooth(false);
    return true;
}
void resources::loadEnemyTexturesHelper(sf::Texture* texture[6], const std::string& type) {

    loadTexture(*texture[0], "Enemies" + type + "Idle.png");
    loadTexture(*texture[1], "Enemies" + type + "Attack.png");
    loadTexture(*texture[2], "Enemies" + type + "Hit.png");
    loadTexture(*texture[3], "Enemies" + type + "Death.png");
    loadTexture(*texture[4], "Enemies" + type + "Walking1.png");
    loadTexture(*texture[5], "Enemies" + type + "Walking2.png");
}

bool resources::loadAudio(sf::SoundBuffer& buffer, const std::string& path, sf::Sound& audio) {
    if (!buffer.loadFromFile(audioPath+path)) {
        std::cerr << "Failed to load sound: " << path << "\n";
        return false;
    }
    audio.setBuffer(buffer);
    return true;
}
void resources::loopBackground() {
    backgroundSound.setLoop(true);
}
void resources::loopWalking() {
    playerWalkSound.setLoop(true);
}
void resources::setVolumes() {
    backgroundSound.setVolume(10.0f);
    playerWalkSound.setVolume(20.0f);
}

void resources::loadAllResources() {
    loopBackground();
    loadAllAudio();
    setVolumes();
    loadAllTextures();
}

void resources::loadAllAudio() {
    //---Attack Audio---
    loadAudio(attackBuffer[0], "Attacks/Standard.wav", meleeStandardSound);
    loadAudio(attackBuffer[1], "Attacks/Heavy.wav", meleeHeavySound);
    loadAudio(attackBuffer[2], "Attacks/Ranged.wav", rangedAttackSound);
    loadAudio(attackBuffer[3], "Attacks/Reload.wav", reloadSound);

    //---Entity Audio---
    loadAudio(playerBuffer[0], "Entity/PlayerDeath.wav", playerDeathSound);
    loadAudio(enemyBuffer[0], "Entity/EnemyDeath.wav", enemyDeathSound);
    loadAudio(playerBuffer[1], "Entity/PlayerHit.wav", hitPlayerSound);
    loadAudio(enemyBuffer[1], "Entity/EnemyHit.wav", hitEnemySound);
    loadAudio(attackBuffer[4], "Entity/Interact.wav", interactSound);
    loadAudio(playerBuffer[2], "Entity/PlayerWalk.wav", playerWalkSound);

    //---Background Audio---
    loadAudio(backgroundBuffer, "Background/Background1.wav", backgroundSound);
}
void resources::loadAllTextures() {

    //---Left Arm Textures---
    loadTexture(leftArmAnim[0], "LeftArm/Loaded.png");
    loadTexture(leftArmAnim[1], "LeftArm/Unloaded.png");
    loadTexture(leftArmAnim[2], "LeftArm/Block.png");
    loadTexture(leftArmAnim[3], "LeftArm/IdleInteract.png");
    loadTexture(leftArmAnim[4], "LeftArm/Interact.png");

    //---Right Arm Textures---
    loadTexture(rightArmAnim[0], "RightArm/Idle.png");
    loadTexture(rightArmAnim[1], "RightArm/Attack1Start.png");
    loadTexture(rightArmAnim[2], "RightArm/Attack1Mid.png");
    loadTexture(rightArmAnim[3], "RightArm/Attack1End.png");
    loadTexture(rightArmAnim[4], "RightArm/Attack2Start.png");
    loadTexture(rightArmAnim[5], "RightArm/Attack2Mid.png");
    loadTexture(rightArmAnim[6], "RightArm/Attack2End.png");
    loadTexture(rightArmAnim[7], "RightArm/AttackPowerStart.png");
    loadTexture(rightArmAnim[8], "RightArm/AttackPowerEnd.png");
    loadTexture(rightArmAnim[9], "RightArm/Reload.png");

    //Building Textures
    for (int i = 0; i < 5; i++) {
        loadTexture(wallTexture[i], "Building/Walls/Lvl"+std::to_string(i+1)+"Wall.png");
        loadTexture(floorTexture[i], "Building/Floors/Lvl"+std::to_string(i + 1)+"Floor.png");
        loadTexture(ceilingTexture[i], "Building/Ceilings/Lvl" + std::to_string(i + 1) + "Ceiling.png");
        loadTexture(doorTexture[i], "Building/Doors/Lvl" + std::to_string(i + 1) + "Door.png");
    }

    loadTexture(doorTexture[5], "Building/Doors/ExitDoor.png");

   // Enemy Textures
        for (int i = 0; i < 6;i++) {
            enemyTempTexture[i] = &enemyRangedTexture[i];
        }
        loadEnemyTexturesHelper(enemyTempTexture, "/Ranged/");
        loadTexture(enemyRangedTexture[6], "Enemies/Ranged/Reloading.png");//Extra texture

        for (int i = 0; i < 6; i++) {
            enemyTempTexture[i] = &enemyMeleeTexture[i];
        }
        loadEnemyTexturesHelper(enemyTempTexture, "/Melee/");
    

    // Cache CPU copies once
    cachedFloorImages.resize(5);
    cachedCeilingImages.resize(5);
    for (int i = 0; i < 5; ++i) {
        cachedFloorImages[i] = floorTexture[i].copyToImage();
        cachedCeilingImages[i] = ceilingTexture[i].copyToImage();
    }

    //---Arrows Textures---
    loadTexture(arrowsTexture[0], "Projectiles/PlayerArrow.png");
    loadTexture(arrowsTexture[1], "Projectiles/EnemyArrow.png");

    //---Player Textures---
    loadTexture(playerTexture[0], "Player/Idle.png");
    loadTexture(playerTexture[1], "Player/Left.png");
    loadTexture(playerTexture[2], "Player/Right.png");
    loadTexture(playerTexture[3], "Player/Attack.png");
    loadTexture(playerTexture[4], "Player/HitEnemy.png");
    loadTexture(playerTexture[5], "Player/HitPlayer.png");
    loadTexture(playerTexture[6], "Player/Death.png");

    //---Extra Textures---
    loadTexture(hudTexture, "Hud/Background.png");
    loadTexture(deathScreenTexture, "Hud/Death.png");
    loadTexture(menuTexture, "Hud/Menu.png");
    loadFont(arialFont, "Fonts/ARIAL.ttf");
}