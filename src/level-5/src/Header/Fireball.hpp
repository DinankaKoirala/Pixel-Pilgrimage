#pragma once
#include <SFML/Graphics.hpp>

// ===================== FIREBALL ATTACK =====================
// A single fireball thrown by the monk, aimed at the hero.
// Logic for direction/speed will be filled in during Phase 4.
class Fireball {
public:
    Fireball(sf::Vector2f startPos, sf::Vector2f velocity, float radius = 8.f)
        : m_shape(radius), m_velocity(velocity), m_active(true)
    {
        m_shape.setOrigin(sf::Vector2f(radius, radius));
        m_shape.setFillColor(sf::Color(255, 80, 0));
        m_shape.setPosition(startPos);
    }

    void update(float dt) {
        // TODO (Phase 4): move using m_velocity, possibly add slight homing/curve
        m_shape.move(m_velocity * dt);
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(m_shape);
    }

    sf::FloatRect getBounds() const {
        return m_shape.getGlobalBounds();
    }

    bool isActive() const { return m_active; }
    void deactivate() { m_active = false; }

private:
    sf::CircleShape m_shape;
    sf::Vector2f m_velocity;
    bool m_active;
};
