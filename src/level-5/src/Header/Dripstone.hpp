#pragma once
#include <SFML/Graphics.hpp>

class Dripstone {
public:
    Dripstone(sf::Vector2f startPos, float speed = 200.f)
        : m_shape(sf::Vector2f(8.f, 30.f)), m_velocity(0.f, speed), m_active(true)
    {
        m_shape.setOrigin(sf::Vector2f(4.f, 0.f));
        m_shape.setFillColor(sf::Color(160, 100, 60));
        m_shape.setPosition(startPos);
    }

    void update(float dt) {
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
    sf::RectangleShape m_shape;
    sf::Vector2f m_velocity;
    bool m_active;
};
