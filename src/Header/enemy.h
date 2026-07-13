#pragma once
#include<vector>
#include<SFML/Graphics.hpp>

 class Enemy {
    private:
    sf::FloatRect hitbox;
    sf::RectangleShape enemyShape;
    float speed=20.f;
    bool movingRight = true;

    public:
    Enemy(float enemyOriginX , float enemyOriginY);
    void update( float dt , const std::vector<sf::FloatRect>& solids);
    void draw(sf::RenderWindow& window);
    sf::Vector2f getPosition() const;
    sf::FloatRect getEnemyHitbox() const;
    void reset(float spawnX, float spawnY);
 };
