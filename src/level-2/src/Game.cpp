// Game.cpp
#include "Game.h"
#include "Level.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>

Game::Game()
    : window(sf::VideoMode({ 1280u, 720u }), "Night Bridge Level")
    , view(sf::FloatRect({ 0.f, 0.f }, { 1280.f, 720.f }))
    , assetsPath("../src/level-2/assets/")
    , background(bgTex)
    , player(sf::Vector2f(150.f, 550.f))
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    window.setView(view);

    if (!loadAssets())
    {
        // A texture failed to load - close the window so run() exits
        // immediately instead of showing a broken/blank game.
        window.close();
        return;
    }

    createLevel(platforms, starSpawner, blockTex, crackedTex);
}

bool Game::loadAssets()
{
    if (!bgTex.loadFromFile(assetsPath + "bk.png"))             return false;

    // background was constructed (in the initializer list) before bgTex had
    // any pixels loaded, so its sprite "remembers" a 0x0 texture rect.
    // Refresh it now that bgTex actually has image data, telling it to
    // reset the rect to match the texture's real size.
    background.setTexture(bgTex, true);

    if (!blockTex.loadFromFile(assetsPath + "block.png"))       return false;
    if (!crackedTex.loadFromFile(assetsPath + "woodcrack.png")) return false;
    if (!starTex.loadFromFile(assetsPath + "ninjastar.png"))    return false;

    if (!player.loadTextures(assetsPath)) return false;

    if (!decorations.init(assetsPath, levelEnd)) return false;

    return true;
}

void Game::run()
{
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        processEvents();
        update(dt);
        render();
    }
}

void Game::processEvents()
{
    while (const std::optional<sf::Event> event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }

        if (const auto* key = event->getIf<sf::Event::KeyPressed>())
        {
            if (key->code == sf::Keyboard::Key::Up && !gameOver)
            {
                player.jump();
            }
            if (key->code == sf::Keyboard::Key::R && gameOver)
            {
                restartGame(player, platforms, ninjaStars, starSpawner, gameOver, blockTex, crackedTex);
            }
        }
    }
}

void Game::update(float dt)
{
    // Snapshot where the player's feet were BEFORE this frame's move -
    // needed to tell "landing on a platform" apart from "already fell
    // through and drifted into the next tile's box".
    float prevPlayerBottom =
        player.getBounds().position.y + player.getBounds().size.y;

    if (!gameOver)
    {
        player.stopHorizontal();
        player.moveRight();
        player.update(dt);
    }

    starSpawner.update(player.shape.getPosition().x, ninjaStars, starTex);

    cameraX = player.shape.getPosition().x;

    // Don't let the camera show anything left of where the level actually
    // begins - otherwise the player starts out staring at empty background
    // with no platform under them. Once the player walks far enough right
    // past this point, the clamp stops mattering and the camera follows
    // them normally (centered).
    float halfViewWidth = view.getSize().x / 2.f;
    float minCameraX = levelStart + halfViewWidth;
    if (cameraX < minCameraX)
    {
        cameraX = minCameraX;
    }

    view.setCenter({ cameraX, 360.f });
    window.setView(view);
    player.onGround = false;

    for (auto& p : platforms)
    {
        p->update(dt);
    }

    checkCrackTriggers();

    if (!gameOver)
    {
        checkCollisions(prevPlayerBottom);

        if (player.shape.getPosition().y > 720.f)
        {
            gameOver = true;
        }
    }

    if (!gameOver)
    {
        for (auto& star : ninjaStars)
        {
            star->update(dt);
            if (player.getBounds().findIntersection(star->getBounds()))
            {
                gameOver = true;
            }
        }
    }

    ninjaStars.erase(
        std::remove_if(
            ninjaStars.begin(),
            ninjaStars.end(),
            [](const auto& star) { return star->getBounds().position.x < -900.f; }),
        ninjaStars.end());

    decorations.update(cameraX, dt); // dt now drives sway/bob/drift/loop animations
}

void Game::checkCrackTriggers()
{
    const float warningDistance = 120.f;

    for (auto& p : platforms)
    {
        auto* cracked = dynamic_cast<CrackedPlatform*>(p.get());

        if (!cracked) continue;
        if (cracked->hasStartedFalling()) continue;

        float playerRight = player.getBounds().position.x + player.getBounds().size.x;
        float platformLeft = cracked->getBounds().position.x;
        float distance = platformLeft - playerRight;

        if (distance >= 0.f && distance <= warningDistance)
        {
            cracked->triggerCrack();
        }
    }
}

void Game::checkCollisions(float prevPlayerBottom)
{
    for (auto& p : platforms)
    {
        bool intersects = player.getBounds().findIntersection(p->getBounds()).has_value();

        if (auto* cracked = dynamic_cast<CrackedPlatform*>(p.get()))
        {
            // Cracked planks are a hazard, not real ground: touching one
            // at all ends the run immediately.
            if (intersects)
            {
                gameOver = true;
                break;
            }
            continue;
        }

        if (!intersects) continue;

        const float landingTolerance = 4.f;
        float platformTop = p->getBounds().position.y;

        bool wasAboveSurface = prevPlayerBottom <= platformTop + landingTolerance;

        if (player.velocity.y >= 0.f && wasAboveSurface)
        {
            player.shape.setPosition({
                player.shape.getPosition().x,
                platformTop - player.getBounds().size.y
                });

            player.velocity.y = 0.f;
            player.onGround = true;
        }
    }
}

void Game::render()
{
    window.clear();

    for (int i = -1; i < 30; i++)
    {
        background.setPosition({ i * 1280.f, 0.f });
        window.draw(background);
    }

    decorations.draw(window);

    for (auto& p : platforms)
    {
        p->draw(window);
    }

    for (auto& star : ninjaStars)
    {
        star->draw(window);
    }

    player.draw(window);

    window.display();
}