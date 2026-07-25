#pragma once
#include "GameObject.hpp"

// The captured princess. She doesn't act on her own - she's positioned
// relative to the villain each frame (carried during the intro, then
// waiting beside him at the top once the fight begins).
class Princess : public GameObject {
public:
    Princess();

    void update(float dt) override; // currently just a hook, kept for consistency

    void followAnchor(sf::Vector2f anchorPosition);

private:
    sf::Vector2f m_offset; // her position relative to whatever she's anchored to
};
