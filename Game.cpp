// Game.cpp
#include "Game.h"
#include "Level.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>

Game::Game()//constructor initializes the game window, view, and other members
    : window(sf::VideoMode({ 1280u, 720u }), "Night Bridge Level")//window creation 
    , view(sf::FloatRect({ 0.f, 0.f }, { 1280.f, 720.f }))//creates a camera the camera follows the player 
    , assetsPath("assets/")
    , background(bgTex)
    , player(sf::Vector2f(150.f, 550.f))//player starts at 150,550
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));//without this always gives same pattern 
    window.setView(view);//with this trees different cloud different ...

    if (!loadAssets())
    {
        // A texture failed to load - close the window so run() exits
        // immediately instead of showing a broken/blank game.
        window.close();
        return;
    }

    createLevel(platforms, coins, starSpawner, blockTex, crackedTex, coinTex);
}

bool Game::loadAssets()
{
    if (!bgTex.loadFromFile(assetsPath + "bk.png"))             return false;


    background.setTexture(bgTex, true);

    if (!blockTex.loadFromFile(assetsPath + "block.png"))       return false;
    if (!crackedTex.loadFromFile(assetsPath + "woodcrack.png")) return false;
    if (!starTex.loadFromFile(assetsPath + "ninjastar.png"))    return false;
    if (!coinTex.loadFromFile(assetsPath + "coin.png"))         return false;
    scoreCoin.emplace(coinTex);

    scoreCoin->setScale({ 0.25f, 0.25f });
    scoreCoin->setPosition({ 1035.f, 18.f });

    if (!player.loadTextures(assetsPath)) return false;

    if (!decorations.init(assetsPath, levelEnd)) return false;//initialize decorations , load textures and build decorations

    if (!scoreFont.openFromFile(assetsPath + "Cinzel-Regular.ttf")) return false;
    scoreText.setFont(scoreFont);
    scoreText.setCharacterSize(28);
    scoreText.setFillColor(sf::Color(255, 215, 0));
    scoreText.setOutlineColor(sf::Color::Black);
    scoreText.setOutlineThickness(2.f);
    scoreText.setPosition({ 1085.f, 18.f});
    // ===== Score Panel =====
    scorePanel.setSize({ 250.f, 65.f });
    scorePanel.setPosition({ 1015.f, 10.f });

    scorePanel.setFillColor(sf::Color(0, 0, 0, 170));   // Semi-transparent black

    scorePanel.setOutlineThickness(3.f);
    scorePanel.setOutlineColor(sf::Color(255, 215, 0)); // Gold border
    // fixed HUD corner - drawn with the default view, not the scrolling one
    scoreText.setString("Score: 0");

    return true;//everything loaded successfully
}
//GAME LOOP
void Game::run()
{
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();//dt is the time taken by one frame. It makes movement independent of FPS.
        processEvents();//handles keyboard
        update(dt);
        render();
    }
}
//ALL keyboard stuffs handled here 
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
            
            if (key->code == sf::Keyboard::Key::R && (gameOver || gameWon))
            {
                restartGame(player, platforms, ninjaStars, coins, starSpawner, gameOver, blockTex, crackedTex, coinTex);
                gameWon = false;
                crackedPlatformsPassed = 0;
                score = 0;
                scoreText.setString("Score: 0");
            }//calls restart game 
        }
    }
}

void Game::update(float dt)//runs every functiom 
{
    //stores previous feet position 
    float prevPlayerBottom =
        player.getBounds().position.y + player.getBounds().size.y;

    if (!gameOver && !gameWon)
    { // move player shitttttts
        player.stopHorizontal();
        player.moveRight();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
        {
            player.bufferJump();
        }
        player.updateJumpBuffer(dt);
        player.update(dt);//apply gravity movement animation 
    }

    starSpawner.update(player.shape.getPosition().x, ninjaStars, starTex);

    cameraX = player.shape.getPosition().x;//camera follows player 

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

    if (!gameWon) // stop triggering new cracks once the level's already won
    {
        checkCrackTriggers();//starts crack animation 
    }

    if (!gameOver && !gameWon)
    {
        checkCollisions(prevPlayerBottom);//collision detection 
        //Execute buffered jump after landing
            if (player.hasBufferedJump() && player.onGround)
            {
                player.jump();
                player.clearJumpBuffer();
            }

        if (player.shape.getPosition().y > 720.f)
        {
            gameOver = true;//player died fell off the screen
        }
    }

    if (!gameOver && !gameWon)
    {
        for (auto& star : ninjaStars) //ninja stars collision 
        {
            star->update(dt);
            if (player.getBounds().findIntersection(star->getBounds()))
            {
                gameOver = true;
            }
        }
    }

    for (auto& c : coins) c->update(dt); // keep animating (spin/bob) even mid-collection frame

    if (!gameOver && !gameWon)
    {
        checkCoinCollisions();
    }

    ninjaStars.erase(// remove stars off screen 
        std::remove_if(
            ninjaStars.begin(),
            ninjaStars.end(),
            [](const auto& star) { return star->getBounds().position.x < -900.f; }),
        ninjaStars.end());

    decorations.update(cameraX, dt); // dt now drives sway/bob/drift/loop animations
}

void Game::checkCrackTriggers() //Warn player before reaching cracked platform.
{
    const float warningDistance = 120.f; // 12p px then the platform cracks 

    for (auto& p : platforms)
    {
        auto* cracked = dynamic_cast<CrackedPlatform*>(p.get());//Is this platform cracked?

        if (!cracked) continue;
        if (cracked->hasStartedFalling()) continue;

        float playerRight = player.getBounds().position.x + player.getBounds().size.x;//gets right side of the player 
        float platformLeft = cracked->getBounds().position.x;
        float distance = platformLeft - playerRight;

        if (distance >= 0.f && distance <= warningDistance)
        {
            if (cracked->triggerCrack()) // only counts the first time it fires
            {
                crackedPlatformsPassed++;
                if (crackedPlatformsPassed >= crackedPlatformsToWin)
                {
                    gameWon = true;
                }
            }
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

void Game::checkCoinCollisions()
{
    for (auto& coin : coins)
    {
        if (coin->isCollected()) continue;

        if (player.getBounds().findIntersection(coin->getBounds()))
        {
            coin->collect();
            score += scorePerCoin;
            player.boostSpeed(speedBoostPerCoin); // ramps in gradually, see Player::update

            scoreText.setString("Score: " + std::to_string(score));
        }
    }

    // Drop collected coins from the vector so we're not iterating over
    // dead ones every frame (same pattern as the off-screen ninja star cleanup).
    coins.erase(
        std::remove_if(
            coins.begin(),
            coins.end(),
            [](const auto& c) { return c->isCollected(); }),
        coins.end());
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

    for (auto& c : coins)
    {
        c->draw(window);
    }

    player.draw(window);

    // TEMPORARY placeholder until you add a font/sf::Text "You Win" screen
    if (gameWon)
    {
        window.clear(sf::Color(30, 100, 30)); // greenish flash so you can visually confirm the win state works
    }

    // Scoreboard is HUD, not world - draw it with the window's default
    // (unscrolled) view so it stays pinned to the top-left corner instead
    // of scrolling away with the camera.
    window.setView(window.getDefaultView());
    window.draw(scorePanel);
    if (scoreCoin)
    {
        window.draw(*scoreCoin);
    }

        window.draw(scoreText);
    window.setView(view);

    window.display();
}