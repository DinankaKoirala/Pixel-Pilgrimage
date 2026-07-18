// main.cpp - Entry point for the platformer demo
// Sets up the game window, player, orb, fireballs, dripstones, and monk.
// Handles input, physics, collision, and rendering each frame.

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <optional>
#include <iostream>
#include <windows.h>
#include "dripstone.h"
#include "Monk.hpp"
#include "ParticleSystem.hpp"
#include "Aura.hpp"

class Fireball
{
public:
    sf::Sprite shape;
    sf::Vector2f velocity;
    ParticleSystem trail;

    Fireball(const sf::Texture& texture, sf::Vector2f spawnPos, sf::Vector2f vel)
        : shape(texture), velocity(vel),
        trail(ParticleSystem::Preset::FireTrail, spawnPos, 40)
    {
        trail.setEmitterVelocity(vel);
    }
};
int main()
{
    unsigned int win_l = 800;
    unsigned int win_b = 600;

    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    wchar_t* last = wcsrchr(path, L'\\');
    if (last) { *last = L'\0'; SetCurrentDirectoryW(path); }

    srand(static_cast<unsigned>(time(nullptr)));

    // Fullscreen at the native desktop resolution -- adapts to whatever
    // monitor/device this runs on instead of a fixed 800x600 window.
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktopMode, "Platform - Orb Fireball Demo", sf::State::Fullscreen);

    win_l = window.getSize().x;
    win_b = window.getSize().y;

    // Scale factor from the original 800x600 design resolution to whatever
    // the real screen turned out to be. Used throughout to keep gameplay
    // positions/speeds consistent across different monitor sizes.
    const float sx = static_cast<float>(win_l) / 800.f;
    const float sy = static_cast<float>(win_b) / 600.f;



    window.setFramerateLimit(60);


    sf::Texture bgTexture;
    if (!bgTexture.loadFromFile("Data/background.png"))
    {
        std::cerr << "Failed to load background.png\n";
    }

    sf::Sprite background(bgTexture);

    // Scale slightly larger than the screen (6% overscan) so we have room to
    // slowly pan/drift the background without ever revealing an edge --
    // this is what makes an otherwise-static image feel subtly alive.
    sf::Vector2u winSize = window.getSize();
    sf::Vector2u texSize = bgTexture.getSize();
    const float bgOverscan = 1.06f;
    background.setScale({
        bgOverscan * static_cast<float>(winSize.x) / texSize.x,
        bgOverscan * static_cast<float>(winSize.y) / texSize.y
        });

    const float bgMarginX = static_cast<float>(winSize.x) * (bgOverscan - 1.f) * 0.5f;
    const float bgMarginY = static_cast<float>(winSize.y) * (bgOverscan - 1.f) * 0.5f;
    float ambientTime = 0.f; // accumulates dt, drives background drift + aura pulse



    // Ambient particle emitters, positioned to match the torch braziers,
    // hanging cage-lanterns, and rear stained-glass window in background.png.
    // Positions are scaled proportionally to the window size in case win_l/win_b change.

    std::vector<ParticleSystem> embers;
    embers.emplace_back(ParticleSystem::Preset::Ember, sf::Vector2f(65.f * sx, 375.f * sy));   // left floor torch
    embers.emplace_back(ParticleSystem::Preset::Ember, sf::Vector2f(745.f * sx, 375.f * sy));  // right floor torch
    embers.emplace_back(ParticleSystem::Preset::Ember, sf::Vector2f(220.f * sx, 235.f * sy));  // left hanging cage lantern
    embers.emplace_back(ParticleSystem::Preset::Ember, sf::Vector2f(515.f * sx, 235.f * sy));  // right hanging cage lantern

    ParticleSystem mist(ParticleSystem::Preset::Mist, sf::Vector2f(380.f * sx, 90.f * sy));    // drifting mist near the rear window

    sf::RectangleShape ground({ 800.f * sx, 50.f * sy });
    ground.setPosition({ 0.f, 400.f * sy });
    ground.setFillColor(sf::Color(100, 100, 100, 50));

    sf::Texture playerTexture;

    if (!playerTexture.loadFromFile("Data/prince.png"))
    {
        return -1;
    }

    sf::Sprite player(playerTexture);

    player.setPosition({ 0.f, 400.f * sy });
    player.setScale({ 0.05f * sx, 0.05f * sy });

    float playerSpeed = 300.f * sx;
    float velocityY = 0.f;
    float gravity = 900.f * sy;
    float jumpStrength = -450.f * sy;
    bool onGround = true;

    sf::Texture orbTexture;
    if (!orbTexture.loadFromFile("Data/orb.png"))
    {
        std::cerr << "Failed to load orb.png\n";
    }

    sf::Sprite orb(orbTexture);
    sf::Vector2u orbTexSize = orbTexture.getSize();
    orb.setOrigin({ orbTexSize.x / 2.f, orbTexSize.y / 2.f });

    // Original orb was a 20.f-radius circle (40.f diameter) in design space.
    // Scale the sprite to match that same footprint, then apply sx/sy like
    // everything else so it stays consistent across screen resolutions.
    const float orbDesignDiameter = 400.f;
    orb.setScale({
        (orbDesignDiameter * sx) / orbTexSize.x,
        (orbDesignDiameter * sy) / orbTexSize.y
        });

    orb.setPosition({ 400.f * sx, 100.f * sy });

    sf::Texture fireballTexture;
    if (!fireballTexture.loadFromFile("Data/fireball.png"))
    {
        std::cerr << "Failed to load fireball.png\n";
    }

    sf::Vector2u fireballTexSize = fireballTexture.getSize();
    sf::Vector2f fireballOrigin = { fireballTexSize.x / 2.f, fireballTexSize.y / 2.f };

    // Original fireball was an 8.f-radius circle (16.f diameter) in design
    // space. Match that footprint, scaled by sx/sy like everything else.
    const float fireballDesignDiameter = 16.f;
    sf::Vector2f fireballScale = {
        (fireballDesignDiameter * sx) / fireballTexSize.x,
        (fireballDesignDiameter * sy) / fireballTexSize.y
    };

    std::vector<Fireball> fireballs;
    float fireballSpeed = 250.f * sx;
    float spawnTimer = 0.f;
    float spawnInterval = 0.1f;

    // Ground level here must match the player's floor collision (400.f * sy)
    // and spawn x-range must be scaled by sx, or dripstones will land at the
    // wrong height / drift outside the visible play area on non-800x600 screens.
    DripstoneManager dm(1.5f, 50.f * sx, 750.f * sx, 0.f, 400.f * sy);
    // Monk drifts +-140px horizontally and +-18px vertically around its
    // anchor, scaled the same way as everything else so the motion looks
    // consistent across screen resolutions. Cull bounds use the ACTUAL
    // window size (not the 800x600 design resolution) or spells spawned
    // off a scaled-up staff position get deleted the same frame they fire.
    Monk monk({ 600.f * sx, 200.f * sy }, 140.f * sx, 0.6f, 18.f * sy, 1.4f,
        400.f * sx,
        -60.f, static_cast<float>(win_l) + 60.f,
        -60.f, static_cast<float>(win_b) + 60.f);

    // Staff aura: violet glow, anchored to the monk's staff tip every frame.
    // See Monk::staffOffsetLocal in Monk.hpp if the glow doesn't line up
    // with the actual staff in your art -- nudge that offset to calibrate.
    Aura staffAura(sf::Color(190, 120, 255), 16.f * sx);
    bool gameOver = false;
    sf::Font font;
    bool fontLoaded = font.openFromFile("Data/Roboto-Medium.ttf");

    sf::Text gameOverText(font);

    if (fontLoaded)
    {
        gameOverText.setString("GAME OVER - Press R to Restart");
        gameOverText.setCharacterSize(36);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setPosition({ 150.f, 50.f });
    }
    sf::Clock clock;
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        ambientTime += dt;

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        // Subtle background drift: slow, small sine pan within the overscan
        // margin, so nothing ever reveals a hard edge.
        float bgOffsetX = std::sin(ambientTime * 0.15f) * bgMarginX;
        float bgOffsetY = std::cos(ambientTime * 0.11f) * (bgMarginY * 0.5f);
        background.setPosition({ -bgMarginX + bgOffsetX, -bgMarginY + bgOffsetY });

        for (auto& e : embers)
        {
            e.update(dt);
        }
        mist.update(dt);

        if (!gameOver)
        {
            sf::Vector2f pos = player.getPosition();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            {
                pos.x -= playerSpeed * dt;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            {
                pos.x += playerSpeed * dt;
            }
            if (pos.x < 0)
                pos.x = 0;

            if (pos.x > 760.f * sx)
                pos.x = 760.f * sx;
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) &&
                onGround)
            {
                velocityY = jumpStrength;
                onGround = false;
            }
            velocityY += gravity * dt;
            pos.y += velocityY * dt;

            if (pos.y >= 400.f * sy)
            {
                pos.y = 400.f * sy;
                velocityY = 0;
                onGround = true;
            }
            player.setPosition(pos);

            spawnTimer += dt;

            if (spawnTimer >= spawnInterval)
            {
                spawnTimer = 0.f;

                float angle =
                    static_cast<float>(rand() % 360) *
                    3.14159265f / 180.f;

                sf::Vector2f vel =
                {
                    std::cos(angle) * fireballSpeed,
                    std::sin(angle) * fireballSpeed
                };

                Fireball fb(fireballTexture, orb.getPosition(), vel);

                fb.shape.setOrigin(fireballOrigin);
                fb.shape.setScale(fireballScale);
                fb.shape.setPosition(orb.getPosition());

                fireballs.push_back(std::move(fb));
            }
            for (auto& fb : fireballs)
            {
                fb.shape.move(fb.velocity * dt);

                fb.trail.setOrigin(fb.shape.getPosition());
                fb.trail.setEmitterVelocity(fb.velocity);
                fb.trail.update(dt);
            }

            fireballs.erase(
                std::remove_if(
                    fireballs.begin(),
                    fireballs.end(),
                    [&](const Fireball& fb)
                    {
                        sf::Vector2f p = fb.shape.getPosition();

                        return p.x < -50.f ||
                            p.x > win_l + 50.f ||
                            p.y < -50.f ||
                            p.y > win_b + 50.f;
                    }),
                fireballs.end());

            dm.update(dt);
            monk.update(dt, player.getPosition());
            auto playerBounds = player.getGlobalBounds();

            for (auto& fb : fireballs)
            {
                if (playerBounds.findIntersection(fb.shape.getGlobalBounds()))
                {
                    gameOver = true;
                    break;
                }
                if (dm.checkCollision(playerBounds))
                {
                    gameOver = true;
                }
                if (monk.checkCollision(playerBounds))
                {
                    gameOver = true;
                }
            }

        }
        else
        {

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))
            {
                gameOver = false;

                player.setPosition({ 100.f * sx, 300.f * sy });

                velocityY = 0.f;
                onGround = true;

                fireballs.clear();

                spawnTimer = 0.f;
                dm.reset();
                monk.reset();
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::J))
            {
                gameOver = true;
                window.close();

            
            }
        }


        // Runs after monk.update() above so the aura tracks this frame's
        // staff-tip position rather than lagging one frame behind.
        staffAura.update(dt, monk.getStaffTipPosition());

        window.clear(sf::Color(30, 30, 40));
        window.draw(background);

        window.draw(mist);
        for (auto& e : embers)
        {
            window.draw(e);
        }

        window.draw(orb);
        window.draw(player);
        dm.draw(window);
        monk.draw(window);
        window.draw(staffAura);

        for (auto& fb : fireballs)
        {
            window.draw(fb.trail);
            window.draw(fb.shape);
        }

        if (gameOver && fontLoaded)
        {
            window.draw(gameOverText);
        }

        window.display();

    }

    return 0;
}