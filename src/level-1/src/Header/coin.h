#pragma once
#include<SFML/Graphics.hpp>
#include <string>

class Coin{
    private:
    sf::FloatRect hitbox;
    sf::Sprite sprite;
    sf::Texture coinTexture;
    static int coinsCollected;
    bool collected = false;

    public:
    Coin(float coinOriginX , float coinOriginY);
    void update();
    bool loadTextures(const std::string& path);
    void draw(sf::RenderWindow& window);
    sf::Vector2f getPosition() const;
    sf::FloatRect getCoinHitbox() const;
    void reset(float spawnX, float spawnY);
    bool isCollected();
    void collect();
};