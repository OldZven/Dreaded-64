#include <SFML/Graphics.hpp>
#include "Headers/raycaster.h"
#include "Headers/player.h"
#include "Headers/resources.h"
#include "Headers/levels.h"
#include "Headers/menu.h"
#include <iostream>
#include "Headers/enemy.h"
#include "Headers/meleeEnemy.h"
#include "Headers/rangedEnemy.h"
#include "Headers/enemyDeathIcon.h"
#include <memory>

int levelIndex = 0;
int map[16 * 16];
int enemiesSpawned = 20;
std::vector<float> enemySpawnX;
std::vector<float> enemySpawnY;
std::vector<bool> enemyMelee;


void levelDecision(int lvlIndex, bool startGame) {
    levels level;
    level.Levels();
    int choice = 0;
    int levelChoice = 0;
    //Choose level to play
    
    while (levelIndex == 0 || startGame == false) {
        if (startGame == false) {
            levelChoice = lvlIndex;
        }
        else {
            sf::RenderWindow windowMenu (sf::VideoMode(400,200), "Dreaded Menu");
            sf::Font font;
            font.loadFromFile("Data/Textures/Fonts/ARIAL.ttf"); // use your font
            sf::Text text;
            text.setFont(font);
            text.setCharacterSize(24);
            text.setFillColor(sf::Color::White);
            text.setPosition(50, 50);
            std::string input = "";
            bool enterPressed = false;
            while (windowMenu.isOpen())
            {
                sf::Event event;

                while (windowMenu.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                        windowMenu.close();

                    if (event.type == sf::Event::TextEntered)
                    {
                        char c = static_cast<char>(event.text.unicode);

                        if (c >= '1' && c <= '5')   // printable characters
                            input = c;

                        if (c == '\b' && !input.empty()) // backspace
                            input.pop_back();
                    }
                    if (event.type == sf::Event::KeyPressed &&
                        event.key.code == sf::Keyboard::Enter)
                    {
                        enterPressed = true; // or break out of your loop
                    }
                }

                text.setString("Choose level [1,2,3,4,5]: \n" + input);

                windowMenu.clear();
                windowMenu.draw(text);
                windowMenu.display();
                if (!input.empty() && enterPressed == true) {
                    levelChoice = std::stoi(input);
                    windowMenu.close();
                }
            }
            

        }
        if (levelChoice == 1) {
            levelIndex = 1;
            for (int i = 0; i < 256; i++) {
                map[i] = level.loadLevelOne(i);
                }
            enemySpawnX = { 300.0f, 600.0f ,100.0f ,100.0f ,200.0f ,200.0f ,800.0f ,200.0f, 600.0f, 600.0f, 1500.0f, 1300.0f, 1300.0f, 1400.0f, 1100.0f, 1100.0f, 1500.0f, 1100.0f, 500.0f, 700.0f };
            enemySpawnY = { 100.0f, 100.0f, 300.0f, 400.0f, 300.0f, 400.0f, 400.0f, 800.0f, 700.0f, 900.0f, 300.0f, 500.0f, 800.0f, 1000.0f, 100.0f, 1500.0f, 1500.0f, 1300.0f, 1500.0f, 1300.0f };
            enemyMelee = { true, false, false, false, true, true, true, false, true, true, false, true, false, true, true, true, true, false, false, true };

            break;
        }
        else if (levelChoice == 2) {
            levelIndex = 2;
            for (int i = 1; i < 256; i++) {
                map[i] = level.loadLevelTwo(i);
                }
            enemySpawnX = { 128.0f, 192.0f, 256.0f, 384.0f, 448.0f,512.0f, 640.0f, 768.0f, 832.0f, 896.0f,1024.0f, 1088.0f, 1152.0f, 1216.0f, 1280.0f,1344.0f, 768.0f, 512.0f, 320.0f, 960.0f };
            enemySpawnY = { 128.0f, 128.0f, 192.0f, 256.0f, 320.0f,    384.0f, 448.0f, 512.0f, 576.0f, 640.0f,    704.0f, 768.0f, 832.0f, 896.0f, 960.0f,   1024.0f, 384.0f, 640.0f, 768.0f, 320.0f };
            enemyMelee = { true, false, true, true, false,    true, true, false, true, false,    true, false, true, true, false,    true, false, true, true, false };

            break;
        }
        else if (levelChoice == 3) {
            levelIndex = 3;
            for (int i = 1; i < 256; i++) {
                map[i] = level.loadLevelThree(i);
                }
            enemySpawnX = { 128.0f, 192.0f, 256.0f, 320.0f, 448.0f,    512.0f, 576.0f, 640.0f, 768.0f, 832.0f,    896.0f, 960.0f, 1024.0f, 1088.0f, 1152.0f,   1216.0f, 384.0f, 704.0f, 800.0f, 576.0f };
            enemySpawnY = { 128.0f, 128.0f, 192.0f, 256.0f, 320.0f,    384.0f, 448.0f, 512.0f, 576.0f, 640.0f,    704.0f, 768.0f, 832.0f, 896.0f, 960.0f,    1024.0f, 320.0f, 640.0f, 768.0f, 896.0f };
            enemyMelee = { true, true, false, true, true,    true, false, true, true, false,    true, true, false, true, true,    true, false, true, true, false };

            break;
        }
        else if (levelChoice == 4) {
            levelIndex = 4;
            for (int i = 1; i < 256; i++) {
                map[i] = level.loadLevelFour(i);
                }
            enemySpawnX = { 128.0f, 192.0f, 256.0f, 384.0f, 448.0f,    512.0f, 640.0f, 704.0f, 768.0f, 832.0f,    896.0f, 960.0f, 1024.0f, 1152.0f, 1216.0f,    384.0f, 576.0f, 640.0f, 800.0f, 1088.0f };
            enemySpawnY = { 128.0f, 128.0f, 192.0f, 256.0f, 320.0f,    384.0f, 448.0f, 512.0f, 576.0f, 640.0f,    704.0f, 768.0f, 832.0f, 896.0f, 960.0f,    320.0f, 640.0f, 768.0f, 896.0f, 1024.0f };
            enemyMelee = { true, true, false, true, true,    true, false, true, true, false,    true, true, false, true, true,    false, true, false, true, true };

            break;
        }
        else if (levelChoice == 5) {
            levelIndex = 5;
            for (int i = 1; i < 256; i++) {
                map[i] = level.loadLevelFive(i);
                }
            enemySpawnX = { 128.0f, 192.0f, 256.0f, 320.0f, 448.0f,    512.0f, 640.0f, 704.0f, 768.0f, 832.0f,    896.0f, 960.0f, 1024.0f, 1152.0f, 1216.0f,    384.0f, 576.0f, 640.0f, 800.0f, 960.0f };
            enemySpawnY = { 128.0f, 128.0f, 192.0f, 256.0f, 320.0f,    384.0f, 448.0f, 512.0f, 576.0f, 640.0f,    704.0f, 768.0f, 832.0f, 896.0f, 960.0f,    320.0f, 640.0f, 768.0f, 896.0f, 1024.0f };
            enemyMelee = { true, true, true, false, true,    true, false, true, true, true,    false, true, true, false, true,    true, false, true, true, false };

            break;
        }
    }
}

int main() {

    bool isLoaded = false;
    
    levels level;
    level.Levels();
  
    //int map[32 * 32];
    
    levelDecision(0,true);

    // --- Internal render resolution ---
    const int internalWidth = 320;
    const int internalHeight = 200;

    // --- Fullscreen window setup ---
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "Dreaded 64", sf::Style::Fullscreen);
    window.setFramerateLimit(30);
    window.setVerticalSyncEnabled(true);

    // --- RenderTexture for low-res rendering ---
    sf::RenderTexture renderTexture;
    renderTexture.create(internalWidth, internalHeight);
    sf::Sprite screenSprite(renderTexture.getTexture());
    screenSprite.setScale(
        float(desktop.width) / internalWidth,
        float(desktop.height) / internalHeight
    );

    // --- Resources ---
    resources resource;
    resource.loadAllResources();
    resource.backgroundSound.play();
    int tileSize = 64;

    std::vector<std::unique_ptr<Enemy>> enemies;
    for (int i = 0; i < enemiesSpawned;i++) {
        if (enemyMelee[i] == true) {
           enemies.push_back(std::make_unique<MeleeEnemy>(enemySpawnX[i], enemySpawnY[i], tileSize, level.mapWidth, level.mapLength, &resource));
        }
        else {
           enemies.push_back(std::make_unique<RangedEnemy>(enemySpawnX[i], enemySpawnY[i], tileSize, level.mapWidth, level.mapLength, &resource));
        }
    }
    std::vector<std::unique_ptr<EnemyDeathIcon>> enemyDeaths;


    menu menu(&resource);
    bool menuOnce = false;

    bool changedLevel = false;
    bool gameEnded = false;
   

    // --- Raycaster and Player ---
    raycaster raycaster(&renderTexture, &resource, internalWidth, internalHeight, 60.0f * (3.14159265f / 180.0f));
    Player player(100.0f, 100.0f, 0.0f, tileSize, level.mapWidth, level.mapLength, internalWidth, internalHeight, &resource);

    sf::Clock clock;

    while (gameEnded == false) {

        

        while (window.isOpen()) {

            if (isLoaded == false) {
                levelDecision(0, true);
                isLoaded = true;
            }

            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            float dt = clock.restart().asSeconds();

            // --- Update ---
            player.handlerUpdater(dt, map, enemies);

            //Checks to open menu
            if (player.getOpenMenu()) {
                menu.setVisible(true);
            }
            else {
                menu.setVisible(false);
            }

            // --- Rendering --- 
            renderTexture.clear();


            raycaster.renderEnvironment(player.getX(), player.getY(), player.getAngle(), tileSize, map, level.mapWidth, level.mapLength, levelIndex);

            for (auto& e : enemies) e->update(dt, player, map, player.getArrows());
            // --- PLAYER MELEE DAMAGE ---
            Enemy::processPlayerMelee(player, enemies);
            Enemy::processDeath(enemies, enemyDeaths, &resource);


            std::sort(enemies.begin(), enemies.end(),
                [&](const std::unique_ptr<Enemy>& a, const std::unique_ptr<Enemy>& b)
                {
                    float dxA = a->getX() - player.getX();
                    float dyA = a->getY() - player.getY();
                    float distA = dxA * dxA + dyA * dyA;

                    float dxB = b->getX() - player.getX();
                    float dyB = b->getY() - player.getY();
                    float distB = dxB * dxB + dyB * dyB;

                    // sort far → near (farther enemies drawn first)
                    return distA > distB;
                }
            );
            for (auto& e : enemies) {
                e->render(
                    renderTexture,
                    player.getX(), player.getY(), player.getAngle(),
                    60.0f * (3.14159265f / 180.0f),
                    internalWidth, internalHeight,
                    raycaster.depthBuffer
                );
            }
            for (auto& i : enemyDeaths) {
                i->render(
                    renderTexture,
                    player.getX(), player.getY(), player.getAngle(),
                    60.0f * (3.14159265f / 180.0f),
                    internalWidth, internalHeight,
                    raycaster.depthBuffer
                );
            }
            raycaster.renderArrows(player.getArrows(), player.getX(), player.getY(), player.getAngle());
            if (player.getHealth() > 0) {
                raycaster.renderHUD(player.getHealth(), player.getArrowCount(), player.getPlayerFrame(), player.getRightArmFrame(), player.getLeftArmFrame());
            }
            else {
                raycaster.renderDeathScreen();
            }

            //Last thing to render
            
            renderTexture.display();

            window.clear();
            window.draw(screenSprite);
            menu.draw(window);
            window.display();

            // End game
            gameEnded = player.exitGame();
            if (gameEnded == true) {
                window.close();
                gameEnded = true;
            }
            //Change Level
            changedLevel = player.changeLevel();
            if (changedLevel)
            {
                levelIndex++;
                player.setChangeLevel(false);

                if (levelIndex > 5)
                    levelIndex = 1; // loop back

                enemies.clear();
                enemyDeaths.clear();
                levelDecision(levelIndex, false);
                player.setX(100.0f);
                player.setY(100.0f);
                player.setHealth(100.0f);
                player.setArrowCount(player.getArrowCount() + 20);
                for (int i = 0; i < enemiesSpawned;i++) {
                    if (enemyMelee[i] == true) {
                        enemies.push_back(std::make_unique<MeleeEnemy>(enemySpawnX[i], enemySpawnY[i], tileSize, level.mapWidth, level.mapLength, &resource));
                    }
                    else {
                        enemies.push_back(std::make_unique<RangedEnemy>(enemySpawnX[i], enemySpawnY[i], tileSize, level.mapWidth, level.mapLength, &resource));
                    }
                }
                
            }
        }
    }
    
    return 0;
}
