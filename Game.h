// Game.h
#pragma once
#include <SFML/Graphics.hpp>
#include<optional>
#include <vector>
#include <memory>
#include <string>
#include "Platform.h"
#include "CrackedPlatform.h"
#include "Player.h"
#include "NinjaStar.h"
#include "StarSpawner.h"
#include "DecorationManager.h"
#include "Coin.h"

// Owns and runs the entire game: the window, the game loop, and every
// system (platforms, player, ninja stars, decorations, collisions).
// main.cpp only ever needs to construct one Game and call run().
class Game
{
public:
    Game();
    void run();

private:
    bool loadAssets();

    void processEvents();
    void update(float dt);
    void checkCrackTriggers();
    void checkCollisions(float prevPlayerBottom);
    void checkCoinCollisions();     // player touches a coin -> score + gradual speed boost
    void render();

    sf::RenderWindow window;//creates window 
    sf::View view;//acts like the camera
    std::string assetsPath;//assets path 

    sf::Texture bgTex, blockTex, crackedTex, starTex, coinTex;
    sf::Sprite background;

    std::vector<std::unique_ptr<Platform>> platforms;
    std::vector<std::unique_ptr<NinjaStar>> ninjaStars;
    std::vector<std::unique_ptr<Coin>> coins;
    StarSpawner starSpawner;
    DecorationManager decorations;

    Player player;
    sf::Clock clock;
    bool gameOver = false;//tracks wether game ended or not 
    bool gameWon = false;                       // true once player clears enough cracked platforms
    int crackedPlatformsPassed = 0;             // counts triggered cracked platforms
    static constexpr int crackedPlatformsToWin = 10; // win threshold
    float cameraX = 0.f;//stores camera center 

    // --- scoring / coins ---
    int score = 0;
    static constexpr int scorePerCoin = 10;
    static constexpr float speedBoostPerCoin = 8.f; // how much the speed ceiling rises per coin (Player smooths the actual ramp)

    // scoreFont must be declared before scoreText - sf::Text holds a
    // reference to it, and members init in declaration order.
    sf::Font scoreFont;
    sf::Text scoreText{ scoreFont };
    sf::RectangleShape scorePanel;
    std::optional<sf::Sprite> scoreCoin;

    static constexpr float levelEnd = 100.f + 500.f * 64.f;
    // Extended left of the real gameplay start (100.f) with filler ground so the
    // camera's initial view is already fully covered - this lets the camera
    // start scrolling immediately instead of sitting frozen for a few seconds.
    // Must match the filler reach built in Level.cpp's createLevel().
    static constexpr float levelStart = -540.f;
};