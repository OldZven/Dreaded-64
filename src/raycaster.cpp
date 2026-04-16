#include "Headers/raycaster.h"
#include "Headers/resources.h"
#include "Headers/player.h"
#include <cmath>
#include <algorithm>

raycaster::raycaster(sf::RenderTarget* tgt, resources* tex, int width, int height, float fov)
    : target(tgt), textures(tex), screenWidth(width), screenHeight(height), FOV(fov)
{
    floorPixels.resize(screenWidth * screenHeight * 4, 0);
    ceilingPixels.resize(screenWidth * screenHeight * 4, 0);
    floorTextureForFrame.create(screenWidth, screenHeight);
    ceilingTextureForFrame.create(screenWidth, screenHeight);
    floorSprite.setTexture(floorTextureForFrame);
    ceilingSprite.setTexture(ceilingTextureForFrame);

    // --- Initialize HUD text objects once ---
    healthText.setFont(textures->arialFont);
    healthText.setScale(0.60f, 0.60f);
    healthText.setPosition(37.5f, 155.0f);
    healthText.setFillColor(sf::Color::Red);

    arrowText.setFont(textures->arialFont);
    arrowText.setScale(0.60f, 0.60f);
    arrowText.setPosition(37.5f, 177.0f);
    arrowText.setFillColor(sf::Color::Blue);

    controlsText.setFont(textures->arialFont);
    controlsText.setScale(0.25f, 0.3f);
    controlsText.setPosition(216.0f, 155.0f);
    controlsText.setFillColor(sf::Color::White);
    controlsText.setString("Up,Down,A,D - Move\nLeft,Right - Rotate\nU,H,Q - Attacks\nSpacebar - interact");

    depthBuffer.resize(screenWidth);

}

void raycaster::renderEnvironment(float playerX, float playerY, float playerAngle, int tileSize, int* map, int mapWidth, int mapLength, int levelIndex) {
    levelIndex--;
    renderFloor(playerX, playerY, playerAngle,tileSize,levelIndex);
    renderCeiling(playerX, playerY, playerAngle, tileSize, levelIndex);
    renderWalls(playerX, playerY, playerAngle, tileSize, map, mapWidth, mapLength,levelIndex);
    renderDoors(playerX, playerY, playerAngle, tileSize, map, mapWidth, mapLength, levelIndex);
    
}
void raycaster::renderHUD(float health, int arrowCount, int playerFrameIndex, int rightArmFrameIndex, int leftArmFrameIndex) {
    renderRightArm(rightArmFrameIndex);
    renderLeftArm(leftArmFrameIndex);
    renderBackgroundHUD();
    renderHealth(health);
    renderArrowCount(arrowCount);
    renderPlayer(playerFrameIndex);
    renderControls();
}
void raycaster::renderDeathScreen() {
    sf::Sprite DeathSprite;
    DeathSprite.setTexture(textures->deathScreenTexture);
    DeathSprite.setScale(1.0f, 1.0f);
    DeathSprite.setPosition(0.0f, 0.0f);

    target->draw(DeathSprite);
}

//Render Environment - floors, ceilings, walls, extras
void raycaster::renderFloor(float playerX, float playerY, float playerAngle, int tileSize, int levelIndex) {
    const sf::Image& floorImage = textures->cachedFloorImages[levelIndex];

    for (int y = screenHeight / 2; y < screenHeight; y++) {
        float rayDistance = float(tileSize) * screenHeight / (2.0f * y - screenHeight);
        for (int x = 0; x < screenWidth; x++) {
            float rayAngle = (playerAngle - FOV / 2.0f) + ((float)x / screenWidth) * FOV;
            float floorX = playerX + rayDistance * std::cos(rayAngle);
            float floorY = playerY + rayDistance * std::sin(rayAngle);
            int tx = ((int)floorX % tileSize + tileSize) % tileSize;
            int ty = ((int)floorY % tileSize + tileSize) % tileSize;
            sf::Color pixel = floorImage.getPixel(tx, ty);
            int idx = (y * screenWidth + x) * 4;
            floorPixels[idx + 0] = pixel.r;
            floorPixels[idx + 1] = pixel.g;
            floorPixels[idx + 2] = pixel.b;
            floorPixels[idx + 3] = 255;
        }
    }
    floorTextureForFrame.update(floorPixels.data());
    target->draw(floorSprite);
}
void raycaster::renderCeiling(float playerX, float playerY, float playerAngle, int tileSize, int levelIndex) {
    const sf::Image& ceilingImage = textures->cachedCeilingImages[levelIndex];

    for (int y = 0; y < screenHeight / 2; y++) {
        float rayDistance = float(tileSize) * screenHeight / (screenHeight - 2.0f * y);
        for (int x = 0; x < screenWidth; x++) {
            float rayAngle = (playerAngle - FOV / 2.0f) + ((float)x / screenWidth) * FOV;
            float ceilX = playerX + rayDistance * std::cos(rayAngle);
            float ceilY = playerY + rayDistance * std::sin(rayAngle);
            int tx = ((int)ceilX % tileSize + tileSize) % tileSize;
            int ty = ((int)ceilY % tileSize + tileSize) % tileSize;
            sf::Color pixel = ceilingImage.getPixel(tx, ty);
            int idx = (y * screenWidth + x) * 4;
            ceilingPixels[idx + 0] = pixel.r;
            ceilingPixels[idx + 1] = pixel.g;
            ceilingPixels[idx + 2] = pixel.b;
            ceilingPixels[idx + 3] = 255;
        }
    }
    ceilingTextureForFrame.update(ceilingPixels.data());
    target->draw(ceilingSprite);
}
void raycaster::renderWalls(float playerX, float playerY, float playerAngle, int tileSize, int* map, int mapWidth, int mapLength, int levelIndex)
{
    sf::VertexArray wallVertices(sf::Quads, screenWidth * 4);
    sf::Texture& wallTexture = textures->wallTexture[levelIndex];
    int textureWidth = wallTexture.getSize().x;
    int textureHeight = wallTexture.getSize().y;

    for (int x = 0; x < screenWidth; x++) {
        // --- Calculate ray direction for this screen column ---
        float cameraX = 2.0f * x / float(screenWidth) - 1.0f;
        float rayAngle = playerAngle + std::atan(cameraX * std::tan(FOV / 2.0f));
        float rayDirX = std::cos(rayAngle);
        float rayDirY = std::sin(rayAngle);


        // --- Player position in tile units ---
        float playerTileX = playerX / tileSize;
        float playerTileY = playerY / tileSize;
        int mapX = int(playerTileX);
        int mapY = int(playerTileY);

        // --- Calculate ray step and initial distances ---
        float deltaDistX = (rayDirX == 0) ? 1e30f : std::abs(1.0f / rayDirX);
        float deltaDistY = (rayDirY == 0) ? 1e30f : std::abs(1.0f / rayDirY);

        int stepX, stepY;
        float sideDistX, sideDistY;

        if (rayDirX < 0) {
            stepX = -1;
            sideDistX = (playerTileX - mapX) * deltaDistX;
        }
        else {
            stepX = 1;
            sideDistX = (mapX + 1.0f - playerTileX) * deltaDistX;
        }

        if (rayDirY < 0) {
            stepY = -1;
            sideDistY = (playerTileY - mapY) * deltaDistY;
        }
        else {
            stepY = 1;
            sideDistY = (mapY + 1.0f - playerTileY) * deltaDistY;
        }

        // --- Perform DDA to find wall hit ---
        bool hit = false;
        int side = 0;
        while (!hit) {
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            }
            else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }

            if (mapX < 0 || mapX >= mapWidth || mapY < 0 || mapY >= mapLength) {
                hit = true;
                break;
            }
            if (map[mapY * mapWidth + mapX] == 1) hit = true;
        }

        // --- Perpendicular distance to wall (removes fisheye) ---
        float perpWallDist = (side == 0 ? sideDistX - deltaDistX : sideDistY - deltaDistY);
        perpWallDist *= tileSize;
        perpWallDist = std::max(perpWallDist, 0.1f);

        // --- store into depth buffer for occlusion ---
        depthBuffer[x] = perpWallDist;

        // --- Projected wall height ---
        int lineHeight = int((tileSize * screenHeight) / perpWallDist);
        lineHeight = std::min(lineHeight, screenHeight);

        int drawStart = std::max(0, -lineHeight / 2 + screenHeight / 2);
        int drawEnd = std::min(screenHeight - 1, lineHeight / 2 + screenHeight / 2);

        // --- Texture coordinate (horizontal offset, stable version) ---
        float wallX;
        if (side == 0)
            wallX = playerTileY + (perpWallDist / tileSize) * rayDirY;
        else
            wallX = playerTileX + (perpWallDist / tileSize) * rayDirX;
        wallX -= std::floor(wallX);

        int texX = int(wallX * textureWidth);
        if ((side == 0 && rayDirX > 0) || (side == 1 && rayDirY < 0))
            texX = textureWidth - texX - 1;
        texX = std::clamp(texX, 0, textureWidth - 1);

        // --- Build wall quad for this screen column ---
        int baseIdx = x * 4;
        wallVertices[baseIdx + 0].position = sf::Vector2f(x, drawStart);
        wallVertices[baseIdx + 1].position = sf::Vector2f(x + 1, drawStart);
        wallVertices[baseIdx + 2].position = sf::Vector2f(x + 1, drawEnd);
        wallVertices[baseIdx + 3].position = sf::Vector2f(x, drawEnd);

        wallVertices[baseIdx + 0].texCoords = sf::Vector2f(texX, 0);
        wallVertices[baseIdx + 1].texCoords = sf::Vector2f(texX + 1, 0);
        wallVertices[baseIdx + 2].texCoords = sf::Vector2f(texX + 1, textureHeight);
        wallVertices[baseIdx + 3].texCoords = sf::Vector2f(texX, textureHeight);

        // --- Light shading for Y-sides (adds depth) ---
        sf::Color shade = (side == 1) ? sf::Color(180, 180, 180) : sf::Color::White;
        for (int i = 0; i < 4; i++)
            wallVertices[baseIdx + i].color = shade;
    }

    // --- Draw all wall slices at once ---
    sf::RenderStates wallState;
    wallState.texture = &wallTexture;
    target->draw(wallVertices, wallState);
}

void raycaster::renderDoors(
    float playerX, float playerY, float playerAngle,
    int tileSize, int* map,
    int mapWidth, int mapLength,
    int levelIndex)
{
    sf::VertexArray doorVertices(sf::Quads);

    sf::Texture& doorTexture = textures->doorTexture[levelIndex];
    int textureWidth = doorTexture.getSize().x;
    int textureHeight = doorTexture.getSize().y;

    for (int x = 0; x < screenWidth; x++)
    {
        // Ray angle for this screen column
        float cameraX = 2.0f * x / float(screenWidth) - 1.0f;
        float rayAngle = playerAngle + std::atan(cameraX * std::tan(FOV * 0.5f));

        float rayDirX = std::cos(rayAngle);
        float rayDirY = std::sin(rayAngle);

        // Player in tile units
        float posX = playerX / tileSize;
        float posY = playerY / tileSize;

        int mapX = int(posX);
        int mapY = int(posY);

        float deltaDistX = (rayDirX == 0) ? 1e30f : std::abs(1.f / rayDirX);
        float deltaDistY = (rayDirY == 0) ? 1e30f : std::abs(1.f / rayDirY);

        int stepX, stepY;
        float sideDistX, sideDistY;

        if (rayDirX < 0)
        {
            stepX = -1;
            sideDistX = (posX - mapX) * deltaDistX;
        }
        else
        {
            stepX = 1;
            sideDistX = (mapX + 1.f - posX) * deltaDistX;
        }

        if (rayDirY < 0)
        {
            stepY = -1;
            sideDistY = (posY - mapY) * deltaDistY;
        }
        else
        {
            stepY = 1;
            sideDistY = (mapY + 1.f - posY) * deltaDistY;
        }

        bool hit = false;
        bool foundDoor = false;
        int side = 0;

        while (!hit)
        {
            if (sideDistX < sideDistY)
            {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            }
            else
            {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }

            if (mapX < 0 || mapX >= mapWidth ||
                mapY < 0 || mapY >= mapLength)
            {
                hit = true;
                break;
            }

            int tile = map[mapY * mapWidth + mapX];

            if (tile == 1) // wall blocks view
            {
                hit = true;
            }
            else if (tile == 2) // door found
            {
                hit = true;
                foundDoor = true;
            }
        }

        if (!foundDoor)
            continue;

        float perpDist =
            (side == 0)
            ? (sideDistX - deltaDistX)
            : (sideDistY - deltaDistY);

        perpDist *= tileSize;
        perpDist = std::max(perpDist, 0.1f);

        // If wall already closer, skip door
        if (perpDist > depthBuffer[x])
            continue;

        int lineHeight = int((tileSize * screenHeight) / perpDist);
        lineHeight = std::min(lineHeight, screenHeight);

        int drawStart = std::max(0, -lineHeight / 2 + screenHeight / 2);
        int drawEnd = std::min(screenHeight - 1, lineHeight / 2 + screenHeight / 2);

        float wallX;
        if (side == 0)
            wallX = posY + (perpDist / tileSize) * rayDirY;
        else
            wallX = posX + (perpDist / tileSize) * rayDirX;

        wallX -= std::floor(wallX);

        int texX = int(wallX * textureWidth);

        if ((side == 0 && rayDirX > 0) ||
            (side == 1 && rayDirY < 0))
        {
            texX = textureWidth - texX - 1;
        }

        texX = std::clamp(texX, 0, textureWidth - 1);

        sf::Color shade =
            (side == 1)
            ? sf::Color(180, 180, 180)
            : sf::Color::White;

        doorVertices.append(sf::Vertex(sf::Vector2f(x, drawStart), shade, sf::Vector2f(texX, 0)));
        doorVertices.append(sf::Vertex(sf::Vector2f(x + 1, drawStart), shade, sf::Vector2f(texX + 1, 0)));
        doorVertices.append(sf::Vertex(sf::Vector2f(x + 1, drawEnd), shade, sf::Vector2f(texX + 1, textureHeight)));
        doorVertices.append(sf::Vertex(sf::Vector2f(x, drawEnd), shade, sf::Vector2f(texX, textureHeight)));
    }

    sf::RenderStates states;
    states.texture = &doorTexture;
    target->draw(doorVertices, states);
}


// --- Arms Rendering ---
void raycaster::renderLeftArm(int frameIndex) {
    if (frameIndex < 0 || frameIndex >= 5) frameIndex = 0;
    sf::Sprite leftArmSprite;
    leftArmSprite.setTexture(textures->leftArmAnim[frameIndex]);
    leftArmSprite.setScale(1.5f, 1.5f);
    leftArmSprite.setPosition(45.0f, 55.0f);

    target->draw(leftArmSprite);
}
void raycaster::renderRightArm(int frameIndex) {
    if (frameIndex < 0 || frameIndex >= 10) frameIndex = 0;
    sf::Sprite rightArmSprite;
    rightArmSprite.setTexture(textures->rightArmAnim[frameIndex]);
    rightArmSprite.setScale(1.5f, 1.5f);
    rightArmSprite.setPosition(175.0f, 55.0f);

    target->draw(rightArmSprite);
}

// --- HUD rendering ---

void raycaster::renderBackgroundHUD() {
    sf::Sprite hudSprite;
    hudSprite.setTexture(textures->hudTexture);
    hudSprite.setScale(5.0f, 0.8f);
    hudSprite.setPosition(0.0f, 150.0f);

    target->draw(hudSprite);
}
void raycaster::renderHealth(float health) {
    healthText.setString("Health: " + std::to_string(int(health)));
    target->draw(healthText);
}

void raycaster::renderArrowCount(int arrowCount) {
    arrowText.setString("Arrows: " + std::to_string(int(arrowCount)));
    target->draw(arrowText);
}

void raycaster::renderControls() {
    target->draw(controlsText); // static text, no per-frame rebuild
}
void raycaster::renderPlayer(int frameIndex) {
    if (frameIndex < 0 || frameIndex >= 10) frameIndex = 0;
    sf::Sprite playerSprite;
    playerSprite.setTexture(textures->playerTexture[frameIndex]);
    playerSprite.setScale(1.0f, 0.6f);
    playerSprite.setPosition(155.0f, 157.0f);

    target->draw(playerSprite);
}

// --- Arrows Rendering ---
void raycaster::renderArrows(const std::vector<Arrow>& arrows, float playerX, float playerY, float playerAngle) {
    for (const auto& arrow : arrows) {
        if (!arrow.alive) continue;

        // Relative position
        float dx = arrow.x - playerX;
        float dy = arrow.y - playerY;

        float distance = std::sqrt(dx * dx + dy * dy);
        if (distance < 1.0f) continue; // avoid divide-by-zero
        float angleToPlayer = std::atan2(dy, dx) - playerAngle;

        // Normalize angle to [-π, π]
        if (angleToPlayer > M_PI) angleToPlayer -= 2.0f * M_PI;
        if (angleToPlayer < -M_PI) angleToPlayer += 2.0f * M_PI;

        // Field of View check
        if (std::abs(angleToPlayer) < this->FOV / 2.0f) {
            int arrowScreenX = (int)((angleToPlayer + this->FOV / 2.0f) / this->FOV * this->screenWidth);
            int arrowHeight = (int)(this->screenHeight / distance);

            sf::Sprite arrowSpr(textures->arrowsTexture[0]);
            arrowSpr.setOrigin(
                textures->arrowsTexture[0].getSize().x / 2.0f,
                textures->arrowsTexture[0].getSize().y / 2.0f
            );
            arrowSpr.setPosition(arrowScreenX, this->screenHeight / 2);
            arrowSpr.setScale(
                arrowHeight / (float)textures->arrowsTexture[0].getSize().y,
                arrowHeight / (float)textures->arrowsTexture[0].getSize().y
            );
            arrowSpr.setRotation(arrow.angle * 180.0f / M_PI);
            target->draw(arrowSpr);
        }
    }
}