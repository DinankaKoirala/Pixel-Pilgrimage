#include"Header/background.h"
#include<SFML/Graphics.hpp>


Background::Background():sprite(BackgroundTexture){

}

bool Background::loadTexture(const std::string& path){
    if(!BackgroundTexture.loadFromFile(path)){
        return false;
    }
    sprite.setTexture(BackgroundTexture, true);
    BackgroundWidth = BackgroundTexture.getSize().x;
    return true;
}
void Background::setCameraPos(float x){
    cameraPosX = x;
}
void Background::draw(sf::RenderWindow& window){
    float cameraLeft = cameraPosX - window.getSize().x / 2.f;

    int backgroundIndex = static_cast<int>(cameraLeft / BackgroundWidth);

    for (int i = backgroundIndex; i <= backgroundIndex + 3; i++)
    {
        float x = i * BackgroundWidth;

        sprite.setPosition({x, 0.f});

        window.draw(sprite);
    }
}