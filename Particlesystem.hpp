#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <cstdint>
#include <cmath>
#include "VertexUtils.hpp"

// ---------------------------------------------------------------------------
// ParticleSystem.hpp
//
// Lightweight, header-only particle emitter for ambient effects like rising
// embers from torches, or slow-drifting mist/dust. Designed to be cheap
// enough to run several instances at once (e.g. one per torch).
//
// Usage example:
//
//   #include "ParticleSystem.hpp"
//
//   ParticleSystem embers(ParticleSystem::Preset::Ember, {120.f, 650.f});
//   ParticleSystem embers2(ParticleSystem::Preset::Ember, {1400.f, 650.f});
//   ParticleSystem mist(ParticleSystem::Preset::Mist, {760.f, 100.f});
//
//   // in your game loop:
//   float dt = clock.restart().asSeconds();
//   embers.update(dt);
//   embers2.update(dt);
//   mist.update(dt);
//
//   window.draw(embers);
//   window.draw(embers2);
//   window.draw(mist);
//
// ---------------------------------------------------------------------------

class ParticleSystem : public sf::Drawable {
public:
    enum class Preset { Ember, Mist, FireTrail };

    ParticleSystem(Preset preset, sf::Vector2f origin, std::size_t maxParticles = 60)
        : m_preset(preset), m_origin(origin)
    {
        // Mist uses fewer, larger soft blobs -- too many overlapping mist
        // particles look noisy rather than foggy.
        if (preset == Preset::Mist && maxParticles > 60)
            maxParticles = 18;
        else if (preset == Preset::Mist)
            maxParticles = std::min(maxParticles, static_cast<std::size_t>(18));

        m_particles.resize(maxParticles);
        m_vertsPerParticle = (preset == Preset::Mist || preset == Preset::FireTrail)
            ? softCircleVertexCount(m_mistSegments)
            : 6; // quad = 2 triangles

        m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
        m_vertices.resize(maxParticles * m_vertsPerParticle);

        for (auto& p : m_particles) {
            respawn(p, true /* stagger initial life so they don't all pop at once */);
        }
    }

    // Call once per frame with delta time in seconds.
    void update(float dt) {
        m_time += dt;

        for (std::size_t i = 0; i < m_particles.size(); ++i) {
            Particle& p = m_particles[i];
            p.life -= dt;

            if (p.life <= 0.f) {
                respawn(p, false);
            }

            // Integrate motion
            p.pos += p.velocity * dt;

            // Gentle horizontal sway so motion doesn't look linear/robotic
            float sway = std::sin(m_time * p.swaySpeed + p.swayOffset) * p.swayAmount;
            sf::Vector2f drawPos = p.pos;
            drawPos.x += sway;

            float lifeRatio = p.life / p.maxLife; // 1 -> 0 over lifetime
            float alpha = fadeCurve(lifeRatio) * 255.f;

            float size;
            if (m_preset == Preset::Mist) {
                size = p.size * (0.6f + 0.4f * (1.f - lifeRatio));
            }
            else if (m_preset == Preset::FireTrail) {
                // Shrinks as it burns out, unlike mist which expands.
                size = p.size * (0.3f + 0.7f * lifeRatio);
            }
            else {
                size = p.size;
            }

            sf::Color color = p.color;
            color.a = static_cast<std::uint8_t>(alpha);

            if (m_preset == Preset::Mist || m_preset == Preset::FireTrail) {
                sf::Color edge = color;
                edge.a = 0;
                appendSoftCircle(m_vertices, i * m_vertsPerParticle,
                    drawPos, size * 0.5f, color, edge, m_mistSegments);
            }
            else {
                updateQuad(i, drawPos, size, color);
            }
        }
    }

    // Reposition the emitter (e.g. if the torch/camera moves).
    void setOrigin(sf::Vector2f origin) { m_origin = origin; }
    sf::Vector2f getOrigin() const { return m_origin; }

    // For FireTrail: tell the emitter which way it's currently moving, so
    // new particles drift backward from the direction of travel instead of
    // just scattering randomly. No-op for other presets. Call this once per
    // frame before update(), alongside setOrigin().
    void setEmitterVelocity(sf::Vector2f velocity) { m_emitterVelocity = velocity; }

private:
    struct Particle {
        sf::Vector2f pos;
        sf::Vector2f velocity;
        float life = 0.f;
        float maxLife = 1.f;
        float size = 2.f;
        float swaySpeed = 1.f;
        float swayOffset = 0.f;
        float swayAmount = 0.f;
        sf::Color color = sf::Color::White;
    };

    Preset m_preset;
    sf::Vector2f m_origin;
    sf::Vector2f m_emitterVelocity{};
    std::vector<Particle> m_particles;
    mutable sf::VertexArray m_vertices;
    float m_time = 0.f;
    std::size_t m_vertsPerParticle = 6;
    static constexpr int m_mistSegments = 10;

    static float frand(float lo, float hi) {
        return lo + (hi - lo) * (static_cast<float>(rand()) / RAND_MAX);
    }

    // Smooth fade in/out instead of a linear pop, looks much nicer for
    // small glowing particles.
    static float fadeCurve(float lifeRatio) {
        float fadeIn = std::min(1.f, (1.f - lifeRatio) * 6.f);
        float fadeOut = std::min(1.f, lifeRatio * 3.f);
        return std::min(fadeIn, fadeOut);
    }

    void respawn(Particle& p, bool staggerLife) {
        switch (m_preset) {
        case Preset::Ember: {
            // Spawn tightly around the torch, drift upward, purple/violet glow
            p.pos = m_origin + sf::Vector2f(frand(-4.f, 4.f), frand(-2.f, 2.f));
            p.velocity = sf::Vector2f(frand(-6.f, 6.f), frand(-40.f, -20.f));
            p.maxLife = frand(1.2f, 2.4f);
            p.size = frand(1.5f, 3.5f);
            p.swaySpeed = frand(1.5f, 3.f);
            p.swayAmount = frand(3.f, 8.f);
            p.color = sf::Color(
                static_cast<std::uint8_t>(frand(160, 220)),
                static_cast<std::uint8_t>(frand(80, 140)),
                static_cast<std::uint8_t>(frand(220, 255))
            );
            break;
        }
        case Preset::Mist: {
            // Spawn across a wide horizontal band, drift slowly sideways,
            // pale blue-grey, larger and slower than embers.
            p.pos = m_origin + sf::Vector2f(frand(-200.f, 200.f), frand(-20.f, 20.f));
            p.velocity = sf::Vector2f(frand(8.f, 20.f), frand(-2.f, 2.f));
            p.maxLife = frand(6.f, 12.f);
            p.size = frand(40.f, 90.f);
            p.swaySpeed = frand(0.2f, 0.5f);
            p.swayAmount = frand(10.f, 25.f);
            p.color = sf::Color(150, 170, 200, 60);
            break;
        }
        case Preset::FireTrail: {
            // Spawn right at the fireball's current position, then drift
            // backward relative to its direction of travel, so this reads
            // as a trailing tail rather than a puff of static smoke.
            sf::Vector2f dir = m_emitterVelocity;
            float speed = std::sqrt(dir.x * dir.x + dir.y * dir.y);
            sf::Vector2f back = (speed > 0.001f) ? (dir / -speed) : sf::Vector2f(0.f, 0.f);

            p.pos = m_origin + sf::Vector2f(frand(-2.f, 2.f), frand(-2.f, 2.f));
            p.velocity = back * frand(30.f, 70.f) + sf::Vector2f(frand(-15.f, 15.f), frand(-15.f, 15.f));
            p.maxLife = frand(0.18f, 0.35f); // short-lived, snappy tail
            p.size = frand(20.f, 14.f);
            p.swaySpeed = frand(2.f, 4.f);
            p.swayOffset = frand(0.f, 6.28318f);
            p.swayAmount = frand(1.f, 3.f);

            // Purple/violet firey palette, matching the staff aura's family
            // of colors -- warm magenta-pink core fading into deep violet.
            p.color = sf::Color(
                static_cast<std::uint8_t>(frand(170, 230)),
                static_cast<std::uint8_t>(frand(30, 70)),
                static_cast<std::uint8_t>(frand(200, 255))
            );
            break;
        }
        }

        p.life = staggerLife ? frand(0.f, p.maxLife) : p.maxLife;
    }

    void updateQuad(std::size_t index, sf::Vector2f pos, float size, sf::Color color) const {
        sf::Vertex* v = &m_vertices[index * 6];
        float hs = size * 0.5f;

        sf::Vector2f topLeft = pos + sf::Vector2f(-hs, -hs);
        sf::Vector2f topRight = pos + sf::Vector2f(hs, -hs);
        sf::Vector2f bottomRight = pos + sf::Vector2f(hs, hs);
        sf::Vector2f bottomLeft = pos + sf::Vector2f(-hs, hs);

        // Triangle 1: topLeft, topRight, bottomRight
        v[0].position = topLeft;
        v[1].position = topRight;
        v[2].position = bottomRight;

        // Triangle 2: topLeft, bottomRight, bottomLeft
        v[3].position = topLeft;
        v[4].position = bottomRight;
        v[5].position = bottomLeft;

        for (int i = 0; i < 6; ++i) {
            v[i].color = color;
        }
    }

    // sf::Drawable interface — lets you just do window.draw(particleSystem)
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.blendMode = sf::BlendAdd; // additive blending = nice glow effect
        target.draw(m_vertices, states);
    }
};