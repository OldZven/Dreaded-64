#pragma once
#include <SFML/Graphics.hpp>
#include "resources.h"
#include "player.h"
#include <vector>

class raycaster {
public:
    raycaster(sf::RenderTarget* tgt, resources* tex, int width, int height, float fov);

    // --- Render functions ---
    void renderEnvironment(float playerX, float playerY, float playerAngle, int tileSize, int* map, int mapWidth, int mapLength, int levelIndex);
    void renderHUD(float health, int arrowCount, int playerIndex, int rightFrame, int leftFrame);
    void renderArrows(const std::vector<Arrow>& arrows, float playerX, float playerY, float playerAngle);
    void renderDeathScreen();

    std::vector<float> depthBuffer;

private:
    sf::RenderTarget* target;  
    resources* textures;
    int screenWidth;
    int screenHeight;
    float FOV;

    //- Environment Renders
    void renderFloor(float playerX, float playerY, float playerAngle, int tileSize, int levelIndex);
    void renderCeiling(float playerX, float playerY, float playerAngle, int tileSize, int levelIndex);
    void renderWalls(float playerX, float playerY, float playerAngle, int tileSize, int* map, int mapWidth, int mapLength, int levelIndex);

    //- Hud renders
    void renderRightArm(int frame);
    void renderLeftArm(int frame);
    void renderHealth(float health);
    void renderArrowCount(int arrowCount);
    void renderPlayer(int frameIndex);
    void renderBackgroundHUD();
    void renderControls();

    // Floor & ceiling pixel buffers
    std::vector<sf::Uint8> floorPixels;
    std::vector<sf::Uint8> ceilingPixels;

    sf::Texture floorTextureForFrame;
    sf::Texture ceilingTextureForFrame;
    sf::Sprite floorSprite;
    sf::Sprite ceilingSprite;

    sf::Text healthText;
    sf::Text arrowText;
    sf::Text controlsText;
};
