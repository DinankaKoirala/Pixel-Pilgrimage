#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include "Monk.hpp"
#include "Attacks/Fireball.hpp"
#include "Attacks/Dripstone.hpp"
#include "Attacks/HandSpell.hpp"
#include "Attacks/Orb.hpp"

// ===================== ATTACK MANAGER =====================
// The "brain" of the boss fight. Phase 4 wires up every attack type so
// they actually trigger and can be tested individually or together:
//   - Orb: continuous radial fireballs from mid-screen (done in Phase "merge")
//   - Aimed fireball throw: monk throws a fireball straight at the hero
//   - Hand spell: monk casts a bolt straight at the hero's position
//   - Dripstone: random ceiling point telegraphs, then falls
//
// Phase 5 will replace these independent fixed-interval timers with a
// smarter weighted/scripted pattern picker and a difficulty ramp - for
// now, each attack fires on its own clock so every system can be seen
// and tested working at once (matches the "fast paced, overlapping
// attacks" goal from the original plan).
class AttackManager {
public:
    AttackManager(sf::Vector2f orbSpawnPos, float windowWidth = 800.f, float ceilingY = 0.f, float groundY = 550.f)
        : m_orb(orbSpawnPos),
        m_windowWidth(windowWidth),
        m_ceilingY(ceilingY),
        m_groundY(groundY),
        m_fireballThrowTimer(1.5f), m_fireballThrowInterval(2.5f),
        m_handSpellTimer(2.f), m_handSpellInterval(3.0f),
        m_dripstoneTimer(1.f), m_dripstoneInterval(2.0f)
    {
        // Orb is always-on from the start - continuous background pressure
        // while the monk's other attacks layer on top of it.
        m_orb.setSpawnInterval(0.1f);
        m_orb.setFireballSpeed(250.f);
        m_orb.activate();
    }

    void update(float dt, Monk& monk, sf::Vector2f heroPos) {
        // --- Orb (continuous) ---
        m_orb.update(dt, m_fireballs);

        // --- Aimed fireball throw (on its own timer) ---
        m_fireballThrowTimer += dt;
        if (m_fireballThrowTimer >= m_fireballThrowInterval) {
            m_fireballThrowTimer = 0.f;
            monk.playCastAnimation();
            spawnAimedFireball(monk.getHandPosition(), heroPos);
        }

        // --- Hand spell (on its own timer) ---
        m_handSpellTimer += dt;
        if (m_handSpellTimer >= m_handSpellInterval) {
            m_handSpellTimer = 0.f;
            monk.playCastAnimation();
            spawnHandSpell(monk.getHandPosition(), heroPos);
        }

        // --- Dripstone (on its own timer) ---
        m_dripstoneTimer += dt;
        if (m_dripstoneTimer >= m_dripstoneInterval) {
            m_dripstoneTimer = 0.f;
            spawnDripstone(heroPos);
        }

        // --- Update all active hazards ---
        for (auto& fb : m_fireballs)  fb.update(dt);
        for (auto& ds : m_dripstones) ds.update(dt);
        for (auto& hs : m_handSpells) hs.update(dt);

        // --- Clean up spent/off-screen hazards ---
        m_fireballs.erase(
            std::remove_if(m_fireballs.begin(), m_fireballs.end(),
                [](const Fireball& fb) {
                    if (!fb.isActive()) return true;
                    sf::Vector2f p = fb.getBounds().position;
                    return p.x < -50.f || p.x > 850.f || p.y < -50.f || p.y > 650.f;
                }),
            m_fireballs.end()
        );

        m_handSpells.erase(
            std::remove_if(m_handSpells.begin(), m_handSpells.end(),
                [](const HandSpell& hs) {
                    if (!hs.isActive()) return true;
                    sf::Vector2f p = hs.getBounds().position;
                    return p.x < -50.f || p.x > 850.f || p.y < -50.f || p.y > 650.f;
                }),
            m_handSpells.end()
        );

        m_dripstones.erase(
            std::remove_if(m_dripstones.begin(), m_dripstones.end(),
                [](const Dripstone& ds) { return ds.getState() == DripstoneState::Landed; }),
            m_dripstones.end()
        );
    }

    void draw(sf::RenderWindow& window) const {
        m_orb.draw(window);
        for (auto& fb : m_fireballs)  fb.draw(window);
        for (auto& ds : m_dripstones) ds.draw(window);
        for (auto& hs : m_handSpells) hs.draw(window);
    }

    // Collision system (Level5::update) checks the hero against all of these
    std::vector<Fireball>& getFireballs() { return m_fireballs; }
    std::vector<Dripstone>& getDripstones() { return m_dripstones; }
    std::vector<HandSpell>& getHandSpells() { return m_handSpells; }

    void reset() {
        m_fireballs.clear();
        m_dripstones.clear();
        m_handSpells.clear();
        m_fireballThrowTimer = 0.f;
        m_handSpellTimer = 0.f;
        m_dripstoneTimer = 0.f;
    }

private:
    void spawnAimedFireball(sf::Vector2f from, sf::Vector2f heroPos) {
        sf::Vector2f dir = normalizedDirection(from, heroPos);
        m_fireballs.emplace_back(from, dir * m_fireballThrowSpeed);
    }

    void spawnHandSpell(sf::Vector2f from, sf::Vector2f heroPos) {
        sf::Vector2f dir = normalizedDirection(from, heroPos);
        m_handSpells.emplace_back(from, dir * m_handSpellSpeed);
    }

    void spawnDripstone(sf::Vector2f heroPos) {
        // Bias the spawn x toward the hero's current position (within a
        // random spread) so it's a real threat, not just background noise,
        // while still being dodgeable since it telegraphs first.
        float spread = 150.f;
        float offset = static_cast<float>(rand() % static_cast<int>(spread * 2)) - spread;
        float x = heroPos.x + offset;

        if (x < 0.f) x = 0.f;
        if (x > m_windowWidth - 20.f) x = m_windowWidth - 20.f; // 20 = dripstone width

        m_dripstones.emplace_back(sf::Vector2f(x, m_ceilingY), m_groundY);
    }

    sf::Vector2f normalizedDirection(sf::Vector2f from, sf::Vector2f to) const {
        sf::Vector2f delta = to - from;
        float length = std::sqrt(delta.x * delta.x + delta.y * delta.y);
        if (length == 0.f) return sf::Vector2f(0.f, 1.f); // fallback: straight down
        return sf::Vector2f(delta.x / length, delta.y / length);
    }

    Orb m_orb;
    std::vector<Fireball>  m_fireballs;
    std::vector<Dripstone> m_dripstones;
    std::vector<HandSpell> m_handSpells;

    float m_windowWidth;
    float m_ceilingY;
    float m_groundY;

    float m_fireballThrowTimer;
    float m_fireballThrowInterval;
    float m_fireballThrowSpeed = 300.f;

    float m_handSpellTimer;
    float m_handSpellInterval;
    float m_handSpellSpeed = 400.f;   // faster than fireballs - "shoots spells" reads as snappier

    float m_dripstoneTimer;
    float m_dripstoneInterval;
};