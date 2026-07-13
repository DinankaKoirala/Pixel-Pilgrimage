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

     float probeX = movingRight ? hitbox.position.x + hitbox.size.x + 5.f : hitbox.position.x - 5.f;
    float probeY = hitbox.position.y + hitbox.size.y + 6.f;
    sf::Vector2f probePoint = {probeX, probeY};

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
    bool groundAhead = false;
    for (const sf::FloatRect& solid : solids) {
        if (solid.contains(probePoint)) {
            groundAhead = true;
            break;
        }
    }
        if (!groundAhead) {
            if(!movingRight){
                movingRight = true;
            }
            else{
                movingRight=false;
            }
        }

    enemyShape.setPosition(hitbox.position);
}

sf::FloatRect Enemy::getEnemyHitbox() const{
    return hitbox;
}

void Enemy::reset(float spawnX, float spawnY){
    hitbox.position = {spawnX , spawnY};
    movingRight = true;
}
