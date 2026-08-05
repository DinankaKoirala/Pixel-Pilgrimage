#pragma once
#include <vector>
#include<SFML/Graphics.hpp>
#include "drawable.h"

class Background : public Drawable{
    private:
    sf::Texture BackgroundTexture;
    sf::Sprite sprite;
    float BackgroundWidth ;
    float cameraPosX = 0.f;

    public:
    Background();
    bool loadTexture(const std::string& path);
    void setCameraPos(float x);
    void draw(sf::RenderWindow& window) override;
};
