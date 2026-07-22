#include "Princess.hpp"
#include "Utility.hpp"

Princess::Princess()
    : m_offset(-45.f, -10.f) // drawn slightly behind/above whoever she's anchored to
{
    m_texture = Utility::loadTextureOrPlaceholder("assets/princess.png", sf::Color(230, 120, 170));
    m_sprite.setTexture(m_texture);

    sf::Vector2u size = m_texture.getSize();
    m_sprite.setOrigin(size.x / 2.f, size.y / 2.f);

    float targetHeight = 90.f;
    float scale = targetHeight / static_cast<float>(size.y);
    m_sprite.setScale(scale, scale);
}

void Princess::update(float /*dt*/) {
    // No independent behavior - position is driven by followAnchor().
}

void Princess::followAnchor(sf::Vector2f anchorPosition) {
    m_sprite.setPosition(anchorPosition + m_offset);
}
