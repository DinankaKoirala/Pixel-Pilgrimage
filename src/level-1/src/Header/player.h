#pragma once
#include <vector>
#include<SFML/Graphics.hpp>
#include"audioManager.h"

enum class PlayerState { Idle, Walk, Jump, Crouch };

class Player{
    private:
    sf::FloatRect hitbox;
    sf::Texture idleTexture;
    sf::Texture walkTexture;
    sf::Texture jumpTexture;
    sf::Texture crouchTexture;

    sf::Sprite sprite;
    sf::Vector2f velocity;
    static constexpr float GRAVITY = 225;
    bool onGround = false;
    AudioManager& audio;

    PlayerState state = PlayerState::Idle;
    bool facingRight = true;
    bool isCrouching = false;
    float animTimer = 0.f;
    int currentFrame = 0;

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