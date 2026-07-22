// Level.cpp
#include "Level.h"
#include "CrackedPlatform.h"
#include <cstdlib>

void createLevel(
    std::vector<std::unique_ptr<Platform>>& platforms,
    StarSpawner& starSpawner,
    const sf::Texture& blockTex,
    const sf::Texture& crackedTex)
{
    platforms.clear();
    starSpawner.clear();

    const float groundY = 600.f;
    const float blockSpacing = 64.f;

    // Purely cosmetic filler ground, placed before the real level start (100.f)
    // so the camera's initial view - which extends left of the player's spawn
    // point - is already covered by solid ground instead of empty space.
    // Built backward from the real start in exact blockSpacing steps so it's
    // perfectly grid-aligned with the real level - otherwise the last filler
    // tile and the first real tile land out of phase and end up overlapping.
    const float fillerReach = -540.f;
    for (float fx = 100.f - blockSpacing; fx >= fillerReach; fx -= blockSpacing)
    {
        platforms.push_back(
            std::make_unique<Platform>(
                blockTex,
                sf::Vector2f(fx, groundY)));
    }

    float x = 100.f;

    for (int i = 0; i < 500; i++)
    {
        if (i != 0 && i % 8 == 0)
        {
            platforms.push_back(
                std::make_unique<CrackedPlatform>(
                    crackedTex,
                    sf::Vector2f(x, groundY)));

            float heights[] = { 500.f, 560.f, 560.f };
            float starY = heights[rand() % 3];

            starSpawner.addSpawnPoint(
                x,
                sf::Vector2f(x + 550.f, starY));
        }
        else
        {
            platforms.push_back(
                std::make_unique<Platform>(
                    blockTex,
                    sf::Vector2f(x, groundY)));
        }

        x += blockSpacing;
    }
}

void restartGame(
    Player& player,
    std::vector<std::unique_ptr<Platform>>& platforms,
    std::vector<std::unique_ptr<NinjaStar>>& ninjaStars,
    StarSpawner& starSpawner,
    bool& gameOver,
    const sf::Texture& blockTex,
    const sf::Texture& crackedTex)
{
    player.shape.setPosition({ 150.f, 550.f });
    player.velocity = { 0.f, 0.f };
    player.onGround = false;

    ninjaStars.clear();

    createLevel(
        platforms,
        starSpawner,
        blockTex,
        crackedTex);

    gameOver = false;
}