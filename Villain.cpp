#include "Villain.hpp"
#include "Projectile.hpp"
#include "Utility.hpp"
#include <cmath>

Villain::Villain(sf::Vector2f slopeStart, sf::Vector2f slopeEnd)
    : m_slopeStart(slopeStart),
      m_slopeEnd(slopeEnd),
      m_t(0.f),
      m_stoneTimer(0.f),
      m_stoneInterval(2.2f),
      m_arrowTimer(0.f),
      m_arrowInterval(1.6f),
      m_stoneSpeed(230.f),
      m_arrowSpeed(430.f)
{
    m_texture = Utility::loadTextureOrPlaceholder("assets/villain.png", sf::Color(160, 30, 30));
    m_sprite.setTexture(m_texture);

    sf::Vector2u size = m_texture.getSize();
    m_sprite.setOrigin(size.x / 2.f, size.y / 2.f);

    float targetHeight = 100.f;
    float scale = targetHeight / static_cast<float>(size.y);
    m_sprite.setScale(scale, scale);

    updateBasePosition();
    m_sprite.setPosition(m_slopeStart);
}

void Villain::updateBasePosition() {
    sf::Vector2f pos = m_slopeStart + (m_slopeEnd - m_slopeStart) * m_t;
    m_sprite.setPosition(pos);
}

void Villain::advanceTowardTop(float dt, float speed) {
    m_t += speed * dt;
    if (m_t > 1.f) m_t = 1.f;
    updateBasePosition();
}

void Villain::snapToTop() {
    m_t = 1.f;
    updateBasePosition();
}

void Villain::update(float /*dt*/) {
    // Movement is handled explicitly via advanceTowardTop()/snapToTop()
    // depending on game state, so there's nothing to do here every frame.
}

void Villain::attack(float dt, sf::Vector2f heroPosition,
                      std::vector<std::unique_ptr<Projectile>>& outProjectiles)
{
    m_stoneTimer += dt;
    m_arrowTimer += dt;

    sf::Vector2f myPos = getPosition();

    if (m_stoneTimer >= m_stoneInterval) {
        m_stoneTimer = 0.f;

        sf::Vector2f direction(-1.f, 0.6f); // rolls down-left along the slope
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        direction /= length;

        outProjectiles.push_back(
            std::make_unique<RollingStone>(myPos, direction * m_stoneSpeed));
    }

    if (m_arrowTimer >= m_arrowInterval) {
        m_arrowTimer = 0.f;

        sf::Vector2f toHero = heroPosition - myPos;
        float length = std::sqrt(toHero.x * toHero.x + toHero.y * toHero.y);
        if (length < 0.001f) length = 0.001f;
        sf::Vector2f direction = toHero / length;

        outProjectiles.push_back(
            std::make_unique<Arrow>(myPos, direction * m_arrowSpeed));
    }
}

void Villain::reset() {
    m_t = 0.f;
    m_stoneTimer = 0.f;
    m_arrowTimer = 0.f;
    updateBasePosition();
}
