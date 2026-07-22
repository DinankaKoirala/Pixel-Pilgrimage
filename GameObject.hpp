#pragma once
#include <SFML/Graphics.hpp>

// Common base class for every visible, updatable thing in the game.
// Demonstrates encapsulation (protected sprite/texture owned by the
// object) and sets up polymorphism (update() is overridden differently
// by each subclass, draw() can be too).
class GameObject {
public:
    virtual ~GameObject() = default;

    virtual void update(float dt) = 0;

    virtual void draw(sf::RenderWindow& window) const {
        window.draw(m_sprite);
    }

    sf::Vector2f getPosition() const {
        return m_sprite.getPosition();
    }

protected:
    sf::Texture m_texture; // owned here so the sprite's texture pointer stays valid
    sf::Sprite m_sprite;
};
