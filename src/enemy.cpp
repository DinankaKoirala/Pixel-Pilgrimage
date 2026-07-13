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
    return hitbox.position;
}
void Enemy::update( float dt , const std::vector<sf::FloatRect>& solids){

    float velocityX = speed * (movingRight ? 1.f : -1.f);
    hitbox.position.x += velocityX * dt;
    
   for (const sf::FloatRect& solid : solids) {
        if (auto overlap = hitbox.findIntersection(solid)) {
            if (movingRight) {
                hitbox.position.x -= overlap->size.x;
                movingRight = false;
            } else {
                hitbox.position.x += overlap->size.x;
                movingRight = true;
            }
        }
    }

    enemyShape.setPosition(hitbox.position);
}
