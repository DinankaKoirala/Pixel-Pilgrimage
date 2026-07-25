#include "Game.hpp"
#include "Constants.hpp"
#include "runLevel4.h"
#include <sstream>
#include <algorithm>
#include <cmath>

Game::Game()
    : m_window(sf::VideoMode({(unsigned)Constants::WINDOW_WIDTH, (unsigned)Constants::WINDOW_HEIGHT}), "Rescue the Princess")
    , m_knight(Constants::SLOPE_START, Constants::SLOPE_END)
    , m_villain(Constants::SLOPE_START, Constants::SLOPE_END)
    , m_princess()
    , m_state(GameState::Intro)
    , m_introSpeed(0.35f)
    , m_deathScreen((float)Constants::WINDOW_WIDTH, (float)Constants::WINDOW_HEIGHT, "../src/level-1/assets/fonts/Vipnagorgialla Bd.otf")
{
    m_window.setFramerateLimit(60);

    if (m_backgroundTexture.loadFromFile("../src/level-3/assets/background.png")) {
        m_backgroundSprite.emplace(m_backgroundTexture);
        sf::Vector2u texSize = m_backgroundTexture.getSize();
        m_backgroundSprite->setScale({
            static_cast<float>(Constants::WINDOW_WIDTH) / texSize.x,
            static_cast<float>(Constants::WINDOW_HEIGHT) / texSize.y
        });
    }

    m_fontLoaded =
        m_font.openFromFile("../src/level-1/assets/fonts/Helvetica.ttf") ||
        m_font.openFromFile("C:/Windows/Fonts/arial.ttf") ||
        m_font.openFromFile("C:/Windows/Fonts/segoeui.ttf");

    if (m_fontLoaded) {
        m_hudText.setCharacterSize(22);
        m_hudText.setFillColor(sf::Color::White);
        m_hudText.setOutlineColor(sf::Color::Black);
        m_hudText.setOutlineThickness(1.f);
        m_hudText.setPosition({20.f, 20.f});

        m_messageText.setCharacterSize(36);
        m_messageText.setFillColor(sf::Color::White);
        m_messageText.setOutlineColor(sf::Color::Black);
        m_messageText.setOutlineThickness(2.f);
    }

    m_princess.followAnchor(m_villain.getPosition());
}

void Game::run() {
    sf::Clock clock;
    while (m_window.isOpen()) {
        float dt = clock.restart().asSeconds();
        processEvents();
        update(dt);
        render();

        if (m_state == GameState::Won && !m_levelCompleteShown) {
            m_levelCompleteShown = true;
            m_levelCompleteClock.restart();
        }
        if (m_state == GameState::Won && m_levelCompleteShown &&
            m_levelCompleteClock.getElapsedTime().asSeconds() >= 1.5f) {
            m_window.close();
            runLevel4();
        }
    }
}

void Game::processEvents() {
    while (const std::optional event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }

        if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            if (key->code == sf::Keyboard::Key::Space && m_state == GameState::Playing) {
                m_knight.jump();
            }
            if (key->code == sf::Keyboard::Key::R &&
                m_state == GameState::GameOver) {
                restart();
            }
            if (key->code == sf::Keyboard::Key::Escape) {
                m_window.close();
            }
        }

        if (const auto* mbp = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mbp->button == sf::Mouse::Button::Left && m_state == GameState::GameOver && !m_deathHandled) {
                DeathScreenResult result = m_deathScreen.getInput(sf::Vector2f(mbp->position));
                if (result == DeathScreenResult::Exit) {
                    m_window.close();
                } else if (result == DeathScreenResult::Restart) {
                    restart();
                }
            }
        }
    }
}

void Game::update(float dt) {
    if (m_state == GameState::Intro) {
        m_villain.advanceTowardTop(dt, m_introSpeed);
        m_princess.followAnchor(m_villain.getPosition());

        if (m_villain.hasReachedTop()) {
            m_villain.snapToTop();
            m_princess.followAnchor(m_villain.getPosition());
            m_state = GameState::Playing;
        }
        return;
    }

    if (m_state != GameState::Playing) {
        return;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
        m_knight.moveUp(dt);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
        m_knight.moveDown(dt);
    }

    m_knight.update(dt);
    m_villain.attack(dt, m_knight.getPosition(), m_projectiles);

    for (auto& projectile : m_projectiles) {
        projectile->update(dt);
    }

    checkCollisions();

    m_projectiles.erase(
        std::remove_if(m_projectiles.begin(), m_projectiles.end(),
            [this](const std::unique_ptr<Projectile>& p) {
                return p->isDead() || p->isOffScreen(m_window.getSize().x, m_window.getSize().y);
            }),
        m_projectiles.end()
    );

    if (!m_knight.isAlive()) {
        m_state = GameState::GameOver;
        m_deathHandled = false;
    } else if (m_knight.getProgress() >= 1.f) {
        m_state = GameState::Won;
    }
}

void Game::checkCollisions() {
    sf::Vector2f knightPos = m_knight.getPosition();
    float knightRadius = m_knight.getRadius();

    for (auto& projectile : m_projectiles) {
        if (projectile->isDead()) continue;

        if (m_knight.isJumping() && projectile->isStone()) continue;

        sf::Vector2f diff = knightPos - projectile->getPosition();
        float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        if (distance < knightRadius + projectile->getRadius()) {
            if (!m_knight.isInvulnerable()) {
                m_knight.takeDamage(1);
            }
            projectile->kill();
        }
    }
}

void Game::render() {
    m_window.clear(sf::Color::Black);
    if (m_backgroundSprite) m_window.draw(*m_backgroundSprite);

    m_villain.draw(m_window);
    m_princess.draw(m_window);

    for (auto& projectile : m_projectiles) {
        projectile->draw(m_window);
    }
    m_knight.draw(m_window);

    if (m_fontLoaded) {
        if (m_state == GameState::Playing || m_state == GameState::Intro) {
            std::ostringstream hud;
            hud << "Lives: " << m_knight.getLives()
                << "   Progress: " << static_cast<int>(m_knight.getProgress() * 100) << "%";
            m_hudText.setString(hud.str());
            m_window.draw(m_hudText);
        }

        if (m_state == GameState::Intro) {
            m_messageText.setString("The villain has taken the princess! Catch him!");
        } else if (m_state == GameState::GameOver) {
            m_messageText.setString("GAME OVER");
        } else if (m_state == GameState::Won) {
            m_messageText.setString("You rescued the princess!");
        }

        if (m_state != GameState::Playing) {
            sf::FloatRect bounds = m_messageText.getLocalBounds();
            m_messageText.setPosition({
                Constants::WINDOW_WIDTH / 2.f - bounds.size.x / 2.f, 40.f});
            m_window.draw(m_messageText);
        }
    }

    if (m_state == GameState::GameOver) {
        m_deathScreen.draw(m_window);
    }

    if (m_state == GameState::Won) {
        sf::RectangleShape overlay({(float)Constants::WINDOW_WIDTH, (float)Constants::WINDOW_HEIGHT});
        overlay.setFillColor(sf::Color(0, 0, 0, 200));
        m_window.draw(overlay);

        if (m_fontLoaded) {
            sf::Text t(m_font, "LEVEL COMPLETE!", 48);
            sf::FloatRect b = t.getLocalBounds();
            t.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
            t.setPosition({Constants::WINDOW_WIDTH / 2.f, Constants::WINDOW_HEIGHT / 2.f - 40.f});
            t.setFillColor(sf::Color(100, 255, 100));
            m_window.draw(t);

            sf::Text s(m_font, "Loading next level...", 22);
            sf::FloatRect sb = s.getLocalBounds();
            s.setOrigin({sb.size.x / 2.f, sb.size.y / 2.f});
            s.setPosition({Constants::WINDOW_WIDTH / 2.f, Constants::WINDOW_HEIGHT / 2.f + 40.f});
            s.setFillColor(sf::Color(200, 200, 200));
            m_window.draw(s);
        }
    }

    m_window.display();
}

void Game::restart() {
    m_knight.reset();
    m_villain.reset();
    m_princess.followAnchor(m_villain.getPosition());
    m_projectiles.clear();
    m_state = GameState::Intro;
    m_deathHandled = false;
    m_levelCompleteShown = false;
}
