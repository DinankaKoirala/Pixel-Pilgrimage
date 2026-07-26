// NinjaStar.h
#pragma once
#include "GameObject.h"

namespace L2 {

class NinjaStar : public GameObject//inheritance
{
public:
    NinjaStar(const sf::Texture& texture, sf::Vector2f position, float rotationSpeedDeg = 250.f)//constructor with default rotation speed
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

    // getBounds() (inherited) returns the sprite's ROTATED axis-aligned
    // bounding box, which grows bigger than the actual star art while it's
    // spinning (a rotated square's AABB is wider than the square itself).
    // That made the star "kill" the player before it visually got close -
    // no fair chance to jump over it. Use this instead for the death
    // check: a small, fixed-size box centered on the star that doesn't
    // change as it rotates, so the hitbox matches what the player sees.
    sf::FloatRect getHitbox() const
    {
        sf::Vector2f center = sprite.getPosition(); // origin is centered, see constructor
        return sf::FloatRect(
            { center.x - hitboxHalfSize, center.y - hitboxHalfSize },
            { hitboxHalfSize * 2.f, hitboxHalfSize * 2.f });
    }
private:
    sf::Angle rotationSpeed;
    const float hitboxHalfSize = 14.f; // pixels from center - smaller than the sprite's rotated AABB, tune to taste
};

inline sf::Vector2f ninjaStarStandingPosition(float x, float groundY, float starHeight)
{
    // Center the star so its bottom edge lines up with the bridge surface
    return sf::Vector2f(x, groundY - starHeight / 2.f);
}

} // namespace L2