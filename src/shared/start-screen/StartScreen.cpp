#include "StartScreen.h"
#include <iostream>

StartScreen::StartScreen(float width, float height, const std::string& assetsPath)
    : width(width), height(height), assetsPath(assetsPath)
{
    if (!font.openFromFile(assetsPath + "fonts/Cinzel-Regular.ttf")) {
        if (!font.openFromFile(assetsPath + "fonts/Helvetica.ttf")) {
            std::cerr << "FAILED to load start screen font!" << std::endl;
        }
    }

    if (!bgTex.loadFromFile("../start-screen/background.png")) {
        if (!bgTex.loadFromFile(assetsPath + "textures/background.png")) {
            std::cout << "Could not load start screen background" << std::endl;
        }
    }
    background.emplace(bgTex);
    background->setScale({ width / bgTex.getSize().x, height / bgTex.getSize().y });

    titleText.emplace(font, "PIXEL PILGRIMAGE", 56);
    titleText->setFillColor(sf::Color(255, 215, 0));
    titleText->setOutlineColor(sf::Color(50, 30, 10));
    titleText->setOutlineThickness(4.f);
    sf::FloatRect tb = titleText->getLocalBounds();
    titleText->setOrigin({ tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f });
    titleText->setPosition({ width / 2.f, 130.f });

    btnStart.emplace(width / 2.f - 150.f, 260.f, 300.f, 70.f, "START GAME", font);
    btnMaps.emplace(width / 2.f - 150.f, 360.f, 300.f, 70.f, "MAPS", font);
    btnExit.emplace(width / 2.f - 150.f, 460.f, 300.f, 70.f, "EXIT", font);

    gearIcon.setRadius(22.f);
    gearIcon.setPosition({ width - 60.f, 25.f });
    gearIcon.setFillColor(sf::Color(200, 200, 200));
    gearIcon.setOutlineColor(sf::Color::Black);
    gearIcon.setOutlineThickness(2.f);

    settings = std::make_unique<SettingsPanel>(font);
}

StartScreenResult StartScreen::run(sf::RenderWindow& window)
{
    showingSettings = false;

    while (window.isOpen())
    {
        sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
                return StartScreenResult::Exit;
            }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                {
                    if (showingSettings) { showingSettings = false; continue; }
                }
            }

            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mousePressed->button != sf::Mouse::Button::Left) continue;

                sf::Vector2f clickPos = window.mapPixelToCoords(mousePressed->position);

                if (showingSettings)
                {
                    settings->handleClick(clickPos);
                    continue;
                }

                StartScreenResult r = handleStartMenuClick(clickPos);
                if (r != StartScreenResult::None) return r;
            }
        }

        if (showingSettings)
        {
            settings->handleDrag(mouse, true);
        }

        window.clear();

        window.draw(*background);

        if (showingSettings)
        {
            settings->draw(window, mouse);
        }
        else
        {
            drawStartMenu(window, mouse);
        }

        window.display();
    }

    return StartScreenResult::Exit;
}

void StartScreen::drawStartMenu(sf::RenderWindow& window, sf::Vector2f mouse)
{
    window.draw(*titleText);
    btnStart->draw(window, mouse);
    btnMaps->draw(window, mouse);
    btnExit->draw(window, mouse);
    window.draw(gearIcon);
}

void StartScreen::drawSettingsPanel(sf::RenderWindow& window, sf::Vector2f mouse)
{
    settings->draw(window, mouse);
}

StartScreenResult StartScreen::handleStartMenuClick(sf::Vector2f mouse)
{
    if (btnStart->contains(mouse)) return StartScreenResult::StartGame;
    if (btnMaps->contains(mouse))  return StartScreenResult::ShowMap;
    if (btnExit->contains(mouse))  return StartScreenResult::Exit;

    sf::FloatRect gearBounds = gearIcon.getGlobalBounds();
    if (gearBounds.contains(mouse)) { showingSettings = true; return StartScreenResult::None; }

    return StartScreenResult::None;
}
