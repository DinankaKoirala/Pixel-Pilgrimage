// Coin.h
#pragma once
#include "GameObject.h"
#include <cmath>

// A collectible that floats above a platform. Purely decorative animation
// (spin + bob) until the player touches it - Game.cpp is what actually
// marks it collected and reacts (score + speed boost).
class Coin : public GameObject
{
public:
    Coin(const sf::Texture& texture, sf::Vector2f position)
        : GameObject(texture, position)
        , basePosition(position)
    {
        sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f }); // spins/bobs around its center
        sprite.setScale({ coinScale, coinScale });
        sprite.setPosition(position);
    }

    void update(float dt) override
    {
        if (collected) return; // frozen in place once picked up (Game erases it right after anyway)

        elapsed += dt;

        // Fake a spinning coin by squashing the horizontal scale between
        // full width and edge-on thin, like it's rotating on a vertical axis.
        float spin = std::cos(elapsed * spinSpeed);
        sprite.setScale({ coinScale * spin, coinScale });

        // Slow bob so it visually reads as "floating" rather than glued on.
        float bobY = std::sin(elapsed * bobSpeed) * bobAmount;
        sprite.setPosition({ basePosition.x, basePosition.y + bobY });
    }

    void collect() { collected = true; }
    bool isCollected() const { return collected; }

private:
    sf::Vector2f basePosition;
    float elapsed = 0.f;
    bool collected = false;

    const float coinScale = 0.5f;   // 64px art -> 32px on screen, same convention as the ninja star
    const float spinSpeed = 3.f;    // radians/second
    const float bobSpeed = 2.f;
    const float bobAmount = 6.f;    // pixels
};