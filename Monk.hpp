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

class Spell
{
public:
    std::unique_ptr<sf::Shape> shape;
    sf::Vector2f velocity;
};

class Monk
{
private:
    sf::CircleShape body;
    std::vector<Spell> spells;

    sf::Vector2f position;

    float spawnTimer = 0.f;
    float spawnInterval =0.7f;
    float spellSpeed = 400.f;

public:

    Monk(sf::Vector2f startPos = { 400.f, 80.f })
        : position(startPos)
    {
        body.setRadius(28.f);
        body.setOrigin({ 28.f, 28.f });
        body.setFillColor(sf::Color(140, 60, 190));
        body.setPosition(position);
    }

    void update(float dt, sf::Vector2f playerPos)
    {
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
                [](const Spell& s)
                {
                    sf::Vector2f p = s.shape->getPosition();
                    return p.x < -60.f || p.x > 860.f ||
                        p.y < -60.f || p.y > 660.f;
                }),
            spells.end());
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(body);

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
    }

private:

    void spawnSpell(sf::Vector2f target)
    {
        int type = rand() % 3;

        Spell s;

        sf::Vector2f dir = target - position;
        float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);

        if (length != 0.f)
            dir /= length;

        s.velocity = dir * spellSpeed;

        if (type == 0)
        {
            auto tri = std::make_unique<sf::ConvexShape>();
            tri->setPointCount(3);
            tri->setPoint(0, { 0.f, -16.f });
            tri->setPoint(1, { 14.f, 14.f });
            tri->setPoint(2, { -14.f, 14.f });
            tri->setFillColor(sf::Color(255, 60, 140));
            tri->setPosition(position);
            s.shape = std::move(tri);
        }
        else if (type == 1)
        {
            auto rect = std::make_unique<sf::RectangleShape>(sf::Vector2f(26.f, 26.f));
            rect->setOrigin({ 13.f, 13.f });
            rect->setFillColor(sf::Color(0, 200, 255));
            rect->setPosition(position);
            s.shape = std::move(rect);
        }
        else
        {
            auto circ = std::make_unique<sf::CircleShape>(13.f);
            circ->setOrigin({ 13.f, 13.f });
            circ->setFillColor(sf::Color(255, 220, 0));
            circ->setPosition(position);
            s.shape = std::move(circ);
        }

        spells.push_back(std::move(s));
    }
};