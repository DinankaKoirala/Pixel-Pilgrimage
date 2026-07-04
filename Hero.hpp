#pragma once
#include <SFML/Graphics.hpp>

// ===================== HERO =====================
// Represents the player character.
// Phase 2 adds: left/right movement, jumping with gravity,
// and a fast dash/dodge-roll with brief invincibility frames.
class Hero {
public:
    Hero(sf::Vector2f startPos, float groundY = 550.f, float windowWidth = 800.f)
        : m_shape(sf::Vector2f(40.f, 60.f)),   // placeholder rectangle until sprites are added
        m_velocityY(0.f),
        m_groundY(groundY),                  // y-coordinate of the floor surface
        m_windowWidth(windowWidth),
        m_onGround(true),
        m_facingRight(true),
        m_lives(4),                          // hero can take 4 hits before dying on the 5th
        m_invincible(false),
        m_invincibleTimer(0.f),
        m_isDashing(false),
        m_dashTimer(0.f),
        m_dashCooldownTimer(0.f)
    {
        m_shape.setFillColor(sf::Color::Green); // temp placeholder color, replace with sprite later
        m_shape.setPosition(startPos);
    }

    void handleInput() {
        // Reset per-frame move direction; movement is re-evaluated every frame
        // based on what's currently held down.
        m_moveDir = 0.f;

        if (!m_isDashing) {
            // While dashing, we ignore normal left/right input so the dash
            // burst isn't overridden mid-move (feels more like a committed action).
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
                m_moveDir = -1.f;
                m_facingRight = false;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
                m_moveDir = 1.f;
                m_facingRight = true;
            }
        }

        // Jump - only allowed while standing on the ground
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) && m_onGround) {
            m_velocityY = m_jumpStrength;
            m_onGround = false;
        }

        // Dash/dodge-roll - triggered on Left Shift, only if not already dashing
        // and the cooldown has expired. This is the hero's main defensive tool
        // against the monk's attacks, since it grants temporary invincibility.
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) && !m_isDashing && m_dashCooldownTimer <= 0.f) {
            startDash();
        }
    }

    void update(float dt) {
        sf::Vector2f pos = m_shape.getPosition();

        // --- Dash handling ---
        if (m_isDashing) {
            m_dashTimer -= dt;
            // While dashing, move fast in whatever direction the hero was facing
            // when the dash started (locked in startDash(), see m_dashDir).
            pos.x += m_dashDir * m_dashSpeed * dt;

            if (m_dashTimer <= 0.f) {
                m_isDashing = false;
            }
        }
        else {
            // --- Normal horizontal movement ---
            pos.x += m_moveDir * m_speed * dt;
        }

        // Tick down dash cooldown regardless of dash state
        if (m_dashCooldownTimer > 0.f)
            m_dashCooldownTimer -= dt;

        // --- Keep hero inside the window ---
        float width = m_shape.getSize().x;
        if (pos.x < 0.f) pos.x = 0.f;
        if (pos.x > m_windowWidth - width) pos.x = m_windowWidth - width;

        // --- Gravity & vertical movement ---
        m_velocityY += m_gravity * dt;
        pos.y += m_velocityY * dt;

        // --- Ground collision ---
        float height = m_shape.getSize().y;
        if (pos.y >= m_groundY - height) {
            pos.y = m_groundY - height;
            m_velocityY = 0.f;
            m_onGround = true;
        }

        m_shape.setPosition(pos);

        // --- Invincibility countdown (from either a hit or a dash) ---
        if (m_invincible) {
            m_invincibleTimer -= dt;
            if (m_invincibleTimer <= 0.f)
                m_invincible = false;
        }

        // TODO (Phase 3): drive animation state here (idle/run/jump/dash) based on
        // m_onGround, m_moveDir, and m_isDashing
    }

    void draw(sf::RenderWindow& window) const {
        // Simple visual feedback: flash/fade while invincible so dashing and
        // post-hit invulnerability are readable at a glance (placeholder until
        // real animations exist in Phase 3).
        sf::RectangleShape displayShape = m_shape;
        if (m_invincible) {
            sf::Color c = displayShape.getFillColor();
            c.a = 130; // semi-transparent while invincible
            displayShape.setFillColor(c);
        }
        window.draw(displayShape);
    }

    sf::FloatRect getBounds() const {
        return m_shape.getGlobalBounds();
    }

    // Called by collision system when hero is hit by any hazard
    void takeHit() {
        if (m_invincible) return;   // ignore hits during invincibility window (includes dash i-frames)

        m_lives--;
        m_invincible = true;
        m_invincibleTimer = 1.0f;   // 1 second of temporary invincibility after being hit
    }

    int getLives() const { return m_lives; }
    bool isDead() const { return m_lives <= 0; }
    bool isInvincible() const { return m_invincible; }
    bool isDashing() const { return m_isDashing; }

    void reset(sf::Vector2f pos) {
        m_shape.setPosition(pos);
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

        // Dashing grants invincibility for its full duration - this is the
        // hero's main tool for dodging fireballs/dripstone/spells safely.
        m_invincible = true;
        m_invincibleTimer = m_dashDuration;
    }

    sf::RectangleShape m_shape;   // placeholder visual - replace with sf::Sprite in Phase 3

    // --- Movement state ---
    float m_moveDir = 0.f;        // -1 = left, 0 = idle, 1 = right (set each frame in handleInput)
    float m_velocityY;
    float m_groundY;
    float m_windowWidth;
    bool m_onGround;
    bool m_facingRight;

    // --- Movement tuning ---
    float m_speed = 300.f;         // horizontal walk speed (pixels/sec)
    float m_gravity = 900.f;       // downward acceleration (pixels/sec^2)
    float m_jumpStrength = -450.f; // instantaneous upward velocity on jump (negative = up)

    // --- Lives / invincibility ---
    int m_lives;
    bool m_invincible;
    float m_invincibleTimer;

    // --- Dash / dodge-roll ---
    bool m_isDashing;
    float m_dashTimer;
    float m_dashDir = 1.f;
    float m_dashCooldownTimer;
    float m_dashSpeed = 900.f;              // dash is roughly 3x normal move speed - fast, committed dodge
    float m_dashDuration = 0.2f;            // how long the dash burst + invincibility lasts
    float m_dashCooldownDuration = 0.8f;    // time before hero can dash again
};