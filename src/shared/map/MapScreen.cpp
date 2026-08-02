#include "MapScreen.h"
#include <iostream>

void MapScreen::drawLevelMarker(sf::RenderWindow& window, sf::FloatRect area, const std::string& label, sf::Vector2f mouse)
{
    bool hovered = area.contains(mouse);

    sf::RectangleShape bg(sf::Vector2f(area.size.x, area.size.y));
    bg.setPosition(sf::Vector2f(area.position.x, area.position.y));
    bg.setFillColor(hovered ? sf::Color(0, 0, 0, 160) : sf::Color(0, 0, 0, 100));
    bg.setOutlineColor(hovered ? sf::Color(255, 215, 0) : sf::Color(255, 255, 255, 100));
    bg.setOutlineThickness(hovered ? 3.f : 1.f);
    window.draw(bg);

    sf::Text labelText(font, label, hovered ? 22 : 16);
    labelText.setFillColor(hovered ? sf::Color(255, 215, 0) : sf::Color::White);
    labelText.setOutlineColor(sf::Color::Black);
    labelText.setOutlineThickness(2.f);
    sf::FloatRect lb = labelText.getLocalBounds();
    labelText.setOrigin({ lb.size.x / 2.f, lb.size.y / 2.f });
    labelText.setPosition({ area.position.x + area.size.x / 2.f, area.position.y + area.size.y / 2.f });
    window.draw(labelText);
}

static sf::FloatRect centeredAt(float cx, float cy, float w, float h)
{
    return sf::FloatRect({ cx - w / 2.f, cy - h / 2.f }, { w, h });
}

MapScreen::MapScreen(float width, float height, const std::string& assetsPath)
    : width(width), height(height), assetsPath(assetsPath)
{
    if (!mapTex.loadFromFile(assetsPath + "textures/maps.png")) {
        std::cerr << "FAILED to load map texture!" << std::endl;
    }
    mapSprite.emplace(mapTex);
    float sx = width / (float)mapTex.getSize().x;
    float sy = height / (float)mapTex.getSize().y;
    mapSprite->setScale({ sx, sy });

    if (!font.openFromFile(assetsPath + "fonts/Cinzel-Regular.ttf")) {
        if (!font.openFromFile(assetsPath + "fonts/Helvetica.ttf")) {
            std::cerr << "FAILED to load font for MapScreen!" << std::endl;
        }
    }

    levelAreas[0] = centeredAt(250.f, 300.f, AREA_W, AREA_H);
    levelAreas[1] = centeredAt(650.f, 400.f, AREA_W, AREA_H);
    levelAreas[2] = centeredAt(1000.f, 250.f, AREA_W, AREA_H);
    levelAreas[3] = centeredAt(1050.f, 500.f, AREA_W, AREA_H);
    levelAreas[4] = centeredAt(250.f, 580.f, AREA_W, AREA_H);

    gearIcon.setRadius(22.f);
    gearIcon.setPosition({ width - 60.f, 25.f });
    gearIcon.setFillColor(sf::Color(200, 200, 200));
    gearIcon.setOutlineColor(sf::Color::Black);
    gearIcon.setOutlineThickness(2.f);

    settings = std::make_unique<SettingsPanel>(font);
}

MapScreenResult MapScreen::run(sf::RenderWindow& window)
{
    showingSettings = false;

    while (window.isOpen())
    {
        sf::Vector2f mouse = sf::Vector2f(sf::Mouse::getPosition(window));
        bool pressing = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
                return MapScreenResult::Exit;
            }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                {
                    if (showingSettings) { showingSettings = false; continue; }
                    return MapScreenResult::BackToMenu;
                }
            }

            if (const auto* mbp = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mbp->button != sf::Mouse::Button::Left) continue;

                sf::Vector2f mp((float)mbp->position.x, (float)mbp->position.y);

                if (showingSettings)
                {
                    settings->handleClick(mp);
                    if (!settings->visible) showingSettings = false;
                    continue;
                }

                for (int i = 0; i < LEVEL_COUNT; i++) {
                    if (levelAreas[i].contains(mp))
                        return static_cast<MapScreenResult>(static_cast<int>(MapScreenResult::Level1) + i);
                }

                sf::FloatRect gearBounds = gearIcon.getGlobalBounds();
                if (gearBounds.contains(mp)) { showingSettings = true; continue; }
            }
        }

        if (showingSettings)
            settings->handleDrag(mouse, pressing);

        window.clear();

        if (mapSprite) window.draw(*mapSprite);

        if (showingSettings)
        {
            settings->draw(window, mouse);
        }
        else
        {
            for (int i = 0; i < LEVEL_COUNT; i++) {
                drawLevelMarker(window, levelAreas[i], "LEVEL " + std::to_string(i + 1), mouse);
            }
            window.draw(gearIcon);
        }

        window.display();
    }

    return MapScreenResult::Exit;
}
