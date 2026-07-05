#pragma once
#include <SFML/Graphics.hpp>
#include "AnimatedSprite.hpp"

// ===================== HERO =====================
// Represents the player character.
// Phase 3 adds an animation pipeline: the hero tries to load a spritesheet
// and animate idle/run/jump/dash states. If no spritesheet is found at
// assets/hero_sheet.png, it falls back to the placeholder green rectangle
// so the game keeps working exactly as before with zero art assets.
//
// IMPORTANT: the hitbox (used for movement bounds + collision) is kept
// completely separate from whatever gets drawn. This means swapping in
// real art later never changes how big/small the hero "feels" to hit.
class Hero {
public:
    Hero(sf::Vector2f startPos, float groundY = 550.f, float windowWidth = 800.f)
        : m_position(startPos),
        m_hitboxSize(40.f, 60.f),          // collision size - independent of sprite art size
        m_fallbackShape(m_hitboxSize),      // used only if no spritesheet is found
        m_anim(32, 48),                     // NOTE: adjust to match your actual spritesheet frame size
        m_velocityY(0.f),
        m_groundY(groundY),
        m_windowWidth(windowWidth),
        m_onGround(true),
        m_facingRight(true),
        m_lives(4),
        m_invincible(false),
        m_invincibleTimer(0.f),
        m_isDashing(false),
        m_dashTimer(0.f),
        m_dashCooldownTimer(0.f)
    {
        m_fallbackShape.setFillColor(sf::Color::Green);

        // Try to load real art. If this file doesn't exist yet, hasTexture()
        // will be false and draw() below quietly uses m_fallbackShape instead -
        // nothing else in the game needs to know or care which one is active.
        if (m_anim.loadTexture("assets/hero_sheet.png")) {
            // NOTE: row/frameCount/frameDuration values below are placeholders -
            // adjust them to match whatever spritesheet you actually use.
            m_anim.addAnimation("idle", { 0, 4, 0.15f, true });
            m_anim.addAnimation("run", { 1, 6, 0.08f, true });
            m_anim.addAnimation("jump", { 2, 2, 0.15f, false });
            m_anim.addAnimation("dash", { 3, 4, 0.05f, false });
            m_anim.play("idle");
        }
    }

    void handleInput() {
        m_moveDir = 0.f;

        if (!m_isDashing) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
                m_moveDir = -1.f;
                m_facingRight = false;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
                m_moveDir = 1.f;
                m_facingRight = true;
            }
        }

        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) && m_onGround) {
            m_velocityY = m_jumpStrength;
            m_onGround = false;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) && !m_isDashing && m_dashCooldownTimer <= 0.f) {
            startDash();
        }
    }

    void update(float dt) {
        // --- Dash handling ---
        if (m_isDashing) {
            m_dashTimer -= dt;
            m_position.x += m_dashDir * m_dashSpeed * dt;
            if (m_dashTimer <= 0.f) {
                m_isDashing = false;
            }
        }
        else {
            m_position.x += m_moveDir * m_speed * dt;
        }

        if (m_dashCooldownTimer > 0.f)
            m_dashCooldownTimer -= dt;

        // --- Keep hero inside the window (uses hitbox width, not sprite width) ---
        if (m_position.x < 0.f) m_position.x = 0.f;
        if (m_position.x > m_windowWidth - m_hitboxSize.x) m_position.x = m_windowWidth - m_hitboxSize.x;

        // --- Gravity & vertical movement ---
        m_velocityY += m_gravity * dt;
        m_position.y += m_velocityY * dt;

        // --- Ground collision ---
        if (m_position.y >= m_groundY - m_hitboxSize.y) {
            m_position.y = m_groundY - m_hitboxSize.y;
            m_velocityY = 0.f;
            m_onGround = true;
        }

        // --- Invincibility countdown ---
        if (m_invincible) {
            m_invincibleTimer -= dt;
            if (m_invincibleTimer <= 0.f)
                m_invincible = false;
        }

        // --- Drive animation state from current movement (Phase 3) ---
        if (m_anim.hasTexture()) {
            if (m_isDashing)            m_anim.play("dash");
            else if (!m_onGround)       m_anim.play("jump");
            else if (m_moveDir != 0.f)  m_anim.play("run");
            else                        m_anim.play("idle");

            m_anim.setFacingRight(m_facingRight);
            m_anim.update(dt);

            // Center the sprite over the hitbox so animation frames of
            // different sizes still line up with where the hero actually is.
            sf::Vector2f center(m_position.x + m_hitboxSize.x / 2.f,
                m_position.y + m_hitboxSize.y / 2.f);
            m_anim.setPosition(center);
        }
        else {
            m_fallbackShape.setPosition(m_position);
        }
    }

    void draw(sf::RenderWindow& window) const {
        // Flash semi-transparent while invincible (dash or post-hit i-frames)
        // so the dodge window is readable at a glance - works for both the
        // sprite and the fallback shape.
        if (m_anim.hasTexture()) {
            const_cast<AnimatedSprite&>(m_anim).setColor(
                m_invincible ? sf::Color(255, 255, 255, 130) : sf::Color::White
            );
            m_anim.draw(window);
        }
        else {
            sf::RectangleShape displayShape = m_fallbackShape;
            if (m_invincible) {
                sf::Color c = displayShape.getFillColor();
                c.a = 130;
                displayShape.setFillColor(c);
            }
            window.draw(displayShape);
        }
    }

    // Collision always uses the hitbox, never the visual sprite - this is
    // what keeps hit detection consistent regardless of what art is loaded.
    sf::FloatRect getBounds() const {
        return sf::FloatRect(m_position, m_hitboxSize);
    }

    void takeHit() {
        if (m_invincible) return;
        m_lives--;
        m_invincible = true;
        m_invincibleTimer = 1.0f;
    }

    int getLives() const { return m_lives; }
    bool isDead() const { return m_lives <= 0; }
    bool isInvincible() const { return m_invincible; }
    bool isDashing() const { return m_isDashing; }

    void reset(sf::Vector2f pos) {
        m_position = pos;
        m_velocityY = 0.f;
        m_onGround = true;
        m_lives = 4;
        m_invincible = false;
        m_invincibleTimer = 0.f;
        m_isDashing = false;
        m_dashTimer = 0.f;
        m_dashCooldownTimer = 0.f;
    }

private:
    void startDash() {
        m_isDashing = true;
        m_dashTimer = m_dashDuration;
        m_dashCooldownTimer = m_dashCooldownDuration;
        m_dashDir = m_facingRight ? 1.f : -1.f;

        m_invincible = true;
        m_invincibleTimer = m_dashDuration;
    }

    // --- Position & hitbox (authoritative game logic, independent of art) ---
    sf::Vector2f m_position;
    sf::Vector2f m_hitboxSize;

    // --- Visuals: fallback shape OR animated sprite, whichever is active ---
    sf::RectangleShape m_fallbackShape;
    AnimatedSprite m_anim;

    // --- Movement state ---
    float m_moveDir = 0.f;
    float m_velocityY;
    float m_groundY;
    float m_windowWidth;
    bool m_onGround;
    bool m_facingRight;

    // --- Movement tuning ---
    float m_speed = 300.f;
    float m_gravity = 900.f;
    float m_jumpStrength = -450.f;

    // --- Lives / invincibility ---
    int m_lives;
    bool m_invincible;
    float m_invincibleTimer;

    // --- Dash / dodge-roll ---
    bool m_isDashing;
    float m_dashTimer;
    float m_dashDir = 1.f;
    float m_dashCooldownTimer;
    float m_dashSpeed = 900.f;
    float m_dashDuration = 0.2f;
    float m_dashCooldownDuration = 0.8f;
};