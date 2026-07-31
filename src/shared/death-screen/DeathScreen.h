#pragma once
#include <SFML/Graphics.hpp>

enum class DeathScreenResult {
    None,
    Restart,
    Exit
};

class DeathScreen{
    private :
    sf::RectangleShape overlay;
    sf::RectangleShape yesButton;
    sf::RectangleShape noButton;
    sf::Font font;
    sf::Text gameOverText;
    sf::Text yesText;
    sf::Text noText;
    sf::Text restartText;
    float m_designWidth;
    float m_designHeight;

    sf::View uiView() const;
    void layout();

    public:
    DeathScreen(float width,float height, const std::string& fontPath);
    void draw(sf::RenderWindow& window);
    DeathScreenResult getInput(sf::RenderWindow& window, sf::Vector2i pixelPos);

};
