#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

// ---------------------------------------------------------------------------
// VertexUtils.hpp
//
// Small shared helper: builds a soft-edged circle (radial gradient, opaque
// center fading to transparent edge) out of plain triangles. SFML doesn't
// support a "blurred circle" shape natively, but since vertex colors are
// interpolated across a triangle by the GPU, a center vertex at full alpha
// and rim vertices at zero alpha gives a nice soft glow/fog blob for free,
// no textures required.
//
// Each circle uses `segments * 3` vertices (one triangle per segment, fanned
// around the center point). Caller must pre-size the sf::VertexArray to fit.
// ---------------------------------------------------------------------------

inline void appendSoftCircle(sf::VertexArray& verts, std::size_t startVertex,
                              sf::Vector2f center, float radius,
                              sf::Color centerColor, sf::Color edgeColor,
                              int segments = 12)
{
    constexpr float pi2 = 6.28318530718f;

    for (int i = 0; i < segments; ++i)
    {
        float a0 = (static_cast<float>(i) / segments) * pi2;
        float a1 = (static_cast<float>(i + 1) / segments) * pi2;

        sf::Vector2f p0 = center + sf::Vector2f(std::cos(a0), std::sin(a0)) * radius;
        sf::Vector2f p1 = center + sf::Vector2f(std::cos(a1), std::sin(a1)) * radius;

        sf::Vertex* tri = &verts[startVertex + static_cast<std::size_t>(i) * 3];
        tri[0].position = center; tri[0].color = centerColor;
        tri[1].position = p0;     tri[1].color = edgeColor;
        tri[2].position = p1;     tri[2].color = edgeColor;
    }
}

// Number of vertices a soft circle with the given segment count consumes.
inline std::size_t softCircleVertexCount(int segments)
{
    return static_cast<std::size_t>(segments) * 3;
}
