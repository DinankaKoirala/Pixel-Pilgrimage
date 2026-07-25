#include"Header/player.h"
#include"Header/tilemap.h"
#include<vector>
#include <SFML/Graphics.hpp>

Player::Player(float playerOriginX , float playerOriginY,AudioManager& audio):sprite(idleTexture), audio(audio)
{
    hitbox = sf::FloatRect({playerOriginX,playerOriginY},{32.f,48.f});
}

void Player::update(float dt, const std::vector<sf::FloatRect>& solids) {
    onGround = false;

    // --- Crouch Hitbox Logic ---
    if (isCrouching && hitbox.size.y == 48.f) {
        hitbox.size.y = 32.f;
        hitbox.position.y += 16.f; // shift down to keep feet on the ground
    } else if (!isCrouching && hitbox.size.y == 32.f) {
        // Try to stand up
        sf::FloatRect standBox = hitbox;
        standBox.size.y = 48.f;
        standBox.position.y -= 16.f;
        bool canStand = true;
        for (const sf::FloatRect& solid : solids) {
            if (standBox.findIntersection(solid)) {
                canStand = false;
                break;
            }
        }
        if (canStand) {
            hitbox = standBox;
        } else {
            isCrouching = true; // force crouch state because ceiling is blocking
        }
    }
    // ---------------------------

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
            } else if (velocity.y < 0) {
                hitbox.position.y += overlap->size.y;
                velocity.y = 0;
            }
        }
    }

    // --- Animation & Sprite State Logic ---
    if (!onGround) {
        state = PlayerState::Jump;
    } else if (isCrouching) {
        state = PlayerState::Crouch;
    } else if (velocity.x != 0.f) {
        state = PlayerState::Walk;
    } else {
        state = PlayerState::Idle;
    }

    animTimer += dt;
    if (state == PlayerState::Walk) {
        if (animTimer >= 0.15f) {
            animTimer = 0.f;
            currentFrame = (currentFrame + 1) % 2;
        }
    } else {
        currentFrame = 0;
    }

    float scaleX = 1.f;
    float scaleY = 1.f;

    if (state == PlayerState::Idle) {
        sprite.setTexture(idleTexture, true);
        scaleX = 32.f / 293.f;
        scaleY = 48.f / 428.f;
    } else if (state == PlayerState::Jump) {
        sprite.setTexture(jumpTexture, true);
        scaleX = 32.f / 389.f;
        scaleY = 48.f / 411.f;
    } else if (state == PlayerState::Crouch) {
        sprite.setTexture(crouchTexture, true);
        scaleX = 32.f / 324.f;
        scaleY = 32.f / 250.f;
    } else if (state == PlayerState::Walk) {
        sprite.setTexture(walkTexture, false);
        int frameWidth = 557 / 2; // 278
        sprite.setTextureRect(sf::IntRect({currentFrame * frameWidth, 0}, {frameWidth, 321}));
        scaleX = 32.f / (float)frameWidth;
        scaleY = 48.f / 321.f;
    }
    
    // Apply facing direction
    if (!facingRight) {
        scaleX = -scaleX;
    }
    sprite.setScale({scaleX, scaleY});

    // Set origin to bottom-center of the unscaled image frame
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin({ bounds.size.x / 2.f, bounds.size.y });

    // Position sprite at the bottom-center of the hitbox
    sprite.setPosition({
        hitbox.position.x + hitbox.size.x / 2.f,
        hitbox.position.y + hitbox.size.y
    });
}

void Player::draw(sf::RenderWindow& window){
    window.draw(sprite);
}

void Player::handleInput() {
    velocity.x = 0.f;
    isCrouching = false;

    if (onGround && (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Down))) {
        isCrouching = true;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right)){
        velocity.x = 200.f;
        facingRight = true;
        audio.playSFXIfNotPlaying("footstep");
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left)){
        velocity.x = -200.f;
        facingRight = false;
        audio.playSFXIfNotPlaying("footstep");
    }

    if (!isCrouching) {
         if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up)) && onGround == true) {
            velocity.y = -130;
            onGround = false;
            audio.playSFX("jump");
        }
    }
}


sf::Vector2f Player::getPosition() const {
    return hitbox.position;
}

bool Player::loadTextures(){
    if(!idleTexture.loadFromFile("../src/level-1/assets/textures/idle.png" )) return false;
    if(!walkTexture.loadFromFile("../src/level-1/assets/textures/walk_spritesheet.png" )) return false;
    if(!jumpTexture.loadFromFile("../src/level-1/assets/textures/jump.png" )) return false;
    if(!crouchTexture.loadFromFile("../src/level-1/assets/textures/crouch.png" )) return false;

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