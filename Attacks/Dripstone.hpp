#pragma once
#include <SFML/Graphics.hpp>

enum class DripstoneState {
    Falling,
    Landed
};

class Dripstone {
public:
    Dripstone(sf::Vector2f startPos, float groundY, float speed = 200.f)
        : m_shape(sf::Vector2f(8.f, 30.f)), m_velocity(0.f, speed), m_groundY(groundY), m_state(DripstoneState::Falling)
    {
        m_shape.setOrigin(sf::Vector2f(4.f, 0.f));
        m_shape.setFillColor(sf::Color(160, 100, 60));
        m_shape.setPosition(startPos);
    }

    void update(float dt) {
        if (m_state != DripstoneState::Falling) return;
        m_shape.move(m_velocity * dt);
        if (m_shape.getPosition().y + m_shape.getSize().y >= m_groundY) {
            m_state = DripstoneState::Landed;
        }
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(m_shape);
    }

    sf::FloatRect getBounds() const {
        return m_shape.getGlobalBounds();
    }

    bool isDangerous() const { return m_state == DripstoneState::Falling; }
    void setLanded() { m_state = DripstoneState::Landed; }
    DripstoneState getState() const { return m_state; }

private:
    sf::RectangleShape m_shape;
    sf::Vector2f m_velocity;
    float m_groundY;
    DripstoneState m_state;
};
