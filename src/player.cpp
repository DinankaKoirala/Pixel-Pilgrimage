#include"Header/player.h"

#include<vector>
#include <SFML/Graphics.hpp>

Player::Player(float playerOriginX , float playerOriginY){
    playerBox.setSize({32.f,32.f});
    playerBox.setFillColor(sf::Color::Red);
    playerBox.setPosition({playerOriginX , playerOriginY});
}

void Player::update(float dt , const std::vector<sf::FloatRect>& solids){
onGround = false;
velocity.y += GRAVITY*dt;
playerBox.move({velocity.x*dt , 0});

for(const sf::FloatRect& solid : solids){
    if(auto overlap = playerBox.getGlobalBounds().findIntersection(solid)){
        if(velocity.x > 0){
            playerBox.move( {-overlap->size.x , 0.f} ); 
            velocity.x = 0;     
        }

        if(velocity.x < 0){
            playerBox.move( {overlap->size.x , 0.f} ); 
            velocity.x = 0;   
        }
    }
}

playerBox.move({0 , velocity.y*dt});
for(const sf::FloatRect& solid : solids){
    if(auto overlap = playerBox.getGlobalBounds().findIntersection(solid)){
        if(velocity.y > 0){
            playerBox.move( {0.f , -overlap->size.y} ); 
            velocity.y = 0;  
            onGround = true;   
        }
    }
}

}

void Player::draw(sf::RenderWindow& window){
    window.draw(playerBox);
}

void Player::handleInput() {
    velocity.x = 0.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D))
        velocity.x = 200.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A))
        velocity.x = -200.f;

     if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W) && onGround) {
        velocity.y = -130;
        onGround = false;
    }
}


sf::Vector2f Player::getPosition() const {
    return playerBox.getPosition();
}
