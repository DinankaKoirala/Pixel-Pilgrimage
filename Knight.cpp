#include "Knight.hpp"
#include "Utility.hpp"
#include <cmath>

Knight::Knight(sf::Vector2f slopeStart, sf::Vector2f slopeEnd)
    : m_slopeStart(slopeStart),
      m_slopeEnd(slopeEnd),
      m_t(0.f),
      m_speed(0.16f),
      m_jumping(false),
      m_jumpTimer(0.f),
      m_jumpDuration(0.55f),
      m_jumpHeight(80.f),
      m_lives(3),
      m_invulnTimer(0.f)
{
    sf::Vector2f diff = m_slopeEnd - m_slopeStart;
    float length = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    m_slopeDir = diff / length;

    m_perpendicular = sf::Vector2f(-m_slopeDir.y, m_slopeDir.x);
    if (m_perpendicular.y > 0.f) {
        m_perpendicular = -m_perpendicular;
    }

    m_texture = Utility::loadTextureOrPlaceholder("assets/knight.png", sf::Color(120, 170, 220));
    m_sprite.setTexture(m_texture);

    sf::Vector2u size = m_texture.getSize();
    m_sprite.setOrigin(size.x / 2.f, size.y / 2.f);

    // Scale the artwork down to a sensible in-game size (~95px tall).
    float targetHeight = 95.f;
    float scale = targetHeight / static_cast<float>(size.y);
    m_sprite.setScale(scale, scale);

    updateBasePosition();
    m_sprite.setPosition(m_basePos);
}

void Knight::updateBasePosition() {
    m_basePos = m_slopeStart + (m_slopeEnd - m_slopeStart) * m_t;
}

void Knight::moveUp(float dt) {
    m_t += m_speed * dt;
    if (m_t > 1.f) m_t = 1.f;
    updateBasePosition();
}

void Knight::moveDown(float dt) {
    m_t -= m_speed * dt;
    if (m_t < 0.f) m_t = 0.f;
    updateBasePosition();
}

void Knight::jump() {
    if (!m_jumping) {
        m_jumping = true;
        m_jumpTimer = 0.f;
    }
}

void Knight::update(float dt) {
    if (m_invulnTimer > 0.f) {
        m_invulnTimer -= dt;
    }

    sf::Vector2f drawPos;
    if (m_jumping) {
        m_jumpTimer += dt;
        float progress = m_jumpTimer / m_jumpDuration;
        if (progress >= 1.f) {
            m_jumping = false;
            drawPos = m_basePos;
        } else {
            float arc = 4.f * progress * (1.f - progress); // 0 -> 1 -> 0
            drawPos = m_basePos + m_perpendicular * (m_jumpHeight * arc);
        }
    } else {
        drawPos = m_basePos;
    }

    m_sprite.setPosition(drawPos);

    // Flash the knight while invulnerable so the player can see the hit registered.
    if (isInvulnerable()) {
        bool visible = static_cast<int>(m_invulnTimer * 10) % 2 == 0;
        m_sprite.setColor(visible ? sf::Color::White : sf::Color(255, 255, 255, 90));
    } else {
        m_sprite.setColor(sf::Color::White);
    }
}

void Knight::takeDamage(int amount) {
    if (isInvulnerable()) return;
    m_lives -= amount;
    if (m_lives < 0) m_lives = 0;
    m_invulnTimer = m_invulnDuration;
}

void Knight::reset() {
    m_t = 0.f;
    m_lives = 3;
    m_jumping = false;
    m_jumpTimer = 0.f;
    m_invulnTimer = 0.f;
    updateBasePosition();
    m_sprite.setPosition(m_basePos);
    m_sprite.setColor(sf::Color::White);
}
