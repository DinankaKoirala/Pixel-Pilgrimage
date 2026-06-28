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

    public:
    static constexpr int TileSize = 32;
    bool loadFromImage(const std::string& path);
    void draw(sf::RenderWindow& window) const;

};