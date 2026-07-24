// Player.h
#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include <algorithm>

// Which pose the player is currently showing.
enum class PlayerState
{
    Idle,     // p.png        - airborne, past the initial jump moment
    Running,  // playerrun.png- on the ground
    Jumping   // player3.png  - the brief "about to jump" pose
};

class Player
{
public:
    Player(sf::Vector2f position)
        : sprite(idleTex) // sprite must be given a texture at construction;
        // idleTex is empty/blank until loadTextures() runs
    {
        // This is the HITBOX - drives physics and collisions. It stays a
        // small, consistent size no matter which pose is showing, so
        // collisions never get weird just because a pose image is bigger
        // or smaller than another. It is never drawn.
        shape.setSize({ 30.f, 50.f });
        shape.setPosition(position);
    }

    // Loads the three pose textures and hooks the sprite up to them.
    // Call this once, after construction, once assets are available -
    // same pattern as Game::loadAssets() refreshing `background` after
    // bgTex is loaded.
    bool loadTextures(const std::string& assetsPath)
    {
        if (!idleTex.loadFromFile(assetsPath + "p.png"))         return false;
        if (!runTex.loadFromFile(assetsPath + "playerrun.png"))  return false;
        if (!jumpTex.loadFromFile(assetsPath + "player3.png"))   return false;

        state = PlayerState::Running;
        sprite.setTexture(runTex, true); // true = reset the texture rect now that runTex has real pixels
        applyOrigin();
        sprite.setScale({ spriteScale, spriteScale });
        updateSpritePosition();

        return true;
    }

    void update(float dt)
    {
        // Detect the exact frame we touched down, to fire a landing squash
        if (!wasOnGroundLastFrame && onGround)
        {
            landingSquashTimer = squashDuration;
        }
        wasOnGroundLastFrame = onGround;

        // Ease current run speed toward whatever ceiling the coins have
        // pushed it to - this is what makes each pickup feel gradual
        // instead of an instant jump.
        moveSpeed += (targetMoveSpeed - moveSpeed) * std::min(1.f, speedSmoothing * dt);
        if (onGround)
        {
            coyoteTimer = coyoteTime;
        }
        else
        {
            coyoteTimer -= dt;
        }

        // Apply gravity
        velocity.y += gravity * dt;

        // Move according to velocity
        shape.move(velocity * dt);

        // Drives the run bob/lean by DISTANCE moved rather than raw time,
        // so it reads as footsteps tied to movement, not a random wobble.
        if (onGround)
        {
            distanceTraveled += std::fabs(velocity.x) * dt;
        }

        updateAnimationState();
        updateSecondaryMotion(dt);
        updateSpritePosition();
    }

    void moveLeft()
    {
        velocity.x = -moveSpeed;
    }

    void moveRight()
    {
        velocity.x = moveSpeed;
    }

    void stopHorizontal()
    {
        velocity.x = 0.f;
    }

    // Nudges the speed CEILING up by `amount` (capped at maxMoveSpeed).
    // The actual moveSpeed then eases toward that ceiling over the next
    // few frames in update(), instead of jumping instantly - so each coin
    // feels like a gradual speed-up rather than a sudden teleport.
    void boostSpeed(float amount)
    {
        targetMoveSpeed = std::min(targetMoveSpeed + amount, maxMoveSpeed);
    }

    // Back to the starting speed - call on restart.
    void resetSpeed()
    {
        moveSpeed = baseMoveSpeed;
        targetMoveSpeed = baseMoveSpeed;
    }

    void jump()
    {
        if (coyoteTimer > 0.f)
        {
            velocity.y = jumpSpeed;
            onGround = false;
            coyoteTimer = 0.f;          // Prevent double jumps
            takeoffSquashTimer = squashDuration;
        }
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(sprite); // only the sprite is ever drawn - shape is collision-only
    }

    sf::FloatRect getBounds() const
    {
        return shape.getGlobalBounds();
    }

public:
    sf::RectangleShape shape; // invisible hitbox - Game.cpp/Level.cpp keep using this exactly as before

    sf::Vector2f velocity{ 0.f, 0.f };

    bool onGround = false;
    void bufferJump()
    {
        jumpBufferTimer = jumpBufferTime;
    }

    void updateJumpBuffer(float dt)
    {
        if (jumpBufferTimer > 0.f)
            jumpBufferTimer -= dt;
    }

    bool hasBufferedJump() const
    {
        return jumpBufferTimer > 0.f;
    }

    void clearJumpBuffer()
    {
        jumpBufferTimer = 0.f;
    }

private:
    
    // Decides which pose SHOULD be showing right now, and swaps the
    // texture only when the pose actually changes.
    void updateAnimationState()
    {
        PlayerState newState;

        if (!onGround && velocity.y < 0.f)
            newState = PlayerState::Jumping;   // rising - lasts the whole way up to the apex
        else if (!onGround)
            newState = PlayerState::Idle;      // falling back down
        else
            newState = PlayerState::Running;   // on the ground

        if (newState == state) return; // already showing the right pose

        state = newState;

        switch (state)
        {
        case PlayerState::Idle:    sprite.setTexture(idleTex, true); break;
        case PlayerState::Running: sprite.setTexture(runTex, true);  break;
        case PlayerState::Jumping: sprite.setTexture(jumpTex, true); break;
        }

        applyOrigin(); // each pose image is a different size, so origin must be redone every swap
    }

    // Fakes life with single static poses via squash/stretch, tilt, and a
    // distance-driven bob - no extra art needed, just motion math applied
    // on top of whichever texture is currently active.
    void updateSecondaryMotion(float dt)
    {
        if (takeoffSquashTimer > 0.f) takeoffSquashTimer -= dt;
        if (landingSquashTimer > 0.f) landingSquashTimer -= dt;

        // ---- Squash & stretch pop (takeoff spring / landing impact) ----
        float targetScaleX = 1.f, targetScaleY = 1.f;

        if (takeoffSquashTimer > 0.f)
        {
            float t = takeoffSquashTimer / squashDuration; // 1 -> 0
            targetScaleX = 1.f + 0.16f * t; // stretch wide
            targetScaleY = 1.f - 0.14f * t; // and short, like coiling to spring up
        }
        else if (landingSquashTimer > 0.f)
        {
            float t = landingSquashTimer / squashDuration;
            targetScaleX = 1.f + 0.20f * t; // impact flattens him out briefly
            targetScaleY = 1.f - 0.16f * t;
        }

        // ---- Lean, depends on state ----
        float targetRotation = 0.f;
        float targetBob = 0.f;

        switch (state)
        {
        case PlayerState::Running:
            // Weight shifting stride-to-stride, tied to ground distance covered
            targetRotation = std::sin(distanceTraveled * bobFrequency) * runTiltDeg;
            targetBob = -std::fabs(std::sin(distanceTraveled * bobFrequency)) * bobHeight;
            break;
        case PlayerState::Jumping:
            targetRotation = jumpTiltDeg;  // leaning back slightly on the way up
            break;
        case PlayerState::Idle:
            targetRotation = fallTiltDeg;  // leaning forward, bracing to land
            break;
        }

        // Ease everything toward its target so pose swaps never pop
        float rotBlend = std::min(1.f, rotationSmoothing * dt);
        float scaleBlend = std::min(1.f, scaleSmoothing * dt);

        currentRotation += (targetRotation - currentRotation) * rotBlend;
        currentBob += (targetBob - currentBob) * scaleBlend;
        currentScaleX += (targetScaleX - currentScaleX) * scaleBlend;
        currentScaleY += (targetScaleY - currentScaleY) * scaleBlend;

        sprite.setRotation(sf::degrees(currentRotation));
        sprite.setScale({ spriteScale * currentScaleX, spriteScale * currentScaleY });
    }


    // Bottom-center origin, so no matter which pose is active, the
    // sprite's feet are the anchor point (matches how Decoration.h anchors
    // ground objects).
    void applyOrigin()
    {
        sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin({ bounds.size.x / 2.f, bounds.size.y });
    }

    // Keeps the visual sprite glued to the bottom-center of the hitbox
    // every frame, regardless of which pose/size is currently showing.
    void updateSpritePosition()
    {
        sf::Vector2f hitboxBottomCenter{
            shape.getPosition().x + shape.getSize().x / 2.f,
            shape.getPosition().y + shape.getSize().y + currentBob
        };
        sprite.setPosition(hitboxBottomCenter);
    }

    sf::Texture idleTex, runTex, jumpTex;
    sf::Sprite sprite;
    PlayerState state = PlayerState::Running;

    // --- secondary motion state (smoothed toward targets each frame) ---
    float currentRotation = 0.f;
    float currentBob = 0.f;
    float currentScaleX = 1.f;
    float currentScaleY = 1.f;

    float distanceTraveled = 0.f;      // accumulates while onGround, drives the run bob/lean
    bool wasOnGroundLastFrame = true;

    float takeoffSquashTimer = 0.f;
    float landingSquashTimer = 0.f;

    const float squashDuration = 0.12f;   // seconds each squash/stretch pop lasts
    const float bobFrequency = 0.05f;     // higher = faster steps per pixel moved
    const float bobHeight = 3.f;          // pixels, run bounce
    const float runTiltDeg = 4.f;         // degrees, stride-to-stride lean while running
    const float jumpTiltDeg = -7.f;       // degrees, lean while rising
    const float fallTiltDeg = 9.f;        // degrees, lean while falling
    const float rotationSmoothing = 9.f;  // higher = snappier easing
    const float scaleSmoothing = 14.f;

    // Your art is ~130-200px tall; tune this until the character looks
    // right next to your 64px-wide platform blocks. Start here and adjust.
    const float spriteScale = 0.45f;
    float jumpBufferTimer = 0.f;
    const float jumpBufferTime = 0.18f;
    float coyoteTimer = 0.f;
    const float coyoteTime = 0.10f;

    const float gravity = 900.f;
    const float jumpSpeed = -500.f;

    // --- coin-driven speed ramp ---
    float moveSpeed = 150.f;             // current speed actually applied in moveLeft/moveRight
    float targetMoveSpeed = 150.f;       // ceiling moveSpeed eases toward; raised by boostSpeed()
    const float baseMoveSpeed = 150.f;   // what resetSpeed() restores
    const float maxMoveSpeed = 260.f;    // hard cap so it doesn't get out of hand
    const float speedSmoothing = 1.5f;   // higher = faster ramp-up per coin
};