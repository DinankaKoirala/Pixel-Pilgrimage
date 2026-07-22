// GameObject.h
#pragma once
#include <SFML/Graphics.hpp>

class GameObject {
public:
    GameObject(const sf::Texture& texture, sf::Vector2f position)
        : sprite(texture)// this constructor gives texture to the sprite, which is a member of GameObject
    {
        sprite.setPosition(position);
    }

    virtual ~GameObject() = default;//virtual=derived classes can create their own update

    virtual void update(float dt) {} // default: does nothing, derived classes override//virtual because different objects move differently 

    virtual void draw(sf::RenderWindow& window) {  //virtual allows runtime polymorhism
        window.draw(sprite);
    }

    sf::FloatRect getBounds() const {
        return sprite.getGlobalBounds();//get bounds=collision detection , returns the rectangle that bounds the sprite
    }

protected:
    sf::Sprite sprite;
};