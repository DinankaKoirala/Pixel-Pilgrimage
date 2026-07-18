#pragma once
#include <SFML/Graphics.hpp>

class Player
{
public:
    Player(sf::Vector2f position)
    {
        shape.setSize({ 30.f, 50.f });
        shape.setFillColor(sf::Color::Red);
        shape.setPosition(position);
    }

    void update(float dt)
    {
        // Apply gravity
        velocity.y += gravity * dt;

        // Move according to velocity
        shape.move(velocity * dt);
    }

    void moveLeft()
    {
        velocity.x = -moveSpeed;
    }

    void moveRight()
    {
        velocity.x = moveSpeed;
    }

    void stopHorizontal()
    {
        velocity.x = 0.f;
    }

    void jump()
    {
        if (onGround)
        {
            velocity.y = jumpSpeed;
            onGround = false;
        }
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(shape);
    }

    sf::FloatRect getBounds() const
    {
        return shape.getGlobalBounds();
    }

public:
    sf::RectangleShape shape;

    sf::Vector2f velocity{ 0.f, 0.f };

    bool onGround = false;

private:
    const float gravity = 900.f;
    const float moveSpeed = 150.f;
    const float jumpSpeed = -450.f;
};