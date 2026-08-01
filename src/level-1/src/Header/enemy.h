#pragma once
#include<vector>
#include<SFML/Graphics.hpp>
#include "entity.h"

 class Enemy : public Entity {
    private:
    sf::Texture enemyTexture;
    float speed=20.f;
    bool movingRight = true;

    public:
    Enemy(float enemyOriginX , float enemyOriginY);
    void update( float dt , const std::vector<sf::FloatRect>& solids) override;
    bool loadTextures(const std::string& path);
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getHitbox() const override;
    void reset(float spawnX, float spawnY) override;
 };
