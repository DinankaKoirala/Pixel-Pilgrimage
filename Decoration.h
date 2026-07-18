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
        sprite.setPosition({ basePos.x + cameraX * (1.f - factor), basePos.y });
    }

private:
    sf::Vector2f basePos;
    float factor;
};