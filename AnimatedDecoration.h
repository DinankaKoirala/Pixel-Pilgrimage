// AnimatedDecoration.h
#pragma once
#include "Decoration.h"
#include <cmath>
#include <cstdlib>

// Small decorations that layer their own motion on top of Decoration's
// parallax positioning. Each overrides animate(dt) only - updateParallax()
// is inherited as-is and applies whatever animOffset/animRotationDeg
// animate() wrote.
//
// Every instance randomizes its starting phase in the constructor so a row
// of trees, birds, etc. don't all move in perfect lockstep.

// Gentle side-to-side rotation, pivoting from the base (bottom-center
// origin), like wind blowing through a tree.
class SwayingTree : public Decoration
{
public:
    SwayingTree(const sf::Texture& texture, sf::Vector2f basePosition, float parallaxFactor, float scale = 1.f)
        : Decoration(texture, basePosition, parallaxFactor, false, scale)
        , phase(static_cast<float>(std::rand() % 1000) / 1000.f * 6.283f)
    {
    }

    void animate(float dt) override
    {
        elapsed += dt;
        animRotationDeg = std::sin(elapsed * swaySpeed + phase) * swayAmount;
    }

private:
    float elapsed = 0.f;
    float phase;
    const float swaySpeed = 0.8f;   // radians/second - slow and lazy
    const float swayAmount = 2.5f;  // degrees - subtle, not cartoonish
};

// Vertical bob + slight tilt while it scrolls, to fake a wingbeat rhythm
// without needing real frame-by-frame sprite animation.
class FlappingBird : public Decoration
{
public:
    FlappingBird(const sf::Texture& texture, sf::Vector2f basePosition, float parallaxFactor, float scale = 1.f)
        : Decoration(texture, basePosition, parallaxFactor, true, scale) // centerOrigin - floats freely in the sky
        , phase(static_cast<float>(std::rand() % 1000) / 1000.f * 6.283f)
    {
    }

    void animate(float dt) override
    {
        elapsed += dt;
        animOffset.y = std::sin(elapsed * flapSpeed + phase) * bobAmount;
        animRotationDeg = std::sin(elapsed * flapSpeed + phase) * tiltAmount;
    }

private:
    float elapsed = 0.f;
    float phase;
    const float flapSpeed = 4.f;   // faster than the tree sway - a wingbeat rhythm
    const float bobAmount = 6.f;   // pixels
    const float tiltAmount = 8.f;  // degrees
};

// Slow, independent horizontal drift plus a gentle vertical bob, so the sky
// feels alive even when the camera isn't moving. Direction/speed randomized
// per cloud so they don't all drift the same way.
class DriftingCloud : public Decoration
{
public:
    DriftingCloud(const sf::Texture& texture, sf::Vector2f basePosition, float parallaxFactor, float scale = 1.f)
        : Decoration(texture, basePosition, parallaxFactor, false, scale)
        , phase(static_cast<float>(std::rand() % 1000) / 1000.f * 6.283f)
        , driftSpeed((std::rand() % 2 == 0 ? 1.f : -1.f)* (8.f + static_cast<float>(std::rand() % 6)))
    {
    }

    void animate(float dt) override
    {
        elapsed += dt;
        driftAccum += driftSpeed * dt;
        animOffset.x = driftAccum;
        animOffset.y = std::sin(elapsed * bobSpeed + phase) * bobAmount;
    }

private:
    float elapsed = 0.f;
    float driftAccum = 0.f;
    float phase;
    float driftSpeed;       // pixels/second, randomized direction and magnitude per cloud
    const float bobSpeed = 0.4f;
    const float bobAmount = 4.f; // pixels
};