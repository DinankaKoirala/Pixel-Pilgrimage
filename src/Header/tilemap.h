#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

enum class TileType {
    Air,
    Ground,
    Grass,
    Coin,
    Enemy,
    PlayerSpawn,
    LevelEnd,
    ObstacleGround,
    Trigger
};

class Tilemap{

    private:
        int tileCountX = 0,tileCountY = 0;
        std::vector<std::vector<TileType>> tileGrid;
        sf::Texture stoneTexture , grassTexture;

    public:
        static constexpr int TileSize = 32;
        bool loadFromImage(const std::string& path);
        void draw(sf::RenderWindow& window) const;
        std::vector<sf::FloatRect> getSolidTiles() const;
        bool loadTexture(const std::string& path, const std::string Block);
        std::vector<sf::Vector2f> getEnemySpawnPoints() const;

};