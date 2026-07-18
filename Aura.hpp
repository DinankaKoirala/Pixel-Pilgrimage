#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdint>
#include "VertexUtils.hpp"

// ---------------------------------------------------------------------------
// Aura.hpp
//
// A soft, pulsing glow anchored to a point -- e.g. the tip of the monk's
// staff. Reuses the same soft-circle technique as the mist (radial vertex
// color gradient, no texture needed), but animated with a breathing pulse
// and drawn additively so it actually looks like a magic light source
// rather than a flat blob.
//
// Usage:
//   Aura staffAura(sf::Color(190, 120, 255), 16.f); // violet glow, ~16px base radius
//
//   // each frame:
//   staffAura.update(dt, staffTipWorldPosition);
//   window.draw(staffAura);
// ---------------------------------------------------------------------------

class Aura : public sf::Drawable {
public:
    explicit Aura(sf::Color color = sf::Color(190, 120, 255), float baseRadius = 16.f)
        : m_color(color), m_baseRadius(baseRadius)
    {
        m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
        m_vertices.resize(softCircleVertexCount(m_segments));
    }

    // anchor = wherever the glow should appear this frame (e.g. staff tip).
    void update(float dt, sf::Vector2f anchor) {
        m_time += dt;

        // Two overlapping sine waves so the pulse doesn't feel like a
        // perfectly metronomic loop -- a little organic irregularity.
        float pulse = 0.5f + 0.35f * std::sin(m_time * 2.3f)
                           + 0.15f * std::sin(m_time * 5.1f + 1.7f);

        float radius = m_baseRadius * (0.8f + 0.35f * pulse);

        sf::Color centerColor = m_color;
        centerColor.a = static_cast<std::uint8_t>(130 + 100 * pulse);

        sf::Color edgeColor = m_color;
        edgeColor.a = 0;

        appendSoftCircle(m_vertices, 0, anchor, radius, centerColor, edgeColor, m_segments);
    }

private:
    sf::Color m_color;
    float m_baseRadius;
    float m_time = 0.f;
    static constexpr int m_segments = 16;
    mutable sf::VertexArray m_vertices;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.blendMode = sf::BlendAdd; // glow, not a flat translucent circle
        target.draw(m_vertices, states);
    }
};
