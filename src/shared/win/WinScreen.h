#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum class WinScreenResult {
    None,
    BackToMenu,
    Exit
};

class WinScreen {
public:
    WinScreen(float width, float height, const std::string& fontPath);

    WinScreenResult run(sf::RenderWindow& window);

private:
    float width, height;
    sf::Font font;
    sf::Text winTitle;
    sf::Text winSubtitle;
    sf::RectangleShape overlay;

    sf::RectangleShape winBanner;
    sf::ConvexShape trophyCup;
    sf::RectangleShape knightBody;
    sf::ConvexShape princessDress;
    sf::CircleShape princessHead;

    sf::RectangleShape btnBack;
    sf::Text btnBackText;
    sf::RectangleShape btnExit;
    sf::Text btnExitText;
};
