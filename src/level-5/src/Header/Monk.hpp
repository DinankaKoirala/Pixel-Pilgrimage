// Monk.hpp - Boss enemy that fires homing spells at the player
// The monk floats at a fixed position and periodically casts
// one of three spell types (triangle, rectangle, circle) aimed at the hero.

#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <iostream>

class Spell
{
public:
    std::unique_ptr<sf::Sprite> shape;
    sf::Vector2f velocity;
};

class Monk
{
private:
    sf::Texture monkTexture;
    sf::Texture m_spellTexture;
    sf::Sprite monk;

    std::vector<Spell> spells;

    sf::Vector2f position;

    // The monk's "home" spot. Actual on-screen position each frame is
    // basePosition + a moving offset (see updateMovementOffset), so the
    // monk drifts around this anchor instead of sitting frozen in place.
    sf::Vector2f basePosition;

    float movementTime = 0.f;

    // How far the monk drifts left/right and up/down from basePosition,
    // and how fast each oscillation cycles. Tuned from main.cpp using sx/sy
    // so the motion scales sensibly across screen resolutions, same as the
    // dripstone spawn range.
    float patrolAmplitude;
    float patrolSpeed;
    float hoverAmplitude;
    float hoverSpeed;

    float spawnTimer = 0.f;
    float spawnInterval = 0.7f;
    float spellSpeed;
    float m_spellScale;

    // Off-screen culling box for spells. Must match the ACTUAL window size,
    // not the 800x600 design resolution -- otherwise spells spawned at a
    // scaled-up staff position are already "outside" these bounds and get
    // deleted the same frame they're created (looks like nothing fires at
    // all). main.cpp passes real win_l/win_b (+ a margin) here.
    float cullMinX;
    float cullMaxX;
    float cullMinY;
    float cullMaxY;

public:

    Monk(sf::Vector2f startPos = { 400.f, 80.f },
        float patrolAmplitude_ = 100.f,
        float patrolSpeed_ = 0.6f,
        float hoverAmplitude_ = 15.f,
        float hoverSpeed_ = 1.4f,
        float spellSpeed_ = 600.f,
        float spellScale_ = 0.2,
        float cullMinX_ = -60.f,
        float cullMaxX_ = 860.f,
        float cullMinY_ = -60.f,
        float cullMaxY_ = 660.f)
        : monk(monkTexture),
        position(startPos),
        basePosition(startPos),
        patrolAmplitude(patrolAmplitude_),
        patrolSpeed(patrolSpeed_),
        hoverAmplitude(hoverAmplitude_),
        hoverSpeed(hoverSpeed_),
        spellSpeed(spellSpeed_),
        m_spellScale(spellScale_),
        cullMinX(cullMinX_),
        cullMaxX(cullMaxX_),
        cullMinY(cullMinY_),
        cullMaxY(cullMaxY_)
    {
        if (!monkTexture.loadFromFile("level-5/assets/textures/monk.png"))
        {
            std::cerr << "Monk: failed to load Data/monk.png\n";
        }

        if (!m_spellTexture.loadFromFile("level-5/assets/textures/fireball.png"))
        {
            std::cerr << "Monk: failed to load fireball.png\n";
        }

        monk.setTexture(monkTexture, true); // refresh texture rect now that it's actually loaded
        monk.setOrigin({ monkTexture.getSize().x / 2.f, monkTexture.getSize().y / 2.f });
        monk.setScale({ 0.7f, 0.7f }); // sprite is 480x720 source - scale down to fit the scene
        monk.setPosition(position);
    }

    sf::Vector2f getPosition() const { return position; }

    // Tunable offset from the monk's center to wherever the staff tip is in
    // Data/monk.png, expressed in UNSCALED sprite-space pixels (before the
    // 0.25 scale is applied). (0,0) is the sprite's center, -x is left,
    // -y is up. I can't see the actual art, so this is a rough guess
    // (upper-left, roughly where a raised staff tip would sit) -- nudge it
    // until the aura lines up with the staff in-game. Easiest way to
    // calibrate: temporarily draw a small bright circle at
    // getStaffTipPosition() and adjust staffOffsetLocal until it sits
    // exactly on the staff tip, then remove the debug circle.
    sf::Vector2f staffOffsetLocal{ -150.f, -300.f };

    sf::Vector2f getStaffTipPosition() const
    {
        sf::Vector2f scale = monk.getScale();
        return position + sf::Vector2f(staffOffsetLocal.x * scale.x, staffOffsetLocal.y * scale.y);
    }

    void update(float dt, sf::Vector2f playerPos)
    {
        movementTime += dt;

        // Phase-shifted sine waves so the horizontal patrol and vertical
        // hover don't sync up into one repetitive loop.
        sf::Vector2f offset(
            std::sin(movementTime * patrolSpeed) * patrolAmplitude,
            std::sin(movementTime * hoverSpeed + 1.57f) * hoverAmplitude
        );

        position = basePosition + offset;
        monk.setPosition(position);

        spawnTimer += dt;

        if (spawnTimer >= spawnInterval)
        {
            spawnTimer = 0.f;
            spawnSpell(playerPos);
        }

        for (auto& s : spells)
            s.shape->move(s.velocity * dt);

        spells.erase(
            std::remove_if(
                spells.begin(),
                spells.end(),
                [this](const Spell& s)
                {
                    sf::Vector2f p = s.shape->getPosition();
                    return p.x < cullMinX || p.x > cullMaxX ||
                        p.y < cullMinY || p.y > cullMaxY;
                }),
            spells.end());
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(monk);

        for (auto& s : spells)
            window.draw(*s.shape);
    }

    bool checkCollision(const sf::FloatRect& playerBounds)
    {
        for (auto& s : spells)
        {
            if (playerBounds.findIntersection(s.shape->getGlobalBounds()))
                return true;
        }

        return false;
    }

    void reset()
    {
        spells.clear();
        spawnTimer = 0.f;
        movementTime = 0.f;
        position = basePosition;
        monk.setPosition(position);
    }

private:

    void spawnSpell(sf::Vector2f target)
    {
        Spell s;

        sf::Vector2f spawnPos = getStaffTipPosition();

        sf::Vector2f dir = target - spawnPos;
        float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (length == 0.f)
            return;

        dir /= length;
        s.velocity = dir * spellSpeed;

        auto spellSprite = std::make_unique<sf::Sprite>(m_spellTexture);
        sf::Vector2u texSize = m_spellTexture.getSize();
        spellSprite->setOrigin({ texSize.x / 2.f, texSize.y / 2.f });
        spellSprite->setPosition(spawnPos);
        spellSprite->setScale({ m_spellScale, m_spellScale });

        float angle = std::atan2(dir.y, dir.x) * 180.f / 3.14159265f;
        spellSprite->setRotation(sf::degrees(angle + 90.f));

        s.shape = std::move(spellSprite);
        spells.push_back(std::move(s));
    }
};