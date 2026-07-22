#pragma once
#include <vector>
#include <memory>
#include "GameObject.hpp"

class Projectile; // forward declaration to avoid a circular include

// The villain. During the intro he climbs the slope carrying the princess.
// Once he reaches the top, he plants himself there and periodically spawns
// rolling stones and arrows aimed at the knight.
class Villain : public GameObject {
public:
    Villain(sf::Vector2f slopeStart, sf::Vector2f slopeEnd);

    // Used only during the intro: walks him up the slope over time.
    void advanceTowardTop(float dt, float speed);
    bool hasReachedTop() const { return m_t >= 1.f; }
    void snapToTop();

    void update(float dt) override; // no-op movement here; attacking is separate

    // Spawns projectiles into 'outProjectiles' when timers fire. Only call
    // this once he's settled at the top (i.e. after the intro).
    void attack(float dt, sf::Vector2f heroPosition,
                std::vector<std::unique_ptr<Projectile>>& outProjectiles);

    void reset();

private:
    void updateBasePosition();

    sf::Vector2f m_slopeStart;
    sf::Vector2f m_slopeEnd;
    float m_t;

    float m_stoneTimer;
    float m_stoneInterval;
    float m_arrowTimer;
    float m_arrowInterval;
    float m_stoneSpeed;
    float m_arrowSpeed;
};
