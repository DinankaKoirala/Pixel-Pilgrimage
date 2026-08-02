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
#include "runLevel3.h"
#include "runLevel4.h"
#include "runLevel5.h"
#include "audioManager.h"
#include "death-screen/DeathScreen.h"
#include "start-screen/StartScreen.h"
#include "map/MapScreen.h"
#include "GameSettings.h"
#include "MenuMusic.h"

static bool runLevel1(sf::RenderWindow& window)
{
    unsigned int width = 1280;
    unsigned int height = 720;

    bool levelComplete = false;

    AudioManager audio;
    audio.loadSFX("jump", "../src/level-1/assets/sounds/jump.wav");
    audio.loadSFX("hurt", "../src/level-1/assets/sounds/hurt.wav");
    audio.loadSFX("coin", "../src/level-1/assets/sounds/coincollect.wav");
    audio.loadSFX("footstep", "../src/level-1/assets/sounds/footstep.mp3");

    Tilemap tilemap;
    if (!tilemap.loadFromImage("../src/level-1/assets/maps/sprite-level1.png")) {
        std::cout << "Failed to load sprite-level1!" << std::endl;
    }
    if (!tilemap.loadTexture("../src/level-1/assets/textures/dirt_tile.png", "Stone")) {
        std::cout << "Failed to load stone texture!" << std::endl;
    }
    if (!tilemap.loadTexture("../src/level-1/assets/textures/grass_tile.png", "Grass")) {
        std::cout << "Failed to load grass texture!" << std::endl;
    }
    if (!tilemap.loadTexture("../src/level-1/assets/textures/platform_tile.jpg", "Obstacle")) {
        std::cout << "Failed to load obstacle texture!" << std::endl;
    }
    if (!tilemap.loadTexture("../src/level-1/assets/textures/bear_trap.png", "Trap")) {
        std::cout << "Failed to load trap texture!" << std::endl;
    }

    std::vector<sf::FloatRect> solids = tilemap.getSolidTiles();
    std::vector<sf::FloatRect> traps = tilemap.getTrapHitboxes();

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
    sf::Clock fpsClock;
    sf::View camera(sf::FloatRect({0.f, 0.f}, {1280.f, 720.f}));
    std::vector<sf::Vector2f> enemySpawns = tilemap.getEnemySpawnPoints();
    std::vector<Enemy> enemies;
    for (const sf::Vector2f& enemyPos : enemySpawns) {
        Enemy enemy(enemyPos.x, enemyPos.y);
        enemy.setSpeed(20.f * GameSettings::get().difficultyMultiplier());
        enemies.push_back(enemy);
    }
    for (Enemy& enemy : enemies) {
        if (!enemy.loadTextures("../src/level-1/assets/textures/rhino_sheet.png")) {
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
        if (!coin.loadTextures("../src/level-1/assets/textures/collectible.png")) {
            std::cout << "Failed to load coin texture!" << std::endl;
        }
    }

    Background background;
    background.loadTexture("../src/level-1/assets/textures/background.png");

    std::vector<Entity*> entities;
    entities.reserve(1 + enemies.size() + coins.size());
    entities.push_back(&player);
    for (Enemy& enemy : enemies) {
        entities.push_back(&enemy);
    }
    for (Coin& coin : coins) {
        entities.push_back(&coin);
    }

    std::vector<Drawable*> scene = { &background, &tilemap };
    std::vector<Drawable*> sceneEntities;
    sceneEntities.reserve(entities.size());
    for (Entity* entity : entities) {
        sceneEntities.push_back(entity);
    }

    const int requiredCoins = 7;

    sf::Font scoreFont;
    if (!scoreFont.openFromFile("../src/level-1/assets/fonts/Helvetica.ttf")) {
        std::cerr << "Failed to load score font!" << std::endl;
    }
    sf::Text scoreText(scoreFont, "COINS: 0/7", 28);
    scoreText.setFillColor(sf::Color(255, 215, 0));
    scoreText.setOutlineColor(sf::Color::Black);
    scoreText.setOutlineThickness(2.f);
    scoreText.setPosition({ 1085.f, 18.f });

    sf::RectangleShape scorePanel({ 250.f, 65.f });
    scorePanel.setPosition({ 1015.f, 10.f });
    scorePanel.setFillColor(sf::Color(0, 0, 0, 170));
    scorePanel.setOutlineThickness(3.f);
    scorePanel.setOutlineColor(sf::Color(255, 215, 0));

    sf::Texture uiCoinTex;
    if (!uiCoinTex.loadFromFile("../src/level-1/assets/textures/collectible.png")) {
        std::cout << "Failed to load ui coin texture!" << std::endl;
    }
    sf::Sprite scoreCoin(uiCoinTex);
    scoreCoin.setTextureRect(sf::IntRect({0, 0}, {32, 32}));
    scoreCoin.setPosition({ 1035.f, 25.f });

    sf::Texture heartTex;
    if (!heartTex.loadFromFile("../src/level-1/assets/textures/heart.png")) {
        std::cout << "Failed to load heart texture!" << std::endl;
    }
    sf::Sprite heartSprite(heartTex);
    heartSprite.setPosition({ 20.f, 20.f });

    DeathScreen deathScreen(1280.f, 720.f, "../src/level-1/assets/fonts/Vipnagorgialla Bd.otf");

    sf::Font completeFont;
    bool fontLoaded = completeFont.openFromFile("../src/level-1/assets/fonts/Helvetica.ttf");
    sf::Clock levelCompleteClock;
    bool transitionReady = false;

    auto resetLevel = [&]() {
        player.reset(playerSpawn.x, playerSpawn.y);
        for (size_t i = 0; i < enemies.size(); i++) {
            enemies[i].reset(enemySpawns[i].x, enemySpawns[i].y);
        }
        for (size_t i = 0; i < coins.size(); i++) {
            coins[i].reset(coinSpawns[i].x, coinSpawns[i].y);
        }
        Coin::coinsCollected = 0;
        playerAlive = true;
        levelComplete = false;
        transitionReady = false;
    };

    while (window.isOpen() && !transitionReady)
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto* mouseclick = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (!playerAlive && mouseclick->button == sf::Mouse::Button::Left) {
                    DeathScreenResult result = deathScreen.getInput(window, mouseclick->position);
                    if (result == DeathScreenResult::Exit) {
                        window.close();
                    } else                     if (result == DeathScreenResult::Restart) {
                        resetLevel();
                    }
                }
            }
        }

        float dt = clock.restart().asSeconds();
        SettingsFX::tick(fpsClock.restart().asSeconds());

        if (!levelComplete && playerAlive) {
            player.handleInput();

            for (Entity* entity : entities) {
                entity->update(dt, solids);
            }

            for (Enemy& enemy : enemies) {
                if (auto overlap = player.getHitbox().findIntersection(enemy.getHitbox())) {
                    playerAlive = false;
                    audio.playSFX("hurt");
                }
            }

            if (player.getPosition().y + 32 >= 720) {
                playerAlive = false;
                audio.playSFX("hurt");
            }
            for (const sf::FloatRect& trap : traps) {
                if (auto overlap = player.getHitbox().findIntersection(trap)) {
                    playerAlive = false;
                    audio.playSFX("hurt");
                }
            }

            for (Coin& coin : coins) {
                if (!coin.isCollected()) {
                    if (auto overlap = player.getHitbox().findIntersection(coin.getHitbox())) {
                        coin.collect();
                        audio.playSFX("coin");
                    }
                }
            }

            int score = Coin::coinsCollected;
            scoreText.setString("COINS: " + std::to_string(score) + "/" + std::to_string(requiredCoins));

            sf::FloatRect playerBox = player.getHitbox();
            float playerRight = playerBox.position.x + playerBox.size.x;
            if (playerRight >= triggerX) {
                levelComplete = true;
                levelCompleteClock.restart();
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LControl) &&
            sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::R)) {
            if (!levelComplete || Coin::coinsCollected < requiredCoins) {
                resetLevel();
            }
        }

        if (player.getPosition().x <= 640.f) {
            camera.setCenter({640.f, 360.f});
        } else {
            camera.setCenter({player.getPosition().x, 360.f});
        }
        window.setView(camera);

        window.clear();
        background.setCameraPos(camera.getCenter().x);
        for (Drawable* drawable : scene) {
            drawable->draw(window);
        }

        if (!levelComplete) {
            for (Drawable* drawable : sceneEntities) {
                drawable->draw(window);
            }
        }

        if (!levelComplete && playerAlive) {
            window.setView(window.getDefaultView());
            window.draw(scorePanel);
            window.draw(scoreCoin);
            window.draw(scoreText);
            window.draw(heartSprite);
            window.setView(camera);
        } else if (!levelComplete && !playerAlive) {
            window.setView(window.getDefaultView());
            deathScreen.draw(window);
            window.setView(camera);
        } else {
            sf::RectangleShape overlay({1280.f, 720.f});
            overlay.setFillColor(sf::Color(0, 0, 0, 200));
            overlay.setPosition({camera.getCenter().x - 640.f, 0.f});
            window.draw(overlay);

            if (fontLoaded) {
                if (Coin::coinsCollected < requiredCoins) {
                    sf::Text t(completeFont, "NOT ENOUGH COINS!", 48);
                    sf::FloatRect b = t.getLocalBounds();
                    t.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
                    t.setPosition({camera.getCenter().x, 290.f});
                    t.setFillColor(sf::Color(255, 100, 100));
                    window.draw(t);

                    sf::Text req(completeFont, "MINIMUM COINS REQUIRED: " + std::to_string(requiredCoins), 26);
                    sf::FloatRect rb = req.getLocalBounds();
                    req.setOrigin({rb.size.x / 2.f, rb.size.y / 2.f});
                    req.setPosition({camera.getCenter().x, 380.f});
                    req.setFillColor(sf::Color(255, 215, 0));
                    window.draw(req);

                    sf::Text got(completeFont, "COINS COLLECTED: " + std::to_string(Coin::coinsCollected), 26);
                    sf::FloatRect gb = got.getLocalBounds();
                    got.setOrigin({gb.size.x / 2.f, gb.size.y / 2.f});
                    got.setPosition({camera.getCenter().x, 420.f});
                    got.setFillColor(sf::Color(200, 200, 200));
                    window.draw(got);

                    sf::Text resetHint(completeFont, "Press Ctrl+R to Restart", 22);
                    sf::FloatRect hb = resetHint.getLocalBounds();
                    resetHint.setOrigin({hb.size.x / 2.f, hb.size.y / 2.f});
                    resetHint.setPosition({camera.getCenter().x, 480.f});
                    resetHint.setFillColor(sf::Color(255, 255, 255));
                    window.draw(resetHint);
                } else {
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
            }

            if (Coin::coinsCollected >= requiredCoins &&
                levelCompleteClock.getElapsedTime().asSeconds() >= 1.5f) {
                transitionReady = true;
            }
        }

        SettingsFX::draw(window);
        window.display();
    }

    return levelComplete;
}

int main()
{
    GameSettings& settings = GameSettings::get();
    sf::RenderWindow window(
        settings.windowVideoMode(),
        "Pixel Pilgrimage",
        settings.windowState()
    );
    window.setFramerateLimit(60);

    StartScreen startScreen(1280.f, 720.f, "../src/shared/start-screen/");
    MapScreen mapScreen(1280.f, 720.f, "../src/shared/map/");

    while (window.isOpen())
    {
        MenuMusic::play();

        StartScreenResult sr = startScreen.run(window);
        if (sr == StartScreenResult::Exit) break;

        if (sr == StartScreenResult::StartGame)
        {
            MenuMusic::stop();
            bool levelComplete = runLevel1(window);

            if (levelComplete)
            {
                runLevel2(window);
            }

            window.create(settings.windowVideoMode(), "Pixel Pilgrimage", settings.windowState());
            window.setFramerateLimit(60);
        }
        else if (sr == StartScreenResult::ShowMap)
        {
            MapScreenResult mr = mapScreen.run(window);
            if (mr == MapScreenResult::Exit) break;

            if (mr == MapScreenResult::Level1)
            {
                MenuMusic::stop();
                runLevel1(window);
            }
            else if (mr == MapScreenResult::Level2)
            {
                MenuMusic::stop();
                runLevel2(window);
            }
            else if (mr == MapScreenResult::Level3)
            {
                MenuMusic::stop();
                runLevel3(window);
            }
            else if (mr == MapScreenResult::Level4)
            {
                MenuMusic::stop();
                runLevel4(window);
            }
            else if (mr == MapScreenResult::Level5)
            {
                MenuMusic::stop();
                runLevel5(window);
            }

            window.create(settings.windowVideoMode(), "Pixel Pilgrimage", settings.windowState());
            window.setFramerateLimit(60);
        }
    }

    return 0;
}
