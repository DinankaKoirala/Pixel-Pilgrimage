#pragma once
#include<SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "entity.h"

class Coin : public Entity {
    private:
    sf::Texture coinTexture;
    bool collected = false;

    public:
    static int coinsCollected;
    Coin(float coinOriginX , float coinOriginY);
    void update(float dt, const std::vector<sf::FloatRect>& solids) override;
    bool loadTextures(const std::string& path);
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getHitbox() const override;
    void reset(float spawnX, float spawnY) override;
    bool isCollected();
    void collect();
};
