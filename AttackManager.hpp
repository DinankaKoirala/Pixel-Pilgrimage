#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include "Monk.hpp"
#include "Attacks/Fireball.hpp"
#include "Attacks/Dripstone.hpp"
#include "Attacks/HandSpell.hpp"
#include "Attacks/Orb.hpp"

// ===================== ATTACK MANAGER =====================
// The "brain" of the boss fight. Decides which attacks to trigger and when.
// Phase 5 will fill in the actual pattern/timing/difficulty-ramp logic.
// For now this just holds containers for every active hazard type
// and stubs out the update loop.
class AttackManager {
public:
    AttackManager(sf::Vector2f orbSpawnPos)
        : m_orb(orbSpawnPos), m_decisionTimer(0.f), m_decisionInterval(3.f)
    {
        // Orb is the one fully wired-up attack so far (merged in from the
        // tested standalone demo) - activate it immediately with the tuned
        // fast-paced values instead of waiting on Phase 5's attack picker.
        m_orb.setSpawnInterval(0.1f);   // fast, continuous fireball emission
        m_orb.setFireballSpeed(250.f);
        m_orb.activate();
    }

    void update(float dt, const Monk& monk, sf::Vector2f heroPos) {
        // TODO (Phase 5): weighted-random or scripted attack pattern selection
        // TODO (Phase 5): ramp up difficulty as the level timer progresses
        m_decisionTimer += dt;
        if (m_decisionTimer >= m_decisionInterval) {
            m_decisionTimer = 0.f;
            // placeholder: decide which attack to trigger next
            // (dripstone/hand-spell triggers will slot in here in Phase 5)
        }

        // Update all active hazards
        m_orb.update(dt, m_fireballs);

        for (auto& fb : m_fireballs) fb.update(dt);
        for (auto& ds : m_dripstones) ds.update(dt);
        for (auto& hs : m_handSpells) hs.update(dt);

        // --- Clean up fireballs that are either off-screen or have already
        // hit the hero (deactivated by the collision check in Level5::update) ---
        m_fireballs.erase(
            std::remove_if(m_fireballs.begin(), m_fireballs.end(),
                [](const Fireball& fb) {
                    if (!fb.isActive()) return true;   // already used up (hit the hero)

                    sf::Vector2f p = fb.getBounds().position;
                    return p.x < -50.f || p.x > 850.f || p.y < -50.f || p.y > 650.f;
                }),
            m_fireballs.end()
        );

        // TODO (Phase 6): same cleanup pattern for dripstones (Landed state) and hand spells
    }

    void draw(sf::RenderWindow& window) const {
        m_orb.draw(window);
        for (auto& fb : m_fireballs) fb.draw(window);
        for (auto& ds : m_dripstones) ds.draw(window);
        for (auto& hs : m_handSpells) hs.draw(window);
    }

    // Collision system (Phase 6) will call these to check hero against all hazards
    std::vector<Fireball>& getFireballs() { return m_fireballs; }
    std::vector<Dripstone>& getDripstones() { return m_dripstones; }
    std::vector<HandSpell>& getHandSpells() { return m_handSpells; }

    void reset() {
        m_fireballs.clear();
        m_dripstones.clear();
        m_handSpells.clear();
        m_orb.deactivate();
        m_decisionTimer = 0.f;
    }

private:
    Orb m_orb;
    std::vector<Fireball>  m_fireballs;
    std::vector<Dripstone> m_dripstones;
    std::vector<HandSpell> m_handSpells;

    float m_decisionTimer;
    float m_decisionInterval;   // how often the manager "decides" on a new attack
};