#pragma once
#include <SFML/Graphics.hpp>

// ===================== HAND SPELL ATTACK =====================
// A projectile (or beam) cast from the monk's hand toward the hero.
// Actual targeting/shape logic comes in Phase 4.
class HandSpell {
public:
    HandSpell(sf::Vector2f startPos, sf::Vector2f velocity)
        : m_shape(sf::Vector2f(15.f, 15.f)), m_velocity(velocity)
    {
        m_shape.setOrigin(sf::Vector2f(7.5f, 7.5f));
        m_shape.setFillColor(sf::Color(180, 0, 255)); // placeholder purple magic bolt
        m_shape.setPosition(startPos);
    }

    void update(float dt) {
        // TODO (Phase 4): straight-line or homing movement toward hero
        m_shape.move(m_velocity * dt);
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(m_shape);
    }

    sf::FloatRect getBounds() const {
        return m_shape.getGlobalBounds();
    }

private:
    sf::RectangleShape m_shape;
    sf::Vector2f m_velocity;
};
