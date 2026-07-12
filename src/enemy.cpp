#include"Header/enemy.h"

Enemy::Enemy(float enemyOriginX , float enemyOriginY){
hitbox = sf::FloatRect({enemyOriginX,enemyOriginY},{32,32}); 
enemyShape.setSize({32,32});
enemyShape.setPosition({enemyOriginX,enemyOriginY});
enemyShape.setFillColor(sf::Color::White);
}
void Enemy::draw(sf::RenderWindow& window){
    window.draw(enemyShape);
}

sf::Vector2f Enemy::getPosition() const{
    return {enemyOriginX,enemyOriginY};
}
void Enemy::update( float dt , const std::vector<sf::FloatRect>& solid){
    for (const sf:FloatRect& solid : solids){
        if (auto overlap = hitbox.findIntersection(solid)) {
            if(velocity.x > 0){
            hitbox.position.x += -overlap->size.x ; 
            speed = 0;     
            }
            if(velocity.x<0){
                hitbox.position.x += overlap->size.x ;
                speed = 0;
            }
        }
    }
    enemyShape.move({speed , 0});
}