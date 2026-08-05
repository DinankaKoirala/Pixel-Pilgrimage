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
#include "death-screen/DeathScreen.h"
#include <SFML/Audio.hpp>

namespace L2 {

class Game
{
public:
    Game(sf::RenderWindow& win);
    bool run();

private:
    bool loadAssets();

    void processEvents();
    void update(float dt);
    void checkCrackTriggers();
    void checkCollisions(float prevPlayerBottom);
    void checkCoinCollisions();
    void render();

    sf::RenderWindow& window;
    sf::View view;
    std::string assetsPath;

    sf::Texture bgTex, blockTex, crackedTex, starTex, coinTex;
    sf::Sprite background;

    std::vector<std::unique_ptr<Platform>> platforms;
    std::vector<std::unique_ptr<NinjaStar>> ninjaStars;
    std::vector<std::unique_ptr<Coin>> coins;
    StarSpawner starSpawner;
    DecorationManager decorations;

    Player player;
    DeathScreen deathScreen;

    //sound
    sf::SoundBuffer coinBuffer;
    std::optional<sf::Sound> coinSound;

    sf::SoundBuffer jumpBuffer;
    std::optional<sf::Sound> jumpSound;

    sf::SoundBuffer crackBuffer;
    std::optional<sf::Sound> crackSound;

    sf::SoundBuffer deathBuffer;
    std::optional<sf::Sound> deathSound;
    sf::Clock clock;
    bool gameOver = false;
    bool gameWon = false;
    int crackedPlatformsPassed = 0;
    int crackedPlatformsToWin = 10;
    float cameraX = 0.f;

    // --- scoring / coins ---
    int score = 0;
    static constexpr int scorePerCoin = 10;
    static constexpr float speedBoostPerCoin = 8.f;

    sf::Font scoreFont;
    sf::Text scoreText{ scoreFont };
    sf::RectangleShape scorePanel;
    std::optional<sf::Sprite> scoreCoin;

    static constexpr float levelEnd = 100.f + 500.f * 64.f;
    static constexpr float levelStart = -540.f;

    // level complete overlay
    sf::Clock levelCompleteClock;
    bool levelCompleteShown = false;
    sf::Font completeFont;
    bool completeFontLoaded = false;
};

} // namespace L2
