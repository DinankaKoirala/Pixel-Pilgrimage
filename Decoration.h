// Decoration.h
#pragma once
#include "GameObject.h"

// A purely visual background object (cloud, hill, tree, bird...) that
// scrolls slower than the foreground to create a sense of depth (parallax scrolling).
class Decoration : public GameObject
{
public:
    // centerOrigin = false (default): anchors by bottom-center, for objects
    // that sit "on" a ground line (hills, trees, clouds resting on a horizon).
    // centerOrigin = true: anchors by the exact center, for floating sky
    // objects like the sun/moon/birds that aren't resting on anything.
    Decoration(const sf::Texture& texture, sf::Vector2f basePosition, float parallaxFactor, bool centerOrigin = false, float scale = 1.f)
        : GameObject(texture, basePosition)
        , basePos(basePosition)
        , factor(parallaxFactor)
    {
        sf::FloatRect bounds = sprite.getLocalBounds();
        if (centerOrigin)
            sprite.setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });
        else
            sprite.setOrigin({ bounds.size.x / 2.f, bounds.size.y });

        sprite.setScale({ scale, scale });
        sprite.setPosition(basePosition);
    }

    void updateParallax(float cameraX) // When camera moves.. far objects move slower than near objects
    {
        sprite.setPosition({
            basePos.x + cameraX * (1.f - factor) + animOffset.x,
            basePos.y + animOffset.y
            });
        sprite.setRotation(sf::degrees(animRotationDeg));
    }

    // Hook for subclasses that want independent motion (swaying, bobbing,
    // drifting, looping...) on top of the parallax position above.
    // Base Decoration does nothing here, so plain hills/trees/sun/etc. are
    // completely unaffected - only animated subclasses need to write to
    // animOffset / animRotationDeg.
    virtual void animate(float dt) {}

protected:
    sf::Vector2f animOffset{ 0.f, 0.f }; // extra positional offset, added on top of parallax
    float animRotationDeg = 0.f;          // extra rotation, in degrees

private:
    sf::Vector2f basePos;
    float factor;
};