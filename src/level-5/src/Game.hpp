#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>

namespace L5 {

class Game5 {
public:
    explicit Game5(sf::RenderWindow& window);
    bool run();

private:
    sf::RenderWindow& window;
    bool gameOver;
    bool gameWon;
    bool deathHandled;

    sf::Texture bgTexture;
    sf::Texture playerTexture;
    sf::Texture orbTexture;
    sf::Texture fireballTexture;
    sf::Texture monkTexture;
    sf::Texture spellTexture;

    sf::Sprite background;
    sf::Sprite player;
    sf::Sprite orb;

    float sx, sy;
    unsigned int winL, winB;

    int playerFrameCols = 6;
    int playerFrameRows = 2;
    int playerFrameCount = 12;
    int playerFrameW, playerFrameH;
    int playerAnimFrame;
    float playerAnimTimer;
    static constexpr float playerAnimFrameDuration = 0.09f;
    bool playerFacingLeft;
    float playerSpeed;
    float velocityY;
    float gravity;
    float jumpStrength;
    bool onGround;

    struct Fireball {
        sf::Sprite shape;
        sf::Vector2f velocity;
        Fireball(const sf::Texture& tex) : shape(tex) {}
    };
    std::vector<Fireball> fireballs;
    float fireballSpeed;
    float spawnTimer;
    float spawnInterval;
    sf::Vector2f fireballOrigin;
    sf::Vector2f fireballScale;

    float ambientTime;
    sf::Vector2f bgMargin;
    float bgOverscan;

    float survivalTimeRemaining;
    float survivalDuration;
    int monkHealthPercent;

    int maxLives;
    int remainingLives;
    float invincibilityTimer;

    sf::Sprite monkSprite;
    sf::Vector2f monkBasePos;
    sf::Vector2f monkPos;
    float monkMovementTime;
    float monkPatrolAmplitude;
    float monkPatrolSpeed;
    float monkHoverAmplitude;
    float monkHoverSpeed;
    float monkSpawnTimer;
    float monkSpawnInterval;
    float monkSpellSpeed;
    float monkSpellScale;
    struct MonkSpell {
        sf::Sprite shape;
        sf::Vector2f velocity;
        MonkSpell(const sf::Texture& tex) : shape(tex) {}
    };
    std::vector<MonkSpell> monkSpells;

    sf::Vector2f staffOffsetLocal{-150.f, -300.f};

    struct DripstoneEntry {
        sf::ConvexShape shape;
        sf::ConvexShape warningShape;
        sf::Sprite sprite;
        enum State { Telegraphing, Falling, Landed } state = Telegraphing;
        float telegraphTimer = 0.f;
        float fallSpeed = 0.f;
        float groundY = 0.f;
        sf::Vector2f size{20.f, 40.f};
        DripstoneEntry(const sf::Texture& tex) : sprite(tex) {}
    };
    std::vector<DripstoneEntry> dripstones;
    float dripstoneSpawnTimer;
    float dripstoneSpawnInterval;
    float dripstoneMinX;
    float dripstoneMaxX;
    float dripstoneCeilingY;
    float dripstoneGroundY;

    sf::Font font;
    bool fontLoaded;

    sf::SoundBuffer hitBuffer;
    sf::SoundBuffer whooshBuffer;
    sf::SoundBuffer laughBuffer;
    sf::Sound hitSound;
    sf::Sound whooshSound;
    sf::Sound laughSound;
    sf::Music bgm;
    float laughTimer;

    void loadAssets();
    void initPlayer();
    void initMonk();
    void initOrb();
    void processEvents();
    void update(float dt);
    void updatePlayer(float dt);
    void updateFireballs(float dt);
    void updateMonk(float dt);
    void updateDripstones(float dt);
    void updateSound(float dt);
    void checkCollisions();
    void render();
    void resetGame();
    float groundY() const { return 400.f * sy; }
    float winL_f() const { return static_cast<float>(winL); }
    float winB_f() const { return static_cast<float>(winB); }
};

} // namespace L5
