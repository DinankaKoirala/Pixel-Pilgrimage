#include <SFML/Graphics.hpp>
#include <optional>
#include <iostream>
#include <vector>

#include "tilemap.h"
#include "player.h"
#include "enemy.h"
#include "background.h"
#include "coin.h"
#include "runLevel2.h"

int main()
{
    unsigned int width = 1280;
    unsigned int height = 720;

    bool levelComplete = false;

    {
        sf::RenderWindow window(
            sf::VideoMode({ width, height }),
            "Pixel Pilgrimage - Level 1"
        );
        window.setFramerateLimit(60);

        AudioManager audio;
        audio.loadSFX("jump", "../src/Resources/jump.mp3");
        audio.loadSFX("hurt", "../src/Resources/hurt.mp3");
        audio.loadSFX("footstep", "../src/Resources/footstep.mp3");

        Tilemap tilemap;
        if (!tilemap.loadFromImage("../src/Resources/sprite-level1.png")) {
            std::cout << "Failed to load sprite-level1!" << std::endl;
        }
        if (!tilemap.loadTexture("../src/Resources/dirt_tile.png", "Stone")) {
            std::cout << "Failed to load stone texture!" << std::endl;
        }
        if (!tilemap.loadTexture("../src/Resources/grass_tile.png", "Grass")) {
            std::cout << "Failed to load grass texture!" << std::endl;
        }
        if (!tilemap.loadTexture("../src/Resources/platform_tile.jpg", "Obstacle")) {
            std::cout << "Failed to load obstacle texture!" << std::endl;
        }

        std::vector<sf::FloatRect> solids = tilemap.getSolidTiles();

        sf::Vector2f playerSpawn = tilemap.getPlayerSpawnPoint();
        Player player(playerSpawn.x, playerSpawn.y, audio);
        if (!player.loadTextures()) {
            std::cout << "Failed to load player textures!" << std::endl;
        }

        sf::Vector2f levelEndPos = tilemap.getLevelEndPoint();
        float triggerX;
        if (levelEndPos.x == 100.f && levelEndPos.y == 100.f) {
            triggerX = 120 * 32 - 64;
            std::cout << "No LevelEnd tile found, fallback trigger at x=" << triggerX << std::endl;
        } else {
            triggerX = levelEndPos.x;
            std::cout << "LevelEnd tile at x=" << levelEndPos.x << " y=" << levelEndPos.y << std::endl;
        }

        bool playerAlive = true;
        sf::Clock clock;
        sf::View camera(sf::FloatRect({0.f, 0.f}, {1280.f, 720.f}));
        sf::Clock levelCompleteClock;

        std::vector<sf::Vector2f> enemySpawns = tilemap.getEnemySpawnPoints();
        std::vector<Enemy> enemies;
        for (const sf::Vector2f& enemyPos : enemySpawns) {
            Enemy enemy(enemyPos.x, enemyPos.y);
            enemies.push_back(enemy);
        }
        for (Enemy& enemy : enemies) {
            if (!enemy.loadTextures("../src/Resources/rhino_sheet.png")) {
                std::cout << "Failed to load enemy texture!" << std::endl;
            }
        }

        std::vector<sf::Vector2f> coinSpawns = tilemap.getCoinSpawnPoints();
        std::vector<Coin> coins;
        for (const sf::Vector2f& coinPos : coinSpawns) {
            Coin coin(coinPos.x, coinPos.y);
            coins.push_back(coin);
        }
        for (Coin& coin : coins) {
            if (!coin.loadTextures("../src/Resources/collectible.png")) {
                std::cout << "Failed to load coin texture!" << std::endl;
            }
        }

        Background background;
        background.loadTexture("../src/Resources/background.png");

        sf::Font completeFont;
        bool fontLoaded = completeFont.openFromFile("/System/Library/Fonts/Helvetica.ttc");

        bool transitionReady = false;

        while (window.isOpen() && !transitionReady)
        {
            while (const std::optional event = window.pollEvent())
            {
                if (event->is<sf::Event::Closed>())
                    window.close();
            }

            float dt = clock.restart().asSeconds();

            if (!levelComplete && playerAlive) {
                player.handleInput();
                player.update(dt, solids);

                for (Enemy& enemy : enemies) {
                    enemy.update(dt, solids);
                }

                for (Enemy& enemy : enemies) {
                    if (auto overlap = player.getPlayerHitbox().findIntersection(enemy.getEnemyHitbox())) {
                        playerAlive = false;
                        audio.playSFX("hurt");
                    }
                    if (player.getPosition().y + 32 >= 720) {
                        playerAlive = false;
                        audio.playSFX("hurt");
                    }
                }

                for (Coin& coin : coins) {
                    if (auto overlap = player.getPlayerHitbox().findIntersection(coin.getCoinHitbox())) {
                        coin.collect();
                        audio.playSFX("hurt");
                    }
                }

                sf::FloatRect playerBox = player.getPlayerHitbox();
                float playerRight = playerBox.position.x + playerBox.size.x;
                if (playerRight >= triggerX) {
                    levelComplete = true;
                    levelCompleteClock.restart();
                }
            }

            if (!levelComplete &&
                sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LControl) &&
                sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::R)) {
                player.reset(playerSpawn.x, playerSpawn.y);
                for (size_t i = 0; i < enemies.size(); i++) {
                    enemies[i].reset(enemySpawns[i].x, enemySpawns[i].y);
                }
                playerAlive = true;
                for (size_t i = 0; i < coins.size(); i++) {
                    coins[i].reset(coinSpawns[i].x, coinSpawns[i].y);
                }
            }

            if (player.getPosition().x <= window.getSize().x / 2.f) {
                camera.setCenter({1280 / 2.f, 360.f});
            } else {
                camera.setCenter({player.getPosition().x, 360.f});
            }
            window.setView(camera);

            window.clear();
            background.draw(window, camera.getCenter().x);
            tilemap.draw(window);

            if (!levelComplete) {
                player.draw(window);

                for (Enemy& enemy : enemies) {
                    enemy.draw(window);
                }
                for (Coin& coin : coins) {
                    if (!coin.isCollected()) {
                        coin.draw(window);
                    }
                }
            } else {
                sf::RectangleShape overlay({1280.f, 720.f});
                overlay.setFillColor(sf::Color(0, 0, 0, 200));
                overlay.setPosition({camera.getCenter().x - 640.f, 0.f});
                window.draw(overlay);

                if (fontLoaded) {
                    sf::Text t(completeFont, "LEVEL COMPLETE!", 48);
                    sf::FloatRect b = t.getLocalBounds();
                    t.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
                    t.setPosition({camera.getCenter().x, 330.f});
                    t.setFillColor(sf::Color(100, 255, 100));
                    window.draw(t);

                    sf::Text s(completeFont, "Loading next level...", 22);
                    sf::FloatRect sb = s.getLocalBounds();
                    s.setOrigin({sb.size.x / 2.f, sb.size.y / 2.f});
                    s.setPosition({camera.getCenter().x, 410.f});
                    s.setFillColor(sf::Color(200, 200, 200));
                    window.draw(s);
                }

                if (levelCompleteClock.getElapsedTime().asSeconds() >= 1.5f) {
                    transitionReady = true;
                }
            }

            window.display();
        }
    }

    if (levelComplete) {
        runLevel2();
    }

    return 0;
}
