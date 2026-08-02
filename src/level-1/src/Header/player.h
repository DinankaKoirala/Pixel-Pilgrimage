#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "entity.h"
#include "audioManager.h"

enum class PlayerState { Idle, Walk, Jump, Crouch };

class Player : public Entity {
    private:
    sf::Texture idleTexture;
    sf::Texture walkTexture;
    sf::Texture jumpTexture;
    sf::Texture crouchTexture;

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
    void update(float dt , const std::vector<sf::FloatRect>& solids) override;
    void draw(sf::RenderWindow& window) override;
    void handleInput();
    bool loadTextures();
    sf::FloatRect getHitbox() const override;
    void reset(float spawnX, float spawnY) override;
};
