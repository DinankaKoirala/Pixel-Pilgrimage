#include"Header/player.h"
#include"Header/tilemap.h"
#include<vector>
#include <SFML/Graphics.hpp>

Player::Player(float playerOriginX , float playerOriginY,AudioManager& audio):sprite(idleTexture), audio(audio)
{
    hitbox = sf::FloatRect({playerOriginX,playerOriginY},{32.f,48.f});
    sprite.setScale({0.107f, 0.12f});
}

void Player::update(float dt, const std::vector<sf::FloatRect>& solids) {
    onGround = false;
    velocity.y += GRAVITY * dt;
    if (velocity.y > 100) {
       velocity.y = 100;
    }
    hitbox.position.x += velocity.x * dt;
    for (const sf::FloatRect& solid : solids) {
        if (auto overlap = hitbox.findIntersection(solid)) {
            if(velocity.x > 0){
            hitbox.position.x += -overlap->size.x ; 
            velocity.x = 0;     
            }
            if(velocity.x<0){
                hitbox.position.x += overlap->size.x ;
                velocity.x = 0;
            }

        }
    }

    hitbox.position.y += velocity.y * dt;
    for (const sf::FloatRect& solid : solids) {
        if (auto overlap = hitbox.findIntersection(solid)) {
            if(velocity.y > 0){
                hitbox.position.y += -overlap->size.y; 
                velocity.y = 0;  
                onGround = true;   
            }
        }
    }

    sprite.setPosition(hitbox.position);
}

void Player::draw(sf::RenderWindow& window){
    window.draw(sprite);
}

void Player::handleInput() {
    velocity.x = 0.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D) ){
        velocity.x = 200.f;
        audio.playSFXIfNotPlaying("footstep");
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A) ){
        velocity.x = -200.f;
        audio.playSFXIfNotPlaying("footstep");
    }

     if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W) && onGround == true) {
        velocity.y = -130;
        onGround = false;
        audio.playSFX("jump");
    }
}


sf::Vector2f Player::getPosition() const {
    return hitbox.position;
}

bool Player::loadTextures(){
    if(!idleTexture.loadFromFile("../src/Resources/idle.png" )){
        return false;
    }
    sprite.setTexture(idleTexture, true);
    return true;
}
sf::FloatRect Player::getPlayerHitbox() const{
    return hitbox;
}

void Player::reset(float spawnX, float spawnY){
    hitbox.position = {spawnX , spawnY};
    velocity = {0.f ,0.f};
    onGround = false;
}