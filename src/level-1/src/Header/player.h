#pragma once
#include <vector>
#include<SFML/Graphics.hpp>
#include"audioManager.h"

class Player{
    private:
    sf::FloatRect hitbox;
    sf::Texture idleTexture;
    sf::Sprite sprite;
    sf::Vector2f velocity;
    static constexpr float GRAVITY = 225;
    bool onGround = false;
    AudioManager& audio;

    public:
    Player(float playerOriginX,float playerOriginY,AudioManager& audio);
    void update(float dt , const std::vector<sf::FloatRect>& solids);
    void draw(sf::RenderWindow& window);
    void handleInput();
    sf::Vector2f getPosition() const;
    bool loadTextures();
    sf::FloatRect getPlayerHitbox() const;
    void reset(float spawnX, float spawnY);
};