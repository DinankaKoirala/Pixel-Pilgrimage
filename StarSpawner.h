#pragma once

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "NinjaStar.h"

class StarSpawnPoint
{
private:
    float crackedPlatformX;
    sf::Vector2f spawnPosition;

public:
    StarSpawnPoint(float crackedX, sf::Vector2f spawnPos);

    float getCrackedPlatformX() const;

    sf::Vector2f getSpawnPosition() const;
};

class StarSpawner
{
public:
    void addSpawnPoint(float crackedX, sf::Vector2f spawnPos);

    void update(
        float playerX,
        std::vector<std::unique_ptr<NinjaStar>>& ninjaStars,
        const sf::Texture& starTexture);

    void clear();

private:
    std::vector<StarSpawnPoint> spawnPoints;
};