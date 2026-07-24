// Decoration.h
#pragma once
#include "GameObject.h"

// A purely visual background object (cloud, hill, tree, bird...) that
// scrolls slower than the foreground to create a sense of depth (parallax scrolling).
class Decoration : public GameObject
{
public:
        Decoration(const sf::Texture& texture, sf::Vector2f basePosition, float parallaxFactor, bool centerOrigin = false, float scale = 1.f, float scaleX = -1.f)
        : GameObject(texture, basePosition)
        , basePos(basePosition)
        , factor(parallaxFactor)
    {
        sf::FloatRect bounds = sprite.getLocalBounds();//sprite size 
        if (centerOrigin)
            sprite.setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });//bird sun floating objects rotate around their center 
        else
            sprite.setOrigin({ bounds.size.x / 2.f, bounds.size.y });//if false bottom-center they sit on the ground 

        float finalScaleX = (scaleX >= 0.f) ? scaleX : scale;
        sprite.setScale({ finalScaleX, scale });
        sprite.setPosition(basePosition);
    }//scale and scale x seperately because it allows changing width without affecting height 

    void updateParallax(float cameraX) // When camera moves.. far objects move slower than near objects
    {
        sprite.setPosition({  // places decoration in the world 
            basePos.x + cameraX * (1.f - factor) + animOffset.x, //factor =1 no parallax 1-1=0
            basePos.y + animOffset.y
            });
        sprite.setRotation(sf::degrees(animRotationDeg));
    }
               //Runtime polymorphism 
        virtual void animate(float dt) {}//empty because decoration doesnot know how trees or birds should move 
                                         //lets child classes decide like tree sway bird flap and cloud drift 
protected:
    sf::Vector2f animOffset{ 0.f, 0.f }; // extra positional offset, added on top of parallax
    float animRotationDeg = 0.f;          // extra rotation, in degrees

private:
    sf::Vector2f basePos;
	float factor; //the smaller the factor the farther away the object is, and the slower it moves relative to the camera
};