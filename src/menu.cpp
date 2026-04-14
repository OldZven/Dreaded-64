#include "Headers/menu.h"
#include "Headers/resources.h"
#include "Headers/menu.h"
#include <iostream>

menu::menu(resources* resourceManager)
    : res(resourceManager), visible(false)
{
    backgroundSprite.setTexture(res->menuTexture); // example reuse
    backgroundSprite.setScale(5.0f, 2.3f);
    backgroundSprite.setColor(sf::Color(255, 255, 255, 240));

    text.setFont(res->arialFont);
    text.setCharacterSize(16);
    text.setFillColor(sf::Color::Yellow);
    text.setString("Controls\nUp/Down Arrows = Move Up and Down\nA and D = Move Left and Right\nRight/Left Arrows = Turn right and left\nU,H,Q = Attacks\nSpacebar = interact\nEsc = Exit Game");
}

void menu::toggle() {
    visible = !visible;
}

bool menu::isVisible() const {
    return visible;
}

void menu::draw(sf::RenderWindow& window) {
    if (!visible) return;

    sf::Vector2f pos(window.getSize().x - backgroundSprite.getGlobalBounds().width - 10.f, 10.f);
    backgroundSprite.setPosition(pos);
    text.setPosition(pos.x + 5.f, pos.y + 5.f);

    window.draw(backgroundSprite);
    window.draw(text);
}
void menu::setVisible(bool visibility) {
    visible = visibility;
}
