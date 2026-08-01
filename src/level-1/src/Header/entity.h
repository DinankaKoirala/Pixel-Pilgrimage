#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "drawable.h"

class Entity : public Drawable {
    private:
    static const sf::Texture& placeholderTexture()
    {
        static sf::Texture texture;
        return texture;
    }

    protected:
    sf::FloatRect hitbox;
    sf::Sprite sprite;

    Entity() : sprite(placeholderTexture()) {}

    public:
    virtual void update(float dt, const std::vector<sf::FloatRect>& solids) = 0;
    virtual void reset(float spawnX, float spawnY) = 0;
    virtual sf::FloatRect getHitbox() const = 0;

    sf::Vector2f getPosition() const { return hitbox.position; }

    virtual ~Entity() = default;
};
