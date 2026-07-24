#pragma once
#include "Platform.h"
#include <cmath>
#include <cstdlib>
#include <cstdint>

class CrackedPlatform : public Platform//inheritance from platform class, which inherits from GameObject class
{
public:
    CrackedPlatform(const sf::Texture& texture, sf::Vector2f position)
        : Platform(texture, position)
    {
        basePosition = sprite.getPosition(); // remember its resting spot so shake can offset from it
    }

    bool triggerCrack()
    {
        if (!isCracking && !hasFallen)
        {
            isCracking = true;//starts the countdown to fall
            timer = crackDelay;
            shakeTime = 0.f;
            return true; // this call actually triggered it
        }
        return false; // was already cracking/fallen, nothing new happened
    }

    void update(float dt) override//Override allows derived class to provide its own implementation of a virtual function.
    {
        if (hasDisappeared) return; // nothing left to do once it's gone

        // Count down before falling
        if (isCracking && !hasFallen)
        {
            timer -= dt;
            shakeTime += dt;

            // 0 at the start of the crack, 1 right as it's about to fall
            float progress = 1.f - (timer / crackDelay);
            float intensity = maxShakeOffset * progress; // shake grows the closer it gets to falling

            float offsetX = std::sin(shakeTime * 40.f) * intensity;
            float offsetY = std::cos(shakeTime * 55.f) * intensity * 0.5f; // smaller vertical jitter
            float wobble = std::sin(shakeTime * 33.f) * (maxShakeRotation * progress);

            sprite.setPosition(basePosition + sf::Vector2f(offsetX, offsetY));
            sprite.setRotation(sf::degrees(wobble));

            if (timer <= 0.f)
            {
                hasFallen = true;//platform starts falling

                // snap back to a clean, un-shaken pose right before the real fall starts
                sprite.setPosition(basePosition);

                // pick which way it tips as it drops - randomized so every collapse feels different
                spinDirection = (std::rand() % 2 == 0) ? -1.f : 1.f;
                horizontalDrift = spinDirection * driftSpeed;
                fallVelocity = 0.f; // starts from rest, then accelerates like a real falling object
            }
        }

        // Real physical fall: accelerates under gravity, tips/spins, drifts sideways,
        // fades out, and then disappears for good once it's off-screen.
        if (hasFallen)
        {
            fallVelocity += fallGravity * dt;

            sprite.move({ horizontalDrift * dt, fallVelocity * dt });

            float currentRotation = sprite.getRotation().asDegrees();
            sprite.setRotation(sf::degrees(currentRotation + spinDirection * tumbleSpeed * dt));

            float distanceFallen = sprite.getPosition().y - basePosition.y;

            // Fade out as it drops so the disappearance isn't a jarring pop.
            float fadeProgress = distanceFallen / fallDisappearDistance;
            if (fadeProgress > 1.f) fadeProgress = 1.f;

            sf::Color c = sprite.getColor();
            c.a = static_cast<std::uint8_t>(255.f * (1.f - fadeProgress));
            sprite.setColor(c);

            if (distanceFallen >= fallDisappearDistance)
            {
                hasDisappeared = true; // fully gone - stops drawing from here on
            }
        }
    }

    void draw(sf::RenderWindow& window) override
    {
        if (hasDisappeared) return; // stop rendering once it's fallen off-screen
        GameObject::draw(window);
    }

    bool isSolid() const override
    {
        // A cracked plank is a hazard, not real ground - the player must
        // jump clear of it rather than land on it, so it's never solid.
        return false;
    }
    bool hasStartedFalling() const
    {
        return hasFallen;
    }

private:
    bool isCracking = false;
    bool hasFallen = false;
    bool hasDisappeared = false;

    float timer = 0.f;
    float shakeTime = 0.f;      // drives the shake wave while cracking

    sf::Vector2f basePosition;  // resting position the shake offsets from

    float fallVelocity = 0.f;   // builds up over time once it starts falling (gravity)
    float spinDirection = 1.f;  // -1 or 1, chosen randomly when it starts to fall
    float horizontalDrift = 0.f; // sideways speed while tipping over and falling

    const float crackDelay = 0.5f;    // wait before falling

    const float fallGravity = 1400.f; // how fast the fall accelerates
    const float driftSpeed = 40.f;    // sideways speed while toppling
    const float tumbleSpeed = 220.f;  // degrees/second spin while falling

    const float fallDisappearDistance = 700.f; // how far it falls before it's fully gone

    const float maxShakeOffset = 4.f;    // pixels, at peak intensity right before falling
    const float maxShakeRotation = 3.f;  // degrees, at peak intensity right before falling
};