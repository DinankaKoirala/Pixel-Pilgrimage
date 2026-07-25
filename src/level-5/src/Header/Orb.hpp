#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include "Fireball.hpp"

// ===================== ORB ATTACK =====================
// Spawns in the middle of the screen and continuously emits fireballs
// in random directions. This reuses the logic from the earlier
// orb+fireball demo, adapted to fit this project's structure.
class Orb {
public:
    Orb(sf::Vector2f position, float radius = 20.f)
        : m_shape(radius), m_spawnTimer(0.f),
          m_spawnInterval(0.4f), m_fireballSpeed(250.f), m_active(false)
    {
        m_shape.setOrigin(sf::Vector2f(radius, radius));
        m_shape.setFillColor(sf::Color::Yellow);
        m_shape.setPosition(position);
    }

    void activate() { m_active = true; }
    void deactivate() { m_active = false; }
    bool isActive() const { return m_active; }

    void update(float dt, std::vector<Fireball>& fireballs) {
        if (!m_active) return;

        // TODO (Phase 4): tune spawn rate/pattern (radial ring vs random angles)
        m_spawnTimer += dt;
        if (m_spawnTimer >= m_spawnInterval) {
            m_spawnTimer = 0.f;
            spawnFireball(fireballs);
        }
    }

    void draw(sf::RenderWindow& window) const {
        if (m_active)
            window.draw(m_shape);
    }

    void setSpawnInterval(float seconds) { m_spawnInterval = seconds; }
    void setFireballSpeed(float speed)   { m_fireballSpeed = speed; }

private:
    void spawnFireball(std::vector<Fireball>& fireballs) {
        float angle = static_cast<float>(rand() % 360) * 3.14159265f / 180.f;
        sf::Vector2f velocity(std::cos(angle) * m_fireballSpeed,
                               std::sin(angle) * m_fireballSpeed);
        fireballs.emplace_back(m_shape.getPosition(), velocity);
    }

    sf::CircleShape m_shape;
    float m_spawnTimer;
    float m_spawnInterval;
    float m_fireballSpeed;
    bool m_active;
};
