#include "Game.hpp"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <string>
#include <iostream>
#include "Header/ParticleSystem.hpp"
#include "Header/Aura.hpp"
#include "../shared/win/WinScreen.h"
#include "GameSettings.h"

namespace L5 {

static sf::FloatRect getPlayerHitbox(const sf::Sprite& sprite) {
    sf::FloatRect local = sprite.getLocalBounds();
    float trimLeft = local.size.x * 0.30f;
    float trimRight = local.size.x * 0.30f;
    float trimTop = local.size.y * 0.30f;
    float trimBottom = local.size.y * 0.3f;
    sf::FloatRect shrunk(
        { local.position.x + trimLeft, local.position.y + trimTop },
        { local.size.x - trimLeft - trimRight, local.size.y - trimTop - trimBottom }
    );
    return sprite.getTransform().transformRect(shrunk);
}

Game5::Game5(sf::RenderWindow& win)
    : window(win)
    , gameOver(false)
    , gameWon(false)
    , deathHandled(false)
    , background(bgTexture)
    , player(playerTexture)
    , orb(orbTexture)
    , sx(1.f), sy(1.f)
    , winL(800), winB(600)
    , playerFrameW(0), playerFrameH(0)
    , playerAnimFrame(0), playerAnimTimer(0.f), playerFacingLeft(false)
    , playerSpeed(300.f), velocityY(0.f)
    , gravity(900.f), jumpStrength(-450.f), onGround(true)
    , fireballSpeed(250.f), spawnTimer(0.f), spawnInterval(0.1f)
    , ambientTime(0.f), bgOverscan(1.06f)
    , survivalTimeRemaining(20.f), survivalDuration(20.f)
    , monkHealthPercent(100)
    , maxLives(GameSettings::get().livesForDifficulty(4))
    , remainingLives(GameSettings::get().livesForDifficulty(4))
    , invincibilityTimer(0.f)
    , monkSprite(monkTexture)
    , monkBasePos(400.f, 200.f), monkPos(400.f, 200.f)
    , monkMovementTime(0.f)
    , monkPatrolAmplitude(100.f), monkPatrolSpeed(0.6f)
    , monkHoverAmplitude(15.f), monkHoverSpeed(1.4f)
    , monkSpawnTimer(0.f), monkSpawnInterval(0.7f)
    , monkSpellSpeed(400.f), monkSpellScale(0.3f)
    , dripstoneSpawnTimer(0.f), dripstoneSpawnInterval(1.5f)
    , dripstoneMinX(50.f), dripstoneMaxX(750.f)
    , dripstoneCeilingY(0.f), dripstoneGroundY(400.f)
    , fontLoaded(false)
    , hitSound(hitBuffer)
    , whooshSound(whooshBuffer)
    , laughSound(laughBuffer)
    , laughTimer(8.f)
{
    const GameSettings& settings = GameSettings::get();
    window.create(settings.windowVideoMode(), "Level 5 - Boss Fight", settings.windowState());
    window.setFramerateLimit(60);
    window.setView(window.getDefaultView());

    winL = window.getSize().x;
    winB = window.getSize().y;
    sx = static_cast<float>(winL) / 800.f;
    sy = static_cast<float>(winB) / 600.f;

    if (settings.difficulty == 0)      survivalDuration = 25.f;
    else if (settings.difficulty == 2) survivalDuration = 15.f;
    else                               survivalDuration = 20.f;
    survivalTimeRemaining = survivalDuration;

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    loadAssets();
    initPlayer();
    initMonk();
    initOrb();
}

void Game5::loadAssets() {
    const std::string base = "../src/level-5/assets/";

    (void)bgTexture.loadFromFile(base + "textures/background.png");
    background.setTexture(bgTexture, true);
    sf::Vector2u texSize = bgTexture.getSize();
    sf::Vector2u wSize = window.getSize();
    background.setScale({
        bgOverscan * static_cast<float>(wSize.x) / texSize.x,
        bgOverscan * static_cast<float>(wSize.y) / texSize.y
    });
    bgMargin = {
        static_cast<float>(wSize.x) * (bgOverscan - 1.f) * 0.5f,
        static_cast<float>(wSize.y) * (bgOverscan - 1.f) * 0.5f
    };

    (void)playerTexture.loadFromFile(base + "textures/prince.png");
    player.setTexture(playerTexture, true);
    (void)orbTexture.loadFromFile(base + "textures/orb.png");
    orb.setTexture(orbTexture, true);
    (void)fireballTexture.loadFromFile(base + "textures/fireball.png");
    (void)monkTexture.loadFromFile(base + "textures/monk.png");
    monkSprite.setTexture(monkTexture, true);
    (void)spellTexture.loadFromFile(base + "textures/fireball.png");

    fontLoaded = font.openFromFile(base + "fonts/OptimusPrinceps.ttf");
    if (!fontLoaded)
        fontLoaded = font.openFromFile(base + "fonts/Roboto-Medium.ttf");

    (void)hitBuffer.loadFromFile(base + "sounds/hit.wav");
    (void)whooshBuffer.loadFromFile(base + "sounds/whoosh.wav");
    (void)laughBuffer.loadFromFile(base + "sounds/evil-laugh.wav");
    (void)bgm.openFromFile(base + "sounds/bgm.mp3");
    bgm.setLooping(true);
    bgm.setVolume(70.f * GameSettings::get().musicScale());
    bgm.play();

    hitSound.setBuffer(hitBuffer);
    whooshSound.setBuffer(whooshBuffer);
    laughSound.setBuffer(laughBuffer);
    float sfxScale = GameSettings::get().sfxScale();
    hitSound.setVolume(100.f * sfxScale);
    whooshSound.setVolume(10.f * sfxScale);
    laughSound.setVolume(500.f * sfxScale);
}

void Game5::initPlayer() {
    sf::Vector2u princeTexSize = playerTexture.getSize();
    playerFrameW = static_cast<int>(princeTexSize.x) / playerFrameCols;
    playerFrameH = static_cast<int>(princeTexSize.y) / playerFrameRows;
    playerFrameCount = playerFrameCols * playerFrameRows;
    player.setTextureRect(sf::IntRect({0, 0}, {playerFrameW, playerFrameH}));
    player.setPosition({0.f, groundY()});
    player.setScale({0.25f * sx, 0.25f * sy});
    playerSpeed = 300.f * sx;
    gravity = 900.f * sy;
    jumpStrength = -450.f * sy;
}

void Game5::initMonk() {
    sf::Vector2u monkTexSize = monkTexture.getSize();
    monkSprite.setOrigin({monkTexSize.x / 2.f, monkTexSize.y / 2.f});
    monkSprite.setScale({0.7f, 0.7f});
    monkBasePos = {400.f * sx, 200.f * sy};
    monkPos = monkBasePos;
    monkSprite.setPosition(monkPos);
    monkPatrolAmplitude = 100.f * sx;
    monkHoverAmplitude = 15.f * sy;
    monkSpellSpeed = 400.f * sx;
    monkSpellScale = 0.3f;
}

void Game5::initOrb() {
    sf::Vector2u orbTexSize = orbTexture.getSize();
    orb.setOrigin({orbTexSize.x / 2.f, orbTexSize.y / 2.f});
    const float orbDesignDiameter = 400.f;
    orb.setScale({
        (orbDesignDiameter * sx) / orbTexSize.x,
        (orbDesignDiameter * sy) / orbTexSize.y
    });
    orb.setPosition({400.f * sx, 100.f * sy});

    sf::Vector2u fbTexSize = fireballTexture.getSize();
    fireballOrigin = {fbTexSize.x / 2.f, fbTexSize.y / 2.f};
    const float fbDesignDiameter = 16.f;
    fireballScale = {
        (fbDesignDiameter * sx) / fbTexSize.x,
        (fbDesignDiameter * sy) / fbTexSize.y
    };
    fireballSpeed = 250.f * sx;

    dripstoneMinX = 50.f * sx;
    dripstoneMaxX = 750.f * sx;
    dripstoneGroundY = groundY();
}

bool Game5::run() {
    sf::Clock clock;
    sf::Clock fpsClock;
    Aura staffAura(sf::Color(190, 120, 255), 16.f * sx);

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        SettingsFX::tick(fpsClock.restart().asSeconds());
        if (dt > 1.f / 30.f) dt = 1.f / 30.f;

        ambientTime += dt;

        processEvents();
        update(dt);
        staffAura.update(dt, monkPos + sf::Vector2f(staffOffsetLocal.x * monkSprite.getScale().x, staffOffsetLocal.y * monkSprite.getScale().y));
        render();
        window.draw(staffAura);
        SettingsFX::draw(window);
        window.display();

        if (gameWon && !deathHandled) {
            deathHandled = true;
            window.display();
            sf::sleep(sf::seconds(2.f));
            WinScreen winScreen(winL_f(), winB_f(), "../src/level-1/assets/fonts/Vipnagorgialla Bd.otf");
            WinScreenResult wr = winScreen.run(window);
            if (wr == WinScreenResult::Exit) {
                window.close();
                return false;
            }
            return true;
        }

        if (gameOver && !gameWon && !deathHandled) {
            deathHandled = true;
        }
    }
    return false;
}

void Game5::processEvents() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        if (gameOver) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R) &&
                (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
                 sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl))) {
                resetGame();
            }
        }
    }
}

void Game5::update(float dt) {
    if (!gameOver) {
        updatePlayer(dt);
        updateFireballs(dt);
        updateMonk(dt);
        updateDripstones(dt);
        updateSound(dt);

        survivalTimeRemaining -= dt;
        monkHealthPercent = static_cast<int>(100.f * (survivalTimeRemaining / survivalDuration));
        if (monkHealthPercent < 0) monkHealthPercent = 0;

        if (invincibilityTimer > 0.f) invincibilityTimer -= dt;

        checkCollisions();

        if (survivalTimeRemaining <= 0.f) {
            survivalTimeRemaining = 0.f;
            gameOver = true;
            gameWon = true;
            deathHandled = false;
        }
        if (remainingLives <= 0) {
            gameOver = true;
            gameWon = false;
            deathHandled = false;
        }
    }
}

void Game5::updatePlayer(float dt) {
    sf::Vector2f pos = player.getPosition();
    bool movingLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
                      sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
    bool movingRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
                       sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);
    if (movingLeft) pos.x -= playerSpeed * dt;
    if (movingRight) pos.x += playerSpeed * dt;
    if (pos.x < 0) pos.x = 0;
    if (pos.x > 760.f * sx) pos.x = 760.f * sx;

    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
         sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) && onGround) {
        velocityY = jumpStrength;
        onGround = false;
    }
    velocityY += gravity * dt;
    pos.y += velocityY * dt;
    if (pos.y >= groundY()) {
        pos.y = groundY();
        velocityY = 0;
        onGround = true;
    }
    player.setPosition(pos);

    bool isMoving = movingLeft || movingRight;
    if (isMoving) {
        playerAnimTimer += dt;
        while (playerAnimTimer >= playerAnimFrameDuration) {
            playerAnimTimer -= playerAnimFrameDuration;
            playerAnimFrame = (playerAnimFrame + 1) % playerFrameCount;
        }
    } else {
        playerAnimFrame = 0;
        playerAnimTimer = 0.f;
    }

    int col = playerAnimFrame % playerFrameCols;
    int row = playerAnimFrame / playerFrameCols;
    int rectX = col * playerFrameW;
    int rectY = row * playerFrameH;

    if (movingLeft && !movingRight) playerFacingLeft = true;
    else if (movingRight && !movingLeft) playerFacingLeft = false;

    if (playerFacingLeft) {
        player.setTextureRect(sf::IntRect({rectX + playerFrameW, rectY}, {-playerFrameW, playerFrameH}));
    } else {
        player.setTextureRect(sf::IntRect({rectX, rectY}, {playerFrameW, playerFrameH}));
    }
}

void Game5::updateFireballs(float dt) {
    spawnTimer += dt;
    if (spawnTimer >= spawnInterval) {
        spawnTimer = 0.f;
        float angle = static_cast<float>(std::rand() % 360) * 3.14159265f / 180.f;
        sf::Vector2f vel = {std::cos(angle) * fireballSpeed, std::sin(angle) * fireballSpeed};

        fireballs.emplace_back(fireballTexture);
        auto& fb = fireballs.back();
        fb.shape.setOrigin(fireballOrigin);
        fb.shape.setScale(fireballScale);
        fb.shape.setPosition(orb.getPosition());
        fb.velocity = vel;

        whooshSound.play();
    }

    for (auto& fb : fireballs) {
        fb.shape.move(fb.velocity * dt);
    }

    fireballs.erase(
        std::remove_if(fireballs.begin(), fireballs.end(),
            [this](const Fireball& fb) {
                sf::Vector2f p = fb.shape.getPosition();
                return p.x < -50.f || p.x > winL_f() + 50.f ||
                       p.y < -50.f || p.y > winB_f() + 50.f;
            }),
        fireballs.end());
}

void Game5::updateMonk(float dt) {
    monkMovementTime += dt;
    sf::Vector2f offset(
        std::sin(monkMovementTime * monkPatrolSpeed) * monkPatrolAmplitude,
        std::sin(monkMovementTime * monkHoverSpeed + 1.57f) * monkHoverAmplitude
    );
    monkPos = monkBasePos + offset;
    monkSprite.setPosition(monkPos);

    monkSpawnTimer += dt;
    if (monkSpawnTimer >= monkSpawnInterval) {
        monkSpawnTimer = 0.f;
        sf::Vector2f spawnPos = monkPos + sf::Vector2f(staffOffsetLocal.x * monkSprite.getScale().x, staffOffsetLocal.y * monkSprite.getScale().y);
        sf::Vector2f dir = player.getPosition() - spawnPos;
        float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (length > 0.f) {
            dir /= length;

            monkSpells.emplace_back(spellTexture);
            auto& ms = monkSpells.back();
            sf::Vector2u texSize = spellTexture.getSize();
            ms.shape.setOrigin({texSize.x / 2.f, texSize.y / 2.f});
            ms.shape.setPosition(spawnPos);
            ms.shape.setScale({monkSpellScale, monkSpellScale});
            float angle = std::atan2(dir.y, dir.x) * 180.f / 3.14159265f;
            ms.shape.setRotation(sf::degrees(angle + 90.f));
            ms.velocity = dir * monkSpellSpeed;
        }
    }

    for (auto& s : monkSpells) {
        s.shape.move(s.velocity * dt);
    }
    monkSpells.erase(
        std::remove_if(monkSpells.begin(), monkSpells.end(),
            [this](const MonkSpell& s) {
                sf::Vector2f p = s.shape.getPosition();
                return p.x < -60.f || p.x > winL_f() + 60.f ||
                       p.y < -60.f || p.y > winB_f() + 60.f;
            }),
        monkSpells.end());
}

void Game5::updateDripstones(float dt) {
    dripstoneSpawnTimer += dt;
    if (dripstoneSpawnTimer >= dripstoneSpawnInterval) {
        dripstoneSpawnTimer = 0.f;
        float span = dripstoneMaxX - dripstoneMinX;
        float x = dripstoneMinX + static_cast<float>(std::rand() % static_cast<int>(span > 0.f ? span : 1.f));

        dripstones.emplace_back();
        auto& entry = dripstones.back();
        entry.size = {20.f, 40.f};
        entry.state = DripstoneEntry::Telegraphing;
        entry.telegraphTimer = 0.5f;
        entry.fallSpeed = 0.f;
        entry.groundY = dripstoneGroundY;

        entry.shape.setPointCount(3);
        entry.shape.setPoint(0, {0.f, 0.f});
        entry.shape.setPoint(1, {entry.size.x, 0.f});
        entry.shape.setPoint(2, {entry.size.x / 2.f, entry.size.y});
        entry.shape.setFillColor(sf::Color(150, 150, 150));
        entry.shape.setPosition({x, dripstoneCeilingY});

        entry.warningShape.setPointCount(3);
        entry.warningShape.setPoint(0, {0.f, 0.f});
        entry.warningShape.setPoint(1, {entry.size.x, 0.f});
        entry.warningShape.setPoint(2, {entry.size.x / 2.f, entry.size.y});
        entry.warningShape.setFillColor(sf::Color(255, 60, 60, 90));
        entry.warningShape.setPosition({x, dripstoneCeilingY});
    }

    for (auto& e : dripstones) {
        if (e.state == DripstoneEntry::Telegraphing) {
            e.telegraphTimer -= dt;
            if (e.telegraphTimer <= 0.f) e.state = DripstoneEntry::Falling;
        } else if (e.state == DripstoneEntry::Falling) {
            e.fallSpeed += 1500.f * dt;
            e.shape.move({0.f, e.fallSpeed * dt});
            sf::Vector2f pos = e.shape.getPosition();
            if (pos.y + e.size.y >= e.groundY) {
                e.shape.setPosition({pos.x, e.groundY - e.size.y});
                e.state = DripstoneEntry::Landed;
            }
        }
    }

    dripstones.erase(
        std::remove_if(dripstones.begin(), dripstones.end(),
            [](const DripstoneEntry& e) {
                return e.state == DripstoneEntry::Landed;
            }),
        dripstones.end());
}

void Game5::updateSound(float dt) {
    laughTimer -= dt;
    if (laughTimer <= 0.f) {
        laughSound.stop();
        laughSound.play();
        laughTimer = 8.f;
    }
}

void Game5::checkCollisions() {
    sf::FloatRect playerBounds = getPlayerHitbox(player);
    bool hit = false;

    for (auto& fb : fireballs) {
        if (playerBounds.findIntersection(fb.shape.getGlobalBounds())) {
            hit = true;
            break;
        }
    }

    if (!hit) {
        for (const auto& e : dripstones) {
            if (e.state == DripstoneEntry::Falling &&
                e.shape.getGlobalBounds().findIntersection(playerBounds)) {
                hit = true;
                break;
            }
        }
    }

    if (!hit) {
        for (auto& s : monkSpells) {
            if (playerBounds.findIntersection(s.shape.getGlobalBounds())) {
                hit = true;
                break;
            }
        }
    }

    if (hit && invincibilityTimer <= 0.f) {
        remainingLives--;
        invincibilityTimer = 1.5f;
    }
}

void Game5::render() {
    float bgOffsetX = std::sin(ambientTime * 0.0f) * bgMargin.x;
    float bgOffsetY = std::cos(ambientTime * 0.11f) * (bgMargin.y * 0.5f);
    background.setPosition({-bgMargin.x + bgOffsetX, -bgMargin.y + bgOffsetY});

    window.clear(sf::Color(30, 30, 40));
    window.draw(background);
    window.draw(orb);

    if (!gameOver) {
        window.draw(player);
    }

    for (auto& e : dripstones) {
        if (e.state == DripstoneEntry::Telegraphing) {
            window.draw(e.warningShape);
        } else {
            window.draw(e.shape);
        }
    }

    window.draw(monkSprite);

    for (auto& fb : fireballs) {
        window.draw(fb.shape);
    }
    for (auto& s : monkSpells) {
        window.draw(s.shape);
    }

    // Health bar
    sf::RectangleShape bgBar({winL_f() * 0.8f, 20.f});
    bgBar.setPosition({winL_f() * 0.1f, 40.f});
    bgBar.setFillColor(sf::Color(30, 30, 30));
    bgBar.setOutlineColor(sf::Color(100, 100, 100));
    bgBar.setOutlineThickness(2.f);
    window.draw(bgBar);

    float ratio = monkHealthPercent / 100.f;
    sf::RectangleShape fgBar({winL_f() * 0.8f * ratio, 20.f});
    fgBar.setPosition({winL_f() * 0.1f, 40.f});
    fgBar.setFillColor(sf::Color(200, 30, 30));
    window.draw(fgBar);

    if (fontLoaded) {
        sf::Text label(font, "ELDER LICH OF FALLEN HEAVENS", 18);
        label.setFillColor(sf::Color(220, 220, 220));
        sf::FloatRect lb = label.getLocalBounds();
        label.setOrigin({lb.size.x / 2.f, lb.size.y});
        label.setPosition({winL_f() / 2.f, 35.f});
        window.draw(label);
    }

    // Lives (hearts)
    float hx = 20.f;
    float hy = winB_f() - 30.f;
    for (int i = 0; i < maxLives; ++i) {
        float cx = hx + i * 24.f;
        sf::Color c = (i < remainingLives) ? sf::Color(220, 40, 40) : sf::Color(60, 60, 60);
        sf::CircleShape lc(6.f);
        lc.setOrigin({6.f, 6.f});
        lc.setPosition({cx - 5.f, hy - 4.f});
        lc.setFillColor(c);
        window.draw(lc);
        sf::CircleShape rc(6.f);
        rc.setOrigin({6.f, 6.f});
        rc.setPosition({cx + 5.f, hy - 4.f});
        rc.setFillColor(c);
        window.draw(rc);
        sf::ConvexShape tri(3);
        tri.setPoint(0, {cx - 8.f, hy + 2.f});
        tri.setPoint(1, {cx + 8.f, hy + 2.f});
        tri.setPoint(2, {cx, hy + 14.f});
        tri.setFillColor(c);
        window.draw(tri);
    }

    // Timer
    if (fontLoaded && !gameOver) {
        int secs = static_cast<int>(std::ceil(survivalTimeRemaining));
        if (secs < 0) secs = 0;
        sf::Text timerText(font, std::to_string(secs), 24);
        timerText.setFillColor(sf::Color::White);
        sf::FloatRect tb = timerText.getLocalBounds();
        timerText.setOrigin({tb.size.x, 0.f});
        timerText.setPosition({winL_f() - 20.f, 10.f});
        window.draw(timerText);
    }

    // Game over / win text
    if (gameOver && fontLoaded) {
        sf::RectangleShape overlay({winL_f(), winB_f()});
        overlay.setFillColor(sf::Color(0, 0, 0, 180));
        window.draw(overlay);

        std::string msg = gameWon ? "GREAT ENEMY FELLED" : "GAME OVER";
        sf::Text goText(font, msg, 196);
        goText.setFillColor(sf::Color(255, 215, 0));
        goText.setOutlineColor(sf::Color(80, 40, 0));
        goText.setOutlineThickness(2.f);
        sf::FloatRect tb = goText.getLocalBounds();
        goText.setOrigin({tb.size.x / 2.f, tb.size.y / 2.f});
        goText.setPosition({winL_f() / 2.f, winB_f() / 2.f});
        window.draw(goText);
    }
}

void Game5::resetGame() {
    gameOver = false;
    gameWon = false;
    deathHandled = false;
    player.setPosition({100.f * sx, groundY()});
    velocityY = 0.f;
    onGround = true;
    fireballs.clear();
    monkSpells.clear();
    dripstones.clear();
    spawnTimer = 0.f;
    monkSpawnTimer = 0.f;
    dripstoneSpawnTimer = 0.f;
    monkMovementTime = 0.f;
    monkPos = monkBasePos;
    monkSprite.setPosition(monkPos);
    survivalTimeRemaining = survivalDuration;
    monkHealthPercent = 100;
    remainingLives = maxLives;
    invincibilityTimer = 0.f;
    laughTimer = 8.f;
}

} // namespace L5
