#pragma once
#include <vector>
#include<SFML/Graphics.hpp>

class Player{
    private:
    sf::FloatRect hitbox;
    sf::Texture idleTexture;
    sf::Sprite sprite;
    sf::Vector2f velocity;
    static constexpr float GRAVITY = 250;
    bool onGround = false;

    public:
    Player(float playerOriginX,float playerOriginY);
    void update(float dt , const std::vector<sf::FloatRect>& solids);
    void draw(sf::RenderWindow& window);
    void handleInput();
    sf::Vector2f getPosition() const;
    bool loadTextures();
};