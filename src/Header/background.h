#pragma once
#include <vector>
#include<SFML/Graphics.hpp>

class Background{
    private:
    sf::Texture BackgroundTexture;
    sf::Sprite sprite;
    float BackgroundWidth ;

    public:
    Background();
    bool loadTexture(const std::string& path);
    void draw(sf::RenderWindow& window , float cameraPosX);   
};