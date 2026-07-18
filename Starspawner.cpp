#include "StarSpawner.h"

StarSpawnPoint::StarSpawnPoint(float crackedX, sf::Vector2f spawnPos)
{
    crackedPlatformX = crackedX;
    spawnPosition = spawnPos;
}

float StarSpawnPoint::getCrackedPlatformX() const
{
    return crackedPlatformX;
}

sf::Vector2f StarSpawnPoint::getSpawnPosition() const
{
    return spawnPosition;
}

void StarSpawner::addSpawnPoint(float crackedX, sf::Vector2f spawnPos)
{
    spawnPoints.emplace_back(crackedX, spawnPos);
}

void StarSpawner::update(
    float playerX,
    std::vector<std::unique_ptr<NinjaStar>>& ninjaStars,
    const sf::Texture& starTexture)
{
    for (size_t i = 0; i < spawnPoints.size();)
    {
        if (playerX > spawnPoints[i].getCrackedPlatformX() + 80.f)
        {
            ninjaStars.push_back(
                std::make_unique<NinjaStar>(
                    starTexture,
                    spawnPoints[i].getSpawnPosition(),
                    400.f));

            spawnPoints.erase(spawnPoints.begin() + i);
        }
        else
        {
            ++i;
        }
    }
}

void StarSpawner::clear()
{
    spawnPoints.clear();
}