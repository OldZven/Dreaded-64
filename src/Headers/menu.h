#pragma once
#include <SFML/Graphics.hpp>
#include "resources.h"
#include "levels.h"
#include <vector>
#include <string>

class menu{
private:
    sf::Font font;
    sf::Sprite backgroundSprite;
    sf::Text text;
    bool visible;
    resources* res;

public:
    menu(resources* resourceManager);
    void toggle();               // Show/hide menu
    void draw(sf::RenderWindow& window);  // Draw overlay
    bool isVisible() const;      // Check visibility
    void setVisible(bool v);
};
