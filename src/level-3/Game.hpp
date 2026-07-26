#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <optional>
#include "Knight.hpp"
#include "Villain.hpp"
#include "Princess.hpp"
#include "Projectile.hpp"
#include "death-screen/DeathScreen.h"

enum class GameState {
    Intro,
    Playing,
    Won,
    GameOver
};

class Game {
public:
    Game(sf::RenderWindow& window);
    bool run();

private:
    void processEvents();
    void update(float dt);
    void render();
    void checkCollisions();
    void restart();

    sf::RenderWindow& m_window;
    sf::Texture m_backgroundTexture;
    std::optional<sf::Sprite> m_backgroundSprite;

    sf::Font m_font;
    bool m_fontLoaded;
    sf::Text m_hudText{m_font, "", 22};
    sf::Text m_messageText{m_font, "", 36};

    Knight m_knight;
    Villain m_villain;
    Princess m_princess;
    std::vector<std::unique_ptr<Projectile>> m_projectiles;

    GameState m_state;
    float m_introSpeed;

    DeathScreen m_deathScreen;
    bool m_deathHandled = false;

    sf::Clock m_levelCompleteClock;
    bool m_levelCompleteShown = false;
};
