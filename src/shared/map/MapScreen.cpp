#include "MapScreen.h"
#include <iostream>

bool MapScreen::pointInRect(sf::Vector2f p, float rx, float ry, float rw, float rh)
{
    return p.x >= rx && p.x <= rx + rw && p.y >= ry && p.y <= ry + rh;
}

void MapScreen::drawLevelMarker(sf::RenderWindow& window, sf::FloatRect area, const std::string& label, sf::Vector2f mouse)
{
    bool hovered = area.contains(mouse);
    sf::Color fill = hovered ? sf::Color(255, 255, 255, 80) : sf::Color(255, 255, 255, 30);
    sf::Color outline = hovered ? sf::Color(255, 215, 0) : sf::Color(255, 255, 255, 0);

    sf::RectangleShape marker(sf::Vector2f(area.size.x, area.size.y));
    marker.setPosition(sf::Vector2f(area.position.x, area.position.y));
    marker.setFillColor(fill);
    marker.setOutlineColor(outline);
    marker.setOutlineThickness(hovered ? 3.f : 0.f);
    window.draw(marker);

    if (hovered) {
        sf::Text tooltip(font, label, 20);
        tooltip.setFillColor(sf::Color::White);
        tooltip.setOutlineColor(sf::Color::Black);
        tooltip.setOutlineThickness(2.f);
        sf::FloatRect tb = tooltip.getLocalBounds();
        tooltip.setOrigin({ tb.size.x / 2.f, tb.size.y / 2.f });
        tooltip.setPosition({ area.position.x + area.size.x / 2.f, area.position.y - 25.f });
        window.draw(tooltip);
    }
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

    level1Area = sf::FloatRect({ 100.f, 200.f }, { 300.f, 200.f });
    level2Area = sf::FloatRect({ 500.f, 300.f }, { 300.f, 200.f });
    level3Area = sf::FloatRect({ 850.f, 150.f }, { 300.f, 200.f });
    level4Area = sf::FloatRect({ 900.f, 400.f }, { 300.f, 200.f });
    level5Area = sf::FloatRect({ 100.f, 480.f }, { 300.f, 200.f });

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

                if (level1Area.contains(mp)) return MapScreenResult::Level1;
                        if (level2Area.contains(mp)) return MapScreenResult::Level2;
                        if (level3Area.contains(mp)) return MapScreenResult::Level3;
                        if (level4Area.contains(mp)) return MapScreenResult::Level4;
                        if (level5Area.contains(mp)) return MapScreenResult::Level5;

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
            drawLevelMarker(window, level1Area, "LEVEL 1", mouse);
            drawLevelMarker(window, level2Area, "LEVEL 2", mouse);
            drawLevelMarker(window, level3Area, "LEVEL 3", mouse);
            drawLevelMarker(window, level4Area, "LEVEL 4", mouse);
            drawLevelMarker(window, level5Area, "LEVEL 5", mouse);
            window.draw(gearIcon);
        }

        window.display();
    }

    return MapScreenResult::Exit;
}
