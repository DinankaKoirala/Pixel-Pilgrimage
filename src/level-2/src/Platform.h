// Platform.h//inheritance
#pragma once
#include "GameObject.h"

class Platform : public GameObject {
public:
    Platform(const sf::Texture& texture, sf::Vector2f position)//calls the constructor of the base class GameObject
        : GameObject(texture, position) {
    } // forwards to GameObject's constructor, which builds sprite(texture)

    virtual bool isSolid() const { return true; } // player can stand on it
};