#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <optional>
#include "level4.h"
#include "death-screen/DeathScreen.h"

namespace L4 {

class Game4 {
public:
    Game4(sf::RenderWindow& win);
    bool run();

private:
    void processEvents();
    void update(float dt);
    void render();
    void spawnObjects(float dt);
    void checkCollisions();
    void loadSounds();

    sf::RenderWindow& window;
    sf::Clock clock;

    Sky sky;
    Snowfall snowfall;
    sf::Texture tilemapTex;
    Ground ground;
    std::vector<Tree> trees;
    std::vector<Coin> coins;
    std::vector<Rock> rocks;
    std::vector<DeerEnemy> deerEnemies;
    std::vector<Giant> giants;
    Player player;
    HUD hud;
    GameOverScreen gameOverScreen;

    DeathScreen deathScreen;

    sf::SoundBuffer jumpBuffer;
    std::optional<sf::Sound> jumpSound;
    sf::SoundBuffer coinBuffer;
    std::optional<sf::Sound> coinSound;
    sf::SoundBuffer heartBuffer;
    std::optional<sf::Sound> heartSound;
    sf::SoundBuffer gameOverBuffer;
    std::optional<sf::Sound> gameOverSound;
    sf::SoundBuffer deerBuffer;
    std::optional<sf::Sound> deerSound;
    sf::SoundBuffer yetiBuffer;
    std::optional<sf::Sound> yetiSound;

    float gameSpeed;
    float score;
    int coinCount;
    int lives;
    bool gameOver;
    bool gameWon;
    bool deathHandled;

    float spawnTimer;
    float treeTimer;
    float deerTimer;
    float giantTimer;
    float distTimer;

    float tilemapScroll = 0;

    static constexpr float WIN_SCORE = 500.f;
};

} // namespace L4
