#include"Header/enemy.h"

Enemy::Enemy(float enemyOriginX , float enemyOriginY):sprite(enemyTexture){
hitbox = sf::FloatRect({enemyOriginX,enemyOriginY},{64,32}); 
}
bool Enemy::loadTextures(const std::string& path){
    if(!enemyTexture.loadFromFile(path)){
        return false;
    }
    sprite.setTexture(enemyTexture, true);
    sprite.setTextureRect(sf::IntRect({0, 0}, {64, 64}));
    sprite.setScale({1.5f, 1.5f});
    return true;
}
void Enemy::draw(sf::RenderWindow& window){
    if(!movingRight){
        sprite.setScale({1.5f, 1.5f});
        sprite.setPosition({hitbox.position.x-16.f, hitbox.position.y - 33.f});
    } else {
        sprite.setScale({-1.5f, 1.5f});
        sprite.setPosition({hitbox.position.x + hitbox.size.x+16.f, hitbox.position.y- 33.f});
    }
    window.draw(sprite);

    /*   SEE HITBOX FOR DEBUG
        sf::RectangleShape debug({hitbox.size.x, hitbox.size.y});
        debug.setPosition(hitbox.position);
        debug.setFillColor(sf::Color(255, 0, 0, 80));
        window.draw(debug);
    */
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
}

sf::FloatRect Enemy::getEnemyHitbox() const{
    return hitbox;
}

void Enemy::reset(float spawnX, float spawnY){
    hitbox.position = {spawnX , spawnY};
    movingRight = true;
}
