#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>
#include "../UIWidgets.h"

enum class MapScreenResult {
    None,
    Level1,
    Level2,
    BackToMenu,
    Exit
};

class MapScreen {
public:
    MapScreen(float width, float height, const std::string& assetsPath);

    MapScreenResult run(sf::RenderWindow& window);

private:
    bool pointInRect(sf::Vector2f p, float rx, float ry, float rw, float rh);
    void drawLevelMarker(sf::RenderWindow& window, sf::FloatRect area, const std::string& label, sf::Vector2f mouse);

    float width, height;
    std::string assetsPath;

    sf::Font font;
    sf::Texture mapTex;
    std::optional<sf::Sprite> mapSprite;

    sf::FloatRect level1Area;
    sf::FloatRect level2Area;

    sf::CircleShape gearIcon;
    bool showingSettings = false;

    std::unique_ptr<SettingsPanel> settings;
};
