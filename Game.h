// Game.h
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>
#include "Platform.h"
#include "CrackedPlatform.h"
#include "Player.h"
#include "NinjaStar.h"
#include "StarSpawner.h"
#include "DecorationManager.h"

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
    void render();

    sf::RenderWindow window;
    sf::View view;
    std::string assetsPath;

    sf::Texture bgTex, blockTex, crackedTex, starTex;
    sf::Sprite background;

    std::vector<std::unique_ptr<Platform>> platforms;
    std::vector<std::unique_ptr<NinjaStar>> ninjaStars;
    StarSpawner starSpawner;
    DecorationManager decorations;

    Player player;
    sf::Clock clock;
    bool gameOver = false;
    float cameraX = 0.f;

    static constexpr float levelEnd = 100.f + 500.f * 64.f;
    // Extended left of the real gameplay start (100.f) with filler ground so the
    // camera's initial view is already fully covered - this lets the camera
    // start scrolling immediately instead of sitting frozen for a few seconds.
    // Must match the filler reach built in Level.cpp's createLevel().
    static constexpr float levelStart = -540.f;
};