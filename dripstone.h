#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <iostream>

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
    // pngScale lets you grow/shrink the dripstone.png overlay independently
    // of the triangle's hitbox: 1.0 = exactly fits the triangle, 1.5 = 50%
    // bigger, 0.5 = half size, etc. The triangle (and collision box) size
    // is unaffected either way.
    Dripstone(sf::Vector2f ceilingPos, float groundY = 550.f, float pngScale = 1.f)
        : m_shape(),
        m_warningShape(),
        m_sprite(getTexture()),
        m_state(DripstoneState::Telegraphing),
        m_telegraphTimer(0.5f),   // half-second warning before falling - keep readable but fast
        m_telegraphDuration(0.5f),
        m_fallSpeed(0.f),
        m_gravity(1500.f),
        m_groundY(groundY),
        m_size(20.f, 40.f),
        m_pngScale(pngScale)
    {
        // Build a downward-pointing triangle (like a stalactite hanging
        // from the ceiling): flat edge on top, point at the bottom.
        m_shape.setPointCount(3);
        m_shape.setPoint(0, sf::Vector2f(0.f, 0.f));                     // top-left
        m_shape.setPoint(1, sf::Vector2f(m_size.x, 0.f));                // top-right
        m_shape.setPoint(2, sf::Vector2f(m_size.x / 2.f, m_size.y));     // bottom point
        m_shape.setFillColor(sf::Color(150, 150, 150));
        m_shape.setPosition(ceilingPos);

        // The warning indicator mirrors the same triangle shape but drawn
        // semi-transparent so players can see exactly where it's about to
        // fall before it actually drops.
        m_warningShape.setPointCount(3);
        m_warningShape.setPoint(0, sf::Vector2f(0.f, 0.f));
        m_warningShape.setPoint(1, sf::Vector2f(m_size.x, 0.f));
        m_warningShape.setPoint(2, sf::Vector2f(m_size.x / 2.f, m_size.y));
        m_warningShape.setFillColor(sf::Color(255, 60, 60, 90));
        m_warningShape.setPosition(ceilingPos);

        // Scale the dripstone.png sprite so it fits over the triangle's
        // bounding box (times pngScale), then center it on the triangle so
        // that adjusting pngScale grows/shrinks it around the same spot
        // instead of shifting it off the shape.
        sf::Vector2u texSize = getTexture().getSize();
        if (texSize.x > 0 && texSize.y > 0) {
            m_sprite.setOrigin(sf::Vector2f(texSize.x / 2.f, texSize.y / 2.f));
            m_sprite.setScale(sf::Vector2f(
                (m_size.x / static_cast<float>(texSize.x)) * m_pngScale,
                (m_size.y / static_cast<float>(texSize.y)) * m_pngScale));
        }
        m_sprite.setPosition(ceilingPos + sf::Vector2f(m_size.x / 2.f, m_size.y / 2.f));
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

            // Land once the bottom point reaches the cave floor.
            sf::Vector2f pos = m_shape.getPosition();
            float height = m_size.y;
            if (pos.y + height >= m_groundY) {
                m_shape.setPosition(sf::Vector2f(pos.x, m_groundY - height));
                m_state = DripstoneState::Landed;
            }
        }

        // Keep the png overlay glued to the triangle's center every frame.
        m_sprite.setPosition(m_shape.getPosition() + sf::Vector2f(m_size.x / 2.f, m_size.y / 2.f));
    }

    void draw(sf::RenderWindow& window) const {
        if (m_state == DripstoneState::Telegraphing) {
            // Flicker the warning sliver so it reads as "about to fall"
            // rather than just a static shape (cheap but effective juice).
            window.draw(m_warningShape);
        }
        else {
            window.draw(m_shape);
            window.draw(m_sprite); // dripstone.png rendered on top of the triangle
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
    // Loaded once and shared across every Dripstone instance.
    static sf::Texture& getTexture() {
        static sf::Texture texture;
        static bool loaded = false;
        if (!loaded) {
            loaded = texture.loadFromFile("Data/dripstone.png");
            if (!loaded) {
                std::cerr << "Failed to load dripstone.png\n";
            }
        }
        return texture;
    }

    sf::ConvexShape m_shape;
    sf::ConvexShape m_warningShape;
    sf::Sprite m_sprite;
    DripstoneState m_state;
    float m_telegraphTimer;
    float m_telegraphDuration;
    float m_fallSpeed;
    float m_gravity;
    float m_groundY;
    sf::Vector2f m_size;
    float m_pngScale;
};

// ===================== DRIPSTONE MANAGER =====================
// Owns the whole set of active dripstones: spawns new ones on a timer,
// updates/draws them each frame, and answers collision queries.
// Landed dripstones are kept around briefly (as visible rubble) before
// being cleaned up automatically.
class DripstoneManager {
public:
    // pngScale grows/shrinks the dripstone.png overlay on every spawned
    // dripstone (1.0 = default fit, >1 bigger, <1 smaller).
    DripstoneManager(float spawnInterval = 1.5f,
        float minX = 50.f,
        float maxX = 750.f,
        float ceilingY = 0.f,
        float groundY = 550.f,
        float pngScale = 10.f)
        : m_spawnTimer(0.f),
        m_spawnInterval(spawnInterval),
        m_minX(minX),
        m_maxX(maxX),
        m_ceilingY(ceilingY),
        m_groundY(groundY),
        m_pngScale(pngScale)
    {
    }

    void update(float dt) {
        m_spawnTimer += dt;
        if (m_spawnTimer >= m_spawnInterval) {
            m_spawnTimer = 0.f;

            float span = m_maxX - m_minX;
            float x = m_minX + static_cast<float>(rand() % static_cast<int>(span > 0.f ? span : 1.f));

            m_entries.push_back({ Dripstone(sf::Vector2f(x, m_ceilingY), m_groundY, m_pngScale), 0.f });
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
    float m_pngScale;
};