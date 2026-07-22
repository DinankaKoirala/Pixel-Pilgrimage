#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Knight.hpp"
#include "Villain.hpp"
#include "Princess.hpp"
#include "Projectile.hpp"

enum class GameState {
    Intro,     // villain carries the princess up the slope
    Playing,   // knight climbs, villain attacks
    Won,       // knight reached the top
    GameOver   // knight ran out of lives
};

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();
    void checkCollisions();
    void restart();

    sf::RenderWindow m_window;
    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite;

    sf::Font m_font;
    bool m_fontLoaded;
    sf::Text m_hudText;
    sf::Text m_messageText;

    Knight m_knight;
    Villain m_villain;
    Princess m_princess;
    std::vector<std::unique_ptr<Projectile>> m_projectiles;

    GameState m_state;
    float m_introSpeed; // fraction of the slope the villain covers per second during intro
};
