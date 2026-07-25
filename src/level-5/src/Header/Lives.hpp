#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Heart
{
public:
    Heart(sf::Vector2f origin)
    {
        float r = 6.f;
        m_leftCircle.setRadius(r);
        m_leftCircle.setOrigin({ r, r });
        m_leftCircle.setPosition({ origin.x - 5.f, origin.y - 4.f });

        m_rightCircle.setRadius(r);
        m_rightCircle.setOrigin({ r, r });
        m_rightCircle.setPosition({ origin.x + 5.f, origin.y - 4.f });

        m_bottom.setPointCount(3);
        m_bottom.setPoint(0, { origin.x - 8.f, origin.y + 2.f });
        m_bottom.setPoint(1, { origin.x + 8.f, origin.y + 2.f });
        m_bottom.setPoint(2, { origin.x, origin.y + 14.f });

        setAlive(true);
    }

    void setAlive(bool alive)
    {
        sf::Color c = alive ? sf::Color(220, 40, 40) : sf::Color(60, 60, 60);
        m_leftCircle.setFillColor(c);
        m_rightCircle.setFillColor(c);
        m_bottom.setFillColor(c);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(m_leftCircle, states);
        target.draw(m_rightCircle, states);
        target.draw(m_bottom, states);
    }

private:
    sf::CircleShape m_leftCircle;
    sf::CircleShape m_rightCircle;
    sf::ConvexShape m_bottom;
};

class Lives : public sf::Drawable
{
public:
    Lives(sf::Vector2f origin, int maxLives = 4)
        : m_maxLives(maxLives)
        , m_remaining(maxLives)
    {
        for (int i = 0; i < maxLives; ++i)
        {
            m_hearts.emplace_back(sf::Vector2f{ origin.x + i * 24.f, origin.y });
        }
    }

    bool takeDamage()
    {
        if (m_invincibilityTimer > 0.f || m_remaining <= 0)
            return false;

        --m_remaining;
        m_hearts[m_remaining].setAlive(false);
        m_invincibilityTimer = 1.5f;

        return true;
    }

    void update(float dt)
    {
        if (m_invincibilityTimer > 0.f)
            m_invincibilityTimer -= dt;
    }

    void reset()
    {
        m_remaining = m_maxLives;
        m_invincibilityTimer = 0.f;
        for (int i = 0; i < m_maxLives; ++i)
            m_hearts[i].setAlive(true);
    }

    bool isDead() const { return m_remaining <= 0; }

    int getRemaining() const { return m_remaining; }

private:
    int m_maxLives;
    int m_remaining;
    float m_invincibilityTimer = 0.f;
    std::vector<Heart> m_hearts;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        for (const auto& h : m_hearts)
            h.draw(target, states);
    }
};
