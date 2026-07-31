#include "DeathScreen.h"
#include <iostream>
#include <string>

DeathScreen::DeathScreen(float width , float height, const std::string& fontPath)
:gameOverText(font),noText(font),yesText(font),restartText(font)
, m_designWidth(width), m_designHeight(height)
{
    if (!font.openFromFile(fontPath)) {
        std::cerr << "FAILED to load font from " << fontPath << "!" << std::endl;
    }

    gameOverText.setString("GAME OVER");
    gameOverText.setFillColor(sf::Color(255,255,255));

    noText.setString("No");
    noText.setFillColor(sf::Color(255,255,255));

    yesText.setString("Yes");
    yesText.setFillColor(sf::Color(255,255,255));

    restartText.setString("RESTART GAME ?");
    restartText.setFillColor(sf::Color(255,255,255));

    overlay.setSize({m_designWidth, m_designHeight});
    overlay.setFillColor(sf::Color(0,0,0,160));

    noButton.setSize({100.f,50.f});
    yesButton.setSize({100.f,50.f});
}

sf::View DeathScreen::uiView() const {
    sf::View v(sf::FloatRect({0.f, 0.f}, {m_designWidth, m_designHeight}));
    v.setViewport(sf::FloatRect({0.f, 0.f}, {1.f, 1.f}));
    return v;
}

void DeathScreen::layout() {
    const float w = m_designWidth;
    const float h = m_designHeight;

    noButton.setOrigin({noButton.getSize().x / 2.f, noButton.getSize().y / 2.f});
    noButton.setPosition({w / 2.f + 120.f, h / 2.f + 80.f});

    yesButton.setOrigin({yesButton.getSize().x / 2.f, yesButton.getSize().y / 2.f});
    yesButton.setPosition({w / 2.f - 120.f, h / 2.f + 80.f});

    auto centerText = [&](sf::Text& text, float x, float y, unsigned charSize) {
        text.setCharacterSize(charSize);
        sf::FloatRect b = text.getLocalBounds();
        text.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
        text.setPosition({x, y});
    };

    centerText(gameOverText, w / 2.f, h / 2.f - 100.f, 40);
    centerText(noText, w / 2.f + 120.f, h / 2.f + 80.f, 20);
    centerText(yesText, w / 2.f - 120.f, h / 2.f + 80.f, 20);
    centerText(restartText, w / 2.f, h / 2.f, 25);
}

void DeathScreen::draw(sf::RenderWindow& window){
    layout();

    sf::View prevView = window.getView();
    window.setView(uiView());

    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    if (yesButton.getGlobalBounds().contains(mousePos)) {
        yesButton.setFillColor(sf::Color(70, 200, 100));
    } else {
        yesButton.setFillColor(sf::Color(50, 168, 82));
    }

    if (noButton.getGlobalBounds().contains(mousePos)) {
        noButton.setFillColor(sf::Color(200, 70, 70));
    } else {
        noButton.setFillColor(sf::Color(168, 50, 50));
    }

    window.draw(overlay);
    window.draw(gameOverText);
    window.draw(restartText);
    window.draw(yesButton);
    window.draw(yesText);
    window.draw(noButton);
    window.draw(noText);

    window.setView(prevView);
}

DeathScreenResult DeathScreen::getInput(sf::RenderWindow& window, sf::Vector2i pixelPos){
    sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos, uiView());
    if (yesButton.getGlobalBounds().contains(mousePos)) {
        return DeathScreenResult::Restart;
    } else if (noButton.getGlobalBounds().contains(mousePos)) {
        return DeathScreenResult::Exit;
    }

    return DeathScreenResult::None;
}
