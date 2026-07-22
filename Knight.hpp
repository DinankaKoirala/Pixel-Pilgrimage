#pragma once
#include "GameObject.hpp"

// The player-controlled knight. Moves along a fixed slope line (t = 0 at
// the bottom, t = 1 at the top where the villain is), can jump to dodge
// rolling stones, and tracks its own 3 lives.
class Knight : public GameObject {
public:
    Knight(sf::Vector2f slopeStart, sf::Vector2f slopeEnd);

    void moveUp(float dt);
    void moveDown(float dt);
    void jump();

    void update(float dt) override;

    void takeDamage(int amount);
    void reset();

    bool isJumping() const { return m_jumping; }
    bool isInvulnerable() const { return m_invulnTimer > 0.f; }
    bool isAlive() const { return m_lives > 0; }
    int getLives() const { return m_lives; }
    float getRadius() const { return 28.f; }

    // 0.0 = bottom of the slope, 1.0 = reached the villain/princess
    float getProgress() const { return m_t; }

private:
    void updateBasePosition();

    sf::Vector2f m_slopeStart;
    sf::Vector2f m_slopeEnd;
    sf::Vector2f m_slopeDir;
    sf::Vector2f m_perpendicular;

    float m_t;
    float m_speed;

    sf::Vector2f m_basePos;

    bool m_jumping;
    float m_jumpTimer;
    float m_jumpDuration;
    float m_jumpHeight;

    int m_lives;
    float m_invulnTimer;      // brief window of immunity right after getting hit
    const float m_invulnDuration = 1.2f;
};
