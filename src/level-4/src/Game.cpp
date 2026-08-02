#include "Game.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "runLevel5.h"

namespace L4 {

Game4::Game4(sf::RenderWindow& win)
    : window(win)
    , deathScreen(SW, SH, "../src/level-1/assets/fonts/Vipnagorgialla Bd.otf")
    , gameSpeed(BASE_SPEED)
    , score(0)
    , coinCount(0)
    , lives(3)
    , gameOver(false)
    , gameWon(false)
    , deathHandled(false)
    , spawnTimer(0)
    , treeTimer(0)
    , deerTimer(0)
    , giantTimer(0)
    , distTimer(0)
{
    window.create(sf::VideoMode({(unsigned)SW, (unsigned)SH}), "Winter Journey");
    window.setFramerateLimit(60);
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    (void)tilemapTex.loadFromFile("../src/level-4/assets/maps/sprite-level4.png");
    tilemapTex.setSmooth(false);

    trees.emplace_back(SW * 0.3f, 1.1f);
    trees.emplace_back(SW * 0.6f, 0.8f);
    trees.emplace_back(SW * 0.9f, 1.0f);

    loadSounds();
}

void Game4::loadSounds() {
    if (jumpBuffer.loadFromFile("../src/level-4/assets/jump.mp3")) jumpSound.emplace(jumpBuffer);
    if (coinBuffer.loadFromFile("../src/level-4/assets/coin.mp3")) coinSound.emplace(coinBuffer);
    if (heartBuffer.loadFromFile("../src/level-4/assets/heartminus.mp3")) heartSound.emplace(heartBuffer);
    if (gameOverBuffer.loadFromFile("../src/level-4/assets/gameover.mp3")) gameOverSound.emplace(gameOverBuffer);
    if (deerBuffer.loadFromFile("../src/level-4/assets/deer.mp3")) deerSound.emplace(deerBuffer);
    if (yetiBuffer.loadFromFile("../src/level-4/assets/yeti.mp3")) yetiSound.emplace(yetiBuffer);
}

bool Game4::run() {
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 1.f / 30.f) dt = 1.f / 30.f;

        processEvents();
        update(dt);
        render();

        if (gameWon) {
            return runLevel5(window);
        }
        if (!window.isOpen()) {
            return false;
        }
    }
    return false;
}

void Game4::processEvents() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }

        if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            if (!gameOver && !gameWon) {
                if (key->code == sf::Keyboard::Key::Space ||
                    key->code == sf::Keyboard::Key::Up ||
                    key->code == sf::Keyboard::Key::W) {
                    if (player.jump() && jumpSound) {
                        jumpSound->play();
                    }
                }
            }
            if (key->code == sf::Keyboard::Key::R && gameOver) {
                player = Player();
                lives = 3;
                score = 0;
                coinCount = 0;
                gameOver = false;
                deathHandled = false;
                coins.clear();
                rocks.clear();
                deerEnemies.clear();
                giants.clear();
                gameSpeed = BASE_SPEED;
                spawnTimer = treeTimer = deerTimer = giantTimer = distTimer = 0;
            }
            if (key->code == sf::Keyboard::Key::Escape) {
                window.close();
            }
        }

        if (const auto* mbp = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mbp->button == sf::Mouse::Button::Left && gameOver && !deathHandled) {
                DeathScreenResult result = deathScreen.getInput(window, mbp->position);
                if (result == DeathScreenResult::Exit) {
                    window.close();
                } else if (result == DeathScreenResult::Restart) {
                    player = Player();
                    lives = 3;
                    score = 0;
                    coinCount = 0;
                    gameOver = false;
                    deathHandled = false;
                    coins.clear();
                    rocks.clear();
                    deerEnemies.clear();
                    giants.clear();
                    gameSpeed = BASE_SPEED;
                    spawnTimer = treeTimer = deerTimer = giantTimer = distTimer = 0;
                }
            }
        }
    }
}

void Game4::update(float dt) {
    if (gameOver || gameWon) return;

    gameSpeed += 2.f * dt;
    if (gameSpeed > 350.f) gameSpeed = 350.f;

    distTimer += dt;
    score += gameSpeed * dt * 0.1f;

    sky.update(dt);
    snowfall.update(dt);
    ground.update(dt, gameSpeed);

    for (auto& t : trees) t.update(dt, gameSpeed);

    spawnObjects(dt);

    for (auto& c : coins) c.update(dt, gameSpeed);
    for (auto& r : rocks) r.update(dt, gameSpeed);
    for (auto& d : deerEnemies) d.update(dt, gameSpeed);
    for (auto& g : giants) g.update(dt, gameSpeed);

    player.update(dt);

    checkCollisions();

    if (gameOver && gameOverSound) {
        gameOverSound->play();
    }

    coins.erase(std::remove_if(coins.begin(), coins.end(),
        [](const Coin& c) { return c.x < -50.f; }), coins.end());
    rocks.erase(std::remove_if(rocks.begin(), rocks.end(),
        [](const Rock& r) { return r.x < -60.f; }), rocks.end());
    deerEnemies.erase(std::remove_if(deerEnemies.begin(), deerEnemies.end(),
        [](const DeerEnemy& d) { return !d.active; }), deerEnemies.end());
    giants.erase(std::remove_if(giants.begin(), giants.end(),
        [](const Giant& g) { return !g.active; }), giants.end());

    if (score >= WIN_SCORE && !gameWon) {
        gameWon = true;
    }
}

void Game4::spawnObjects(float dt) {
    treeTimer += dt;
    if (treeTimer > 2.5f) {
        treeTimer = 0;
        float sc = 0.7f + (std::rand() % 60) / 100.f;
        trees.emplace_back(SW + 80.f, sc);
    }

    spawnTimer += dt;
    float coinInterval = std::max(0.6f, 1.8f - gameSpeed * 0.002f);
    if (spawnTimer > coinInterval) {
        spawnTimer = 0;
        float cx = SW + 30.f;
        float cy = groundYat(cx) - 30.f - (std::rand() % 60);
        coins.emplace_back(cx, cy);
    }

    float rockInterval = std::max(0.8f, 2.5f - gameSpeed * 0.003f);
    if (spawnTimer > rockInterval * 0.5f && (std::rand() % 100) < 2) {
        rocks.emplace_back(SW + 30.f);
    }

    deerTimer += dt;
    float deerInterval = std::max(3.f, 7.f - gameSpeed * 0.005f);
    if (deerTimer > deerInterval) {
        deerTimer = 0;
        if (deerEnemies.empty() || !deerEnemies.back().active) {
            deerEnemies.emplace_back();
            deerEnemies.back().spawn();
            if (deerSound) {
                deerSound->play();
            }
        }
    }

    giantTimer += dt;
    float giantInterval = std::max(5.f, 12.f - gameSpeed * 0.004f);
    if (giantTimer > giantInterval) {
        giantTimer = 0;
        if (giants.empty() || !giants.back().active) {
            giants.emplace_back();
            giants.back().spawn();
        }
    }
}

void Game4::checkCollisions() {
    sf::FloatRect pb = player.bounds();

    for (auto& c : coins) {
        if (c.collected) continue;
        if (pb.findIntersection(c.bounds())) {
            c.collected = true;
            coinCount++;
            score += 50;
            bool yeti = (coinCount % 10 == 5);
            player.setYetiMode(yeti);
            if (coinSound) {
                coinSound->play();
            }
            if (yeti && yetiSound) {
                yetiSound->play();
            }
        }
    }

    for (auto& r : rocks) {
        if (r.counted) continue;
        if (pb.findIntersection(r.bounds())) {
            r.counted = true;
            lives--;
            if (heartSound) {
                heartSound->play();
            }
            player.hit();
            if (lives <= 0) {
                gameOver = true;
                deathHandled = false;
                gameOverScreen.show();
            }
        }
    }

    for (auto& d : deerEnemies) {
        if (!d.active || d.dead) continue;
        if (pb.findIntersection(d.bounds())) {
            d.dead = true;
            d.active = false;
            lives--;
            if (heartSound) {
                heartSound->play();
            }
            player.hit();
            if (lives <= 0) {
                gameOver = true;
                deathHandled = false;
                gameOverScreen.show();
            }
        }
    }

    for (auto& g : giants) {
        if (!g.active) continue;
        if (pb.findIntersection(g.bounds())) {
            lives--;
            if (heartSound) {
                heartSound->play();
            }
            player.hit();
            g.active = false;
            if (lives <= 0) {
                gameOver = true;
                deathHandled = false;
                gameOverScreen.show();
            }
        }
    }
}

void Game4::render() {
    window.clear();
    sky.draw(window);
    snowfall.draw(window);

    {
        sf::Sprite tilemapSpr{tilemapTex};
        tilemapSpr.setScale({32.f, 32.f});
        float mapX = -std::fmod(ground.offset, 3840.f);
        tilemapSpr.setPosition({mapX, 0.f});
        window.draw(tilemapSpr);
        if (mapX < 0.f) {
            tilemapSpr.setPosition({mapX + 3840.f, 0.f});
            window.draw(tilemapSpr);
        }
    }

    ground.draw(window);

    for (auto& t : trees) t.draw(window);
    for (auto& c : coins) c.draw(window);
    for (auto& r : rocks) r.draw(window);
    for (auto& d : deerEnemies) d.draw(window);
    for (auto& g : giants) g.draw(window);
    player.draw(window);

    hud.draw(window, score, coinCount, lives);

    if (gameOver) {
        deathScreen.draw(window);
    }

    window.display();
}

} // namespace L4
