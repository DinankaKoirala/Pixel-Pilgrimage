#pragma once
#include "GameObject.hpp"
#include "Utility.hpp"
#include <cmath>
#include <algorithm>

// Abstract base for anything the villain throws or shoots. Inherits from
// GameObject to reuse its sprite/texture handling and draw(), and adds
// the collision-related interface every projectile needs.
class Projectile : public GameObject {
public:
    Projectile(sf::Vector2f position, sf::Vector2f velocity)
        : m_velocity(velocity), m_dead(false)
    {
        m_sprite.setPosition(position);
    }

    void update(float dt) override {
        m_sprite.setPosition(m_sprite.getPosition() + m_velocity * dt);
    }

    virtual float getRadius() const = 0;

    // Stones can be jumped over, arrows cannot (in this game's rules).
    virtual bool isStone() const = 0;

    bool isDead() const { return m_dead; }
    void kill() { m_dead = true; }

    bool isOffScreen(unsigned int width, unsigned int height) const {
        const float margin = 80.f;
        sf::Vector2f pos = m_sprite.getPosition();
        return pos.x < -margin || pos.x > width + margin ||
               pos.y < -margin || pos.y > height + margin;
    }

protected:
    sf::Vector2f m_velocity;
    bool m_dead;
};

// A rolling boulder that travels down the slope toward the knight.
class RollingStone : public Projectile {
public:
    RollingStone(sf::Vector2f position, sf::Vector2f velocity)
        : Projectile(position, velocity)
    {
        m_texture = Utility::loadTextureOrPlaceholder("assets/stone.png", sf::Color(110, 110, 110));
        m_sprite.setTexture(m_texture);

        sf::Vector2u size = m_texture.getSize();
        m_sprite.setOrigin(size.x / 2.f, size.y / 2.f);

        float targetSize = 40.f;
        float scale = targetSize / static_cast<float>(std::max(size.x, size.y));
        m_sprite.setScale(scale, scale);
        m_sprite.setPosition(position);
    }

    void update(float dt) override {
        Projectile::update(dt);
        m_rotation += m_rollSpeed * dt;
        m_sprite.setRotation(m_rotation);
    }

    float getRadius() const override { return 20.f; }
    bool isStone() const override { return true; }

private:
    float m_rotation = 0.f;
    float m_rollSpeed = 320.f; // degrees/sec, purely a visual tumbling effect
};

// A fast arrow fired in a straight line toward wherever the knight was
// standing at the moment it was fired.
class Arrow : public Projectile {
public:
    Arrow(sf::Vector2f position, sf::Vector2f velocity)
        : Projectile(position, velocity)
    {
        m_texture = Utility::loadTextureOrPlaceholder("assets/arrow.png", sf::Color(80, 50, 20));
        m_sprite.setTexture(m_texture);

        sf::Vector2u size = m_texture.getSize();
        // Origin on the left-center, so rotation pivots around the nock
        // end and the arrow visually points along its direction of travel.
        m_sprite.setOrigin(0.f, size.y / 2.f);

        float targetLength = 45.f;
        float scale = targetLength / static_cast<float>(size.x);
        m_sprite.setScale(scale, scale);
        m_sprite.setPosition(position);

        float angle = std::atan2(velocity.y, velocity.x) * 180.f / 3.14159265f;
        m_sprite.setRotation(angle);
    }

    float getRadius() const override { return 10.f; }
    bool isStone() const override { return false; }
};
