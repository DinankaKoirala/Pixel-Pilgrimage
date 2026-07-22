// NinjaStar.h
#pragma once
#include "GameObject.h"

class NinjaStar : public GameObject//inheritance
{
public:
    NinjaStar(const sf::Texture& texture, sf::Vector2f position, float rotationSpeedDeg = 250.f)//constructor with default rotation spee
        : GameObject(texture, position)
        , rotationSpeed(sf::degrees(rotationSpeedDeg))
    {
        sf::FloatRect bounds = sprite.getLocalBounds();//gets the size 
        sprite.setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });//star rotates around its center instead of top-left corner
        sprite.setPosition(position);// re-apply since changing origin shifts the visual anchor
        sprite.setScale({ 0.5f, 0.5f });//makes star smaller 
    }

    void update(float dt) override// implement the update function to make the star spin and fly across the screen
    {//it gives the ninja star the movement and rotation behavior
    // Spin the star
        sprite.rotate(rotationSpeed * dt);

        // Fly from right to left
        sprite.move({ -500.f * dt, 0.f });//make ninja stars faster or slower
    }
private:
    sf::Angle rotationSpeed;
};
inline sf::Vector2f ninjaStarStandingPosition(float x, float groundY, float starHeight)
{
    // Center the star so its bottom edge lines up with the bridge surface
    return sf::Vector2f(x, groundY - starHeight / 2.f);
}