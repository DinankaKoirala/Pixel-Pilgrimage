#include "Game.hpp"
#include "Constants.hpp"
#include <sstream>
#include <algorithm>
#include <cmath>

Game::Game()
    : m_window(sf::VideoMode(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT), "Rescue the Princess"),
      m_knight(Constants::SLOPE_START, Constants::SLOPE_END),
      m_villain(Constants::SLOPE_START, Constants::SLOPE_END),
      m_princess(),
      m_state(GameState::Intro),
      m_introSpeed(0.35f)
{
    m_window.setFramerateLimit(60);

    if (m_backgroundTexture.loadFromFile("assets/background.png")) {
        m_backgroundSprite.setTexture(m_backgroundTexture);
        sf::Vector2u texSize = m_backgroundTexture.getSize();
        m_backgroundSprite.setScale(
            static_cast<float>(Constants::WINDOW_WIDTH) / texSize.x,
            static_cast<float>(Constants::WINDOW_HEIGHT) / texSize.y
        );
    }

    m_fontLoaded =
        m_font.loadFromFile("assets/font.ttf") ||
        m_font.loadFromFile("C:/Windows/Fonts/arial.ttf") ||
        m_font.loadFromFile("C:/Windows/Fonts/segoeui.ttf");

    if (m_fontLoaded) {
        m_hudText.setFont(m_font);
        m_hudText.setCharacterSize(22);
        m_hudText.setFillColor(sf::Color::White);
        m_hudText.setOutlineColor(sf::Color::Black);
        m_hudText.setOutlineThickness(1.f);
        m_hudText.setPosition(20.f, 20.f);

        m_messageText.setFont(m_font);
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
    }
}

void Game::processEvents() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_window.close();
        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Space && m_state == GameState::Playing) {
                m_knight.jump();
            }
            if (event.key.code == sf::Keyboard::R &&
                (m_state == GameState::GameOver || m_state == GameState::Won)) {
                restart();
            }
            if (event.key.code == sf::Keyboard::Escape) {
                m_window.close();
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

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        m_knight.moveUp(dt);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
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
    } else if (m_knight.getProgress() >= 1.f) {
        m_state = GameState::Won;
    }
}

void Game::checkCollisions() {
    sf::Vector2f knightPos = m_knight.getPosition();
    float knightRadius = m_knight.getRadius();

    for (auto& projectile : m_projectiles) {
        if (projectile->isDead()) continue;

        // Jumping only dodges rolling stones, not arrows.
        if (m_knight.isJumping() && projectile->isStone()) continue;

        sf::Vector2f diff = knightPos - projectile->getPosition();
        float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        if (distance < knightRadius + projectile->getRadius()) {
            if (!m_knight.isInvulnerable()) {
                m_knight.takeDamage(1); // one life per hit
            }
            projectile->kill();
        }
    }
}

void Game::render() {
    m_window.clear(sf::Color::Black);
    m_window.draw(m_backgroundSprite);

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
            m_messageText.setString("GAME OVER - Press R to try again");
        } else if (m_state == GameState::Won) {
            m_messageText.setString("You rescued the princess! Press R to play again");
        }

        if (m_state != GameState::Playing) {
            sf::FloatRect bounds = m_messageText.getLocalBounds();
            m_messageText.setPosition(
                Constants::WINDOW_WIDTH / 2.f - bounds.width / 2.f, 40.f);
            m_window.draw(m_messageText);
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
}
