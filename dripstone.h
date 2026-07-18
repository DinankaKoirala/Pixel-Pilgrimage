#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <cstdlib>

// ===================== DRIPSTONE ATTACK =====================
// A ceiling hazard that telegraphs (warns) before falling, then
// drops fast under gravity, and lands on the cave floor.
enum class DripstoneState {
    Telegraphing,   // warning shown, not yet falling - NOT dangerous to touch
    Falling,        // actively dropping - dangerous
    Landed          // hit the ground - about to be removed by AttackManager
};

class Dripstone {
public:
    Dripstone(sf::Vector2f ceilingPos, float groundY = 550.f)
        : m_shape(sf::Vector2f(20.f, 40.f)),
        m_warningShape(sf::Vector2f(20.f, 40.f)),
        m_state(DripstoneState::Telegraphing),
        m_telegraphTimer(0.5f),   // half-second warning before falling - keep readable but fast
        m_telegraphDuration(0.5f),
        m_fallSpeed(0.f),
        m_gravity(1500.f),
        m_groundY(groundY)
    {
        m_shape.setFillColor(sf::Color(150, 150, 150));
        m_shape.setPosition(ceilingPos);

        // The warning indicator sits at the same x position but drawn
        // as a thin, semi-transparent sliver so players can see exactly
        // where it's about to fall before it actually drops.
        m_warningShape.setFillColor(sf::Color(255, 60, 60, 90));
        m_warningShape.setPosition(ceilingPos);
    }

    void update(float dt) {
        if (m_state == DripstoneState::Telegraphing) {
            m_telegraphTimer -= dt;
            if (m_telegraphTimer <= 0.f)
                m_state = DripstoneState::Falling;
        }
        else if (m_state == DripstoneState::Falling) {
            m_fallSpeed += m_gravity * dt;
            m_shape.move(sf::Vector2f(0.f, m_fallSpeed * dt));

            // Land once the bottom edge reaches the cave floor.
            sf::Vector2f pos = m_shape.getPosition();
            float height = m_shape.getSize().y;
            if (pos.y + height >= m_groundY) {
                m_shape.setPosition(sf::Vector2f(pos.x, m_groundY - height));
                m_state = DripstoneState::Landed;
            }
        }
    }

    void draw(sf::RenderWindow& window) const {
        if (m_state == DripstoneState::Telegraphing) {
            // Flicker the warning sliver so it reads as "about to fall"
            // rather than just a static shape (cheap but effective juice).
            window.draw(m_warningShape);
        }
        else {
            window.draw(m_shape);
        }
    }

    sf::FloatRect getBounds() const {
        return m_shape.getGlobalBounds();
    }

    DripstoneState getState() const { return m_state; }
    void setLanded() { m_state = DripstoneState::Landed; }

    // Only the Falling state should count as a hazard - touching the
    // telegraph warning or the already-landed rubble doesn't hurt the hero.
    bool isDangerous() const { return m_state == DripstoneState::Falling; }

private:
    sf::RectangleShape m_shape;
    sf::RectangleShape m_warningShape;
    DripstoneState m_state;
    float m_telegraphTimer;
    float m_telegraphDuration;
    float m_fallSpeed;
    float m_gravity;
    float m_groundY;
};

// ===================== DRIPSTONE MANAGER =====================
// Owns the whole set of active dripstones: spawns new ones on a timer,
// updates/draws them each frame, and answers collision queries.
// Landed dripstones are kept around briefly (as visible rubble) before
// being cleaned up automatically.
class DripstoneManager {
public:
    DripstoneManager(float spawnInterval = 1.5f,
        float minX = 50.f,
        float maxX = 750.f,
        float ceilingY = 0.f,
        float groundY = 550.f)
        : m_spawnTimer(0.f),
        m_spawnInterval(spawnInterval),
        m_minX(minX),
        m_maxX(maxX),
        m_ceilingY(ceilingY),
        m_groundY(groundY)
    {
    }

    void update(float dt) {
        m_spawnTimer += dt;
        if (m_spawnTimer >= m_spawnInterval) {
            m_spawnTimer = 0.f;

            float span = m_maxX - m_minX;
            float x = m_minX + static_cast<float>(rand() % static_cast<int>(span > 0.f ? span : 1.f));

            m_entries.push_back({ Dripstone(sf::Vector2f(x, m_ceilingY), m_groundY), 0.f });
        }

        for (auto& entry : m_entries) {
            entry.stone.update(dt);
            if (entry.stone.getState() == DripstoneState::Landed) {
                entry.landedTimer += dt;
            }
        }

        // Give landed rubble a moment on screen before removing it.
        const float landedLifetime = 1.5f;
        m_entries.erase(
            std::remove_if(m_entries.begin(), m_entries.end(),
                [landedLifetime](const Entry& entry) {
                    return entry.stone.getState() == DripstoneState::Landed &&
                        entry.landedTimer >= landedLifetime;
                }),
            m_entries.end());
    }

    void draw(sf::RenderWindow& window) const {
        for (const auto& entry : m_entries) {
            entry.stone.draw(window);
        }
    }

    // True if the player's bounds overlap any currently-falling dripstone.
    bool checkCollision(const sf::FloatRect& playerBounds) const {
        for (const auto& entry : m_entries) {
            if (entry.stone.isDangerous() &&
                entry.stone.getBounds().findIntersection(playerBounds)) {
                return true;
            }
        }
        return false;
    }

    void reset() {
        m_entries.clear();
        m_spawnTimer = 0.f;
    }

private:
    struct Entry {
        Dripstone stone;
        float landedTimer;
    };

    std::vector<Entry> m_entries;
    float m_spawnTimer;
    float m_spawnInterval;
    float m_minX;
    float m_maxX;
    float m_ceilingY;
    float m_groundY;
};