#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "level4.h"
#include "death-screen/DeathScreen.h"
#include "win/WinScreen.h"

class Game4 {
public:
    Game4();
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();
    void spawnObjects(float dt);
    void checkCollisions();

    sf::RenderWindow window;
    sf::Clock clock;

    Sky sky;
    Snowfall snowfall;
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
    WinScreen winScreen;

    float gameSpeed;
    float score;
    int coinCount;
    int lives;
    bool gameOver;
    bool gameWon;
    bool deathHandled;
    bool winHandled;

    float spawnTimer;
    float treeTimer;
    float deerTimer;
    float giantTimer;
    float distTimer;

    static constexpr float WIN_SCORE = 1000.f;
};
