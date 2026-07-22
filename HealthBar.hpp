#pragma once
#include <SFML/Graphics.hpp>

class HealthBar : public sf::Drawable
{
public:
    HealthBar(float maxHealth, float drainDuration, sf::Vector2f position, sf::Vector2f size, const sf::Font& font)
        : m_maxHealth(maxHealth)
        , m_currentHealth(maxHealth)
        , m_drainRate(maxHealth / drainDuration)
        , m_label(font)
    {
        m_background.setSize(size);
        m_background.setPosition(position);
        m_background.setFillColor(sf::Color(30, 30, 30));
        m_background.setOutlineColor(sf::Color(100, 100, 100));
        m_background.setOutlineThickness(2.f);

        m_foreground.setSize(size);
        m_foreground.setPosition(position);
        m_foreground.setFillColor(sf::Color(200, 30, 30));

        m_label.setString("ELDER LICH OF FALLEN HEAVENS");
        m_label.setCharacterSize(18);
        m_label.setFillColor(sf::Color(220, 220, 220));
        sf::FloatRect lb = m_label.getLocalBounds();
        m_label.setOrigin({ lb.size.x / 2.f, lb.size.y });
        m_label.setPosition({ position.x + size.x / 2.f, position.y - 5.f });
    }

    void update(float dt)
    {
        if (m_currentHealth > 0.f)
        {
            m_currentHealth -= m_drainRate * dt;
            if (m_currentHealth < 0.f)
                m_currentHealth = 0.f;

            float ratio = m_currentHealth / m_maxHealth;
            m_foreground.setSize({ m_background.getSize().x * ratio, m_background.getSize().y });
        }
    }

    void reset()
    {
        m_currentHealth = m_maxHealth;
        m_foreground.setSize(m_background.getSize());
    }

    bool isEmpty() const { return m_currentHealth <= 0.f; }

private:
    float m_maxHealth;
    float m_currentHealth;
    float m_drainRate;
    sf::RectangleShape m_background;
    sf::RectangleShape m_foreground;
    sf::Text m_label;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        target.draw(m_label, states);
        target.draw(m_background, states);
        target.draw(m_foreground, states);
    }
};
