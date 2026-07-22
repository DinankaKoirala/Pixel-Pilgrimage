#include<iostream>
#include"Header/coin.h"

int Coin::coinsCollected = 0;

Coin::Coin(float coinOriginX , float coinOriginY):sprite(coinTexture){
hitbox = sf::FloatRect({coinOriginX,coinOriginY},{32,32}); 
}

bool Coin::loadTextures(const std::string& path){
    if(!coinTexture.loadFromFile(path)){
        return false;
    }
    sprite.setTexture(coinTexture, true);
    sprite.setTextureRect(sf::IntRect({0, 0}, {32, 32}));
    sprite.setScale({1.25f, 1.25f});
    return true;
}

void Coin::draw(sf::RenderWindow& window){
    sprite.setPosition(hitbox.position);
    if(!collected){
    window.draw(sprite);
    }

    /*   SEE HITBOX FOR DEBUG
        sf::RectangleShape debug({hitbox.size.x, hitbox.size.y});
        debug.setPosition(hitbox.position);
        debug.setFillColor(sf::Color(255, 0, 0, 80));
        window.draw(debug);
    */
}

sf::Vector2f Coin::getPosition() const{
    return hitbox.position;
}

sf::FloatRect Coin::getCoinHitbox() const{
    return hitbox;
}
void Coin::reset(float spawnX, float spawnY){
    if(collected) coinsCollected--;
    hitbox.position = {spawnX , spawnY};
    collected = false;
}


bool Coin::isCollected(){
    return collected;
}

void Coin::collect(){
    collected = true;
    coinsCollected++;
}