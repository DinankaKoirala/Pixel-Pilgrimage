// Level.cpp
#include "Level.h"
#include "CrackedPlatform.h"
#include <cstdlib>

void createLevel(
    std::vector<std::unique_ptr<Platform>>& platforms,
    std::vector<std::unique_ptr<Coin>>& coins,
    StarSpawner& starSpawner,
    const sf::Texture& blockTex,
    const sf::Texture& crackedTex,
    const sf::Texture& coinTex)
{
    platforms.clear();
    coins.clear();
    starSpawner.clear();//removes old spawn points so new ones can be added for the new level

    const float groundY = 600.f;//platform height 
    const float blockSpacing = 64.f;//distance between platforms
    const int coinInterval = 5; // one coin above every 5th regular (non-cracked) platform
    const float coinHeight = 90.f; // how far above the platform the coin floats - tall enough it needs a real jump

    //Creates extra blocks left of the starting point.
    //without these blocks camera starts shows empty background 
    const float fillerReach = -540.f;
    for (float fx = 100.f - blockSpacing; fx >= fillerReach; fx -= blockSpacing)
    {
        platforms.push_back(
            std::make_unique<Platform>(
                blockTex,
                sf::Vector2f(fx, groundY)));
    }

    float x = 100.f;//starting position 

    for (int i = 0; i < 500; i++)//cracked platform logic 
    {
        if (i != 0 && i % 8 == 0)
        {
            platforms.push_back(
                std::make_unique<CrackedPlatform>(
                    crackedTex,
                    sf::Vector2f(x, groundY)));

            float heights[] = { 450.f, 560.f, 560.f };//possible spawn heights for ninja stars
            float starY = heights[rand() % 3];

            starSpawner.addSpawnPoint(
                x,
                sf::Vector2f(x + 550.f, starY));//It delays the ninja star so it appears after the cracked platform instead of immediately.
        }
        else
        {
            platforms.push_back(
                std::make_unique<Platform>(
                    blockTex,
                    sf::Vector2f(x, groundY)));

            // Every Nth normal platform gets a coin floating above it.
            // Only ever placed on solid ground, never over a cracked
            // plank, so grabbing one is always a safe jump.
            if (i != 0 && i % coinInterval == 0)
            {
                coins.push_back(
                    std::make_unique<Coin>(
                        coinTex,
                        sf::Vector2f(x + 32.f, groundY - coinHeight)));
            }
        }

        x += blockSpacing;
    }
}

void restartGame(
    Player& player,
    std::vector<std::unique_ptr<Platform>>& platforms,
    std::vector<std::unique_ptr<NinjaStar>>& ninjaStars,
    std::vector<std::unique_ptr<Coin>>& coins,
    StarSpawner& starSpawner,
    bool& gameOver,
    const sf::Texture& blockTex,
    const sf::Texture& crackedTex,
    const sf::Texture& coinTex)
{
    player.shape.setPosition({ 150.f, 550.f });
    player.velocity = { 0.f, 0.f };
    player.onGround = false;
    player.resetSpeed(); // undo any coin speed boosts from the last run

    ninjaStars.clear();

    createLevel(
        platforms,
        coins,
        starSpawner,
        blockTex,
        crackedTex,
        coinTex);

    gameOver = false;
}