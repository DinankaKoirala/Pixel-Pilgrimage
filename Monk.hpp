#pragma once
#include <SFML/Graphics.hpp>
#include "AnimatedSprite.hpp"

// ===================== MONK (BOSS) =====================
// Represents the old monk enemy. Same fallback pattern as Hero: tries to
// load a spritesheet for idle/cast animations, and quietly falls back to
// the placeholder purple rectangle if no art file is found yet.
class Monk {
public:
    Monk(sf::Vector2f position)
        : m_position(position),
        m_size(50.f, 80.f),
        m_fallbackShape(m_size),
        m_anim(48, 64),   // NOTE: adjust to match your actual spritesheet frame size
        m_castTimer(0.f)
    {
        m_fallbackShape.setFillColor(sf::Color(120, 60, 160));
        m_fallbackShape.setPosition(m_position);

        if (m_anim.loadTexture("assets/monk_sheet.png")) {
            // NOTE: placeholder values - adjust rows/frame counts to match your art
            m_anim.addAnimation("idle", { 0, 4, 0.2f, true });
            m_anim.addAnimation("cast", { 1, 5, 0.1f, false });
            m_anim.play("idle");
        }
    }

    void update(float dt) {
        // Auto-revert to idle a short moment after casting, since we don't
        // have real animation-finished detection wired up yet - a fixed
        // "cast pose duration" is a simple stand-in for that.
        if (m_castTimer > 0.f) {
            m_castTimer -= dt;
            if (m_anim.hasTexture()) m_anim.play("cast");
        }
        else if (m_anim.hasTexture()) {
            m_anim.play("idle");
        }

        if (m_anim.hasTexture()) {
            m_anim.update(dt);
            sf::Vector2f center(m_position.x + m_size.x / 2.f, m_position.y + m_size.y / 2.f);
            m_anim.setPosition(center);
        }
    }

    void draw(sf::RenderWindow& window) const {
        if (m_anim.hasTexture())
            m_anim.draw(window);
        else
            window.draw(m_fallbackShape);
    }

    // Play the casting animation - called by AttackManager right before
    // spawning a fireball/hand-spell, giving the player a visual "tell"
    // that an attack is incoming.
    void playCastAnimation() {
        m_castTimer = 0.4f;   // brief cast pose duration
        if (m_anim.hasTexture())
            m_anim.play("cast");
    }

    sf::Vector2f getHandPosition() const {
        // Approximate spawn point for hand-spell projectiles.
        return sf::Vector2f(m_position.x + 40.f, m_position.y + 20.f);
    }

    sf::Vector2f getPosition() const {
        return m_position;
    }

private:
    sf::Vector2f m_position;
    sf::Vector2f m_size;
    sf::RectangleShape m_fallbackShape;
    AnimatedSprite m_anim;
    float m_castTimer;
};