#pragma once
#include <SFML/Graphics.hpp>

// ===================== MONK (BOSS) =====================
// Represents the old monk enemy. He stays mostly stationary and
// triggers attacks via the AttackManager. Animation/casting poses
// come in Phase 3; actual attack logic comes in Phase 4.
class Monk {
public:
    Monk(sf::Vector2f position)
        : m_shape(sf::Vector2f(50.f, 80.f))
    {
        m_shape.setFillColor(sf::Color(120, 60, 160)); // placeholder purple robe color
        m_shape.setPosition(position);
    }

    void update(float dt) {
        // TODO (Phase 4+): casting animation triggers, idle sway, etc.
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(m_shape);
        // TODO: replace with animated sprite (idle / cast pose) in Phase 3
    }

    sf::Vector2f getHandPosition() const {
        // Approximate position where hand-spell projectiles should spawn from.
        // Placeholder: just offset from top of the shape.
        sf::Vector2f pos = m_shape.getPosition();
        return sf::Vector2f(pos.x + 40.f, pos.y + 20.f);
    }

    sf::Vector2f getPosition() const {
        return m_shape.getPosition();
    }

private:
    sf::RectangleShape m_shape;   // placeholder visual - replace with sprite in Phase 3
};
