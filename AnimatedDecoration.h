// AnimatedDecoration.h
#pragma once
#include "Decoration.h"
#include <cmath>
#include <cstdlib>

class SwayingTree : public Decoration //inheritance
{
public:
    SwayingTree(const sf::Texture& texture, sf::Vector2f basePosition, float parallaxFactor, float scale = 1.f)
        : Decoration(texture, basePosition, parallaxFactor, false, scale)// constructor parent constructor call because
        , phase(static_cast<float>(std::rand() % 1000) / 1000.f * 6.283f)// it contains all parallox scrolling stuffs 
    {
    }

	void animate(float dt) override// polymorphism, override the base class function
    {
        elapsed += dt;
        animRotationDeg = std::sin(elapsed * swaySpeed + phase) * swayAmount;//sin creates a wave trees appera
    }                                                                        //sway in the wind 

private:
    float elapsed = 0.f;//tracks total animation time 
    float phase;//stores random starting position 
    const float swaySpeed = 0.8f;   // radians/second - slow and lazy//higher the number faster the movement 
    const float swayAmount = 2.5f;  // degrees - subtle, not cartoonish//controls angle 
};

// Vertical bob + slight tilt while it scrolls, to fake a wingbeat rhythm
// without needing real frame-by-frame sprite animation.
class FlappingBird : public Decoration
{
public:
    FlappingBird(const sf::Texture& texture, sf::Vector2f basePosition, float parallaxFactor, float scale = 1.f)
		: Decoration(texture, basePosition, parallaxFactor, true, scale) // trees uses false but birds uses true 
        , phase(static_cast<float>(std::rand() % 1000) / 1000.f * 6.283f)
    {   //birds rotate around the center why trees rotate around their base
    }

    void animate(float dt) override
    {
        elapsed += dt;
        animOffset.y = std::sin(elapsed * flapSpeed + phase) * bobAmount;//creates up down movement flap 
		animRotationDeg = std::sin(elapsed * flapSpeed + phase) * tiltAmount;//birds tilt while flapping wings
    }

private:
    float elapsed = 0.f;
    float phase;
    const float flapSpeed = 4.f;   // faster than the tree sway - a wingbeat rhythm
	const float bobAmount = 6.f;   // pixels moves 6 pixels up and down
    const float tiltAmount = 8.f;  // degrees rotates 8 degrees 
};
class DriftingCloud : public Decoration
{
public:
    DriftingCloud(const sf::Texture& texture, sf::Vector2f basePosition, float parallaxFactor, float scale = 1.f)
        : Decoration(texture, basePosition, parallaxFactor, false, scale)
        , phase(static_cast<float>(std::rand() % 1000) / 1000.f * 6.283f)
        , driftSpeed((std::rand() % 2 == 0 ? 1.f : -1.f)* (8.f + static_cast<float>(std::rand() % 6)))
    {  //drift speed gives random directions
    }

    void animate(float dt) override
    {
        elapsed += dt;
		driftAccum += driftSpeed * dt;// horixontal movement of clouds
        animOffset.x = driftAccum;//cloud shifts horizontally 
        animOffset.y = std::sin(elapsed * bobSpeed + phase) * bobAmount;//bobbing movement up down floating movement 
    }

private:
    float elapsed = 0.f;//stores cloud movemnt (total)
    float driftAccum = 0.f;
    float phase;
    float driftSpeed;       // pixels/second, randomized direction and magnitude per cloud
    const float bobSpeed = 0.4f;//very slow movement 
    const float bobAmount = 4.f; // moves by 4 pixels
};