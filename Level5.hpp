#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <memory>
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
// As of Phase 4: all attack types (orb, aimed fireball, hand spell,
// dripstone) are live and collision-checked against the hero. Win/lose
// flow (Phase 8) and difficulty ramping (Phase 5) are still stubbed.
class Level5 {
public:
    Level5(sf::RenderWindow& window, sf::Font& font)
        : m_window(window),
        m_hero(sf::Vector2f(380.f, 490.f), 550.f, 800.f),   // startPos, groundY, windowWidth
        m_monk(sf::Vector2f(600.f, 470.f)),
        m_attackManager(sf::Vector2f(400.f, 200.f), 800.f, 0.f, 550.f),   // orbSpawnPos, windowWidth, ceilingY, groundY
        m_hud(font),
        m_state(LevelState::Playing),
        m_survivalTime(120.f)   // 2-minute survival timer
    {
        // Ground platform (placeholder - will match cave floor sprite later)
        m_ground.setSize(sf::Vector2f(800.f, 50.f));
        m_ground.setPosition(sf::Vector2f(0.f, 550.f));
        m_ground.setFillColor(sf::Color(60, 50, 40));

        // --- Cave background (Phase 3) ---
        // Tries to load a full-window background image. If it's missing,
        // draw() just skips it and the dark clear-color set in main.cpp
        // shows through instead, so nothing breaks without art assets.
        if (m_backgroundTexture.loadFromFile("assets/cave_background.png")) {
            m_backgroundSprite = std::make_unique<sf::Sprite>(m_backgroundTexture);

            // Scale the image to exactly fill the 800x600 window regardless
            // of the source image's native resolution.
            sf::Vector2u texSize = m_backgroundTexture.getSize();
            m_backgroundSprite->setScale(sf::Vector2f(
                800.f / static_cast<float>(texSize.x),
                600.f / static_cast<float>(texSize.y)
            ));
        }
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

        // --- Collision: hand spells vs hero ---
        for (auto& hs : m_attackManager.getHandSpells()) {
            if (hs.isActive() && heroBounds.findIntersection(hs.getBounds())) {
                m_hero.takeHit();
                hs.deactivate();
            }
        }

        // --- Collision: dripstones vs hero ---
        // Only the Falling state is dangerous - touching the telegraph
        // warning or the landed rubble afterward doesn't hurt the hero.
        for (auto& ds : m_attackManager.getDripstones()) {
            if (ds.isDangerous() && heroBounds.findIntersection(ds.getBounds())) {
                m_hero.takeHit();
                ds.setLanded();   // treat a successful hit like it "landed" so it's cleaned up
            }
        }

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
        if (m_backgroundSprite)
            m_window.draw(*m_backgroundSprite);

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

    sf::Texture m_backgroundTexture;
    std::unique_ptr<sf::Sprite> m_backgroundSprite;   // null if no background image found

    Hero m_hero;
    Monk m_monk;
    AttackManager m_attackManager;
    HUD m_hud;

    LevelState m_state;
    float m_survivalTime;
};