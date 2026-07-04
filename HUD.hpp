#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// ===================== HUD =====================
// Displays lives remaining and the survival countdown timer.
// Phase 7 will add heart icons, styling, and juice (flash/shake on hit).
class HUD {
public:
    HUD(sf::Font& font)
        : m_font(font),
          m_livesText(font, "Lives: 4", 24),
          m_timerText(font, "Time: 120", 24)
    {
        m_livesText.setFillColor(sf::Color::White);
        m_livesText.setPosition(sf::Vector2f(20.f, 20.f));

        m_timerText.setFillColor(sf::Color::White);
        m_timerText.setPosition(sf::Vector2f(650.f, 20.f));
    }

    void update(int lives, float timeRemaining) {
        // TODO (Phase 7): swap text for heart icon sprites
        m_livesText.setString("Lives: " + std::to_string(lives));

        int seconds = static_cast<int>(timeRemaining);
        m_timerText.setString("Time: " + std::to_string(seconds));
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(m_livesText);
        window.draw(m_timerText);
    }

private:
    sf::Font& m_font;
    sf::Text m_livesText;
    sf::Text m_timerText;
};
