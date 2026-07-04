#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include "Hero.hpp"
#include "Monk.hpp"
#include "AttackManager.hpp"
#include "HUD.hpp"

// ===================== LEVEL 5 STATE =====================
enum class LevelState {
    Playing,
    Won,    // survived 120 seconds
    Lost    // hero ran out of lives
};

// ===================== LEVEL 5 =====================
// Ties together Hero, Monk, AttackManager, and HUD into one playable level.
// Win/lose flow (Phase 8), collision resolution (Phase 6), and difficulty
// ramping (Phase 5) are stubbed for now - this class just wires everything
// together so each system can be built out independently.
class Level5 {
public:
    Level5(sf::RenderWindow& window, sf::Font& font)
        : m_window(window),
        m_hero(sf::Vector2f(380.f, 490.f), 550.f, 800.f),   // startPos, groundY, windowWidth
        m_monk(sf::Vector2f(600.f, 470.f)),
        m_attackManager(sf::Vector2f(400.f, 200.f)),   // orb spawns mid-screen
        m_hud(font),
        m_state(LevelState::Playing),
        m_survivalTime(120.f)   // 2-minute survival timer
    {
        // Ground platform (placeholder - will match cave floor sprite later)
        m_ground.setSize(sf::Vector2f(800.f, 50.f));
        m_ground.setPosition(sf::Vector2f(0.f, 550.f));
        m_ground.setFillColor(sf::Color(60, 50, 40));
    }

    void handleEvent(const sf::Event& event) {
        // TODO: pause menu, debug toggles, etc.
    }

    void update(float dt) {
        if (m_state != LevelState::Playing) {
            // TODO (Phase 8): handle retry/continue input on win/lose screens
            return;
        }

        m_hero.handleInput();
        m_hero.update(dt);

        m_monk.update(dt);
        m_attackManager.update(dt, m_monk, m_hero.getBounds().position);

        // --- Collision: fireballs vs hero ---
        // Deactivating the fireball on hit stops it from damaging the hero
        // again on the next frame (AttackManager's cleanup step removes
        // deactivated fireballs right after). takeHit() itself no-ops if the
        // hero is currently invincible (post-hit i-frames or mid-dash).
        sf::FloatRect heroBounds = m_hero.getBounds();
        for (auto& fb : m_attackManager.getFireballs()) {
            if (fb.isActive() && heroBounds.findIntersection(fb.getBounds())) {
                m_hero.takeHit();
                fb.deactivate();
            }
        }

        // TODO (Phase 6): same collision pattern for dripstones and hand spells

        m_survivalTime -= dt;
        m_hud.update(m_hero.getLives(), m_survivalTime);

        // --- Win/Lose checks ---
        if (m_hero.isDead()) {
            m_state = LevelState::Lost;
        }
        else if (m_survivalTime <= 0.f) {
            m_state = LevelState::Won;
        }
    }

    void draw() {
        m_window.draw(m_ground);
        m_monk.draw(m_window);
        m_attackManager.draw(m_window);
        m_hero.draw(m_window);
        m_hud.draw(m_window);

        // TODO (Phase 8): draw win/lose overlay screens based on m_state
    }

    void reset() {
        m_hero.reset(sf::Vector2f(380.f, 490.f));
        m_attackManager.reset();
        m_survivalTime = 120.f;
        m_state = LevelState::Playing;
    }

    LevelState getState() const { return m_state; }

private:
    sf::RenderWindow& m_window;
    sf::RectangleShape m_ground;

    Hero m_hero;
    Monk m_monk;
    AttackManager m_attackManager;
    HUD m_hud;

    LevelState m_state;
    float m_survivalTime;
};