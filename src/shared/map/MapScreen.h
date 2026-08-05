#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <memory>
#include <optional>
#include "../UIWidgets.h"

enum class MapScreenResult {
    None,
    Level1,
    Level2,
    Level3,
    Level4,
    Level5,
    BackToMenu,
    Exit
};

class MapScreen {
public:
    MapScreen(float width, float height, const std::string& assetsPath);

    MapScreenResult run(sf::RenderWindow& window);

private:
    static constexpr int LEVEL_COUNT = 5;
    static constexpr float AREA_W = 160.f;
    static constexpr float AREA_H = 120.f;

    void drawLevelMarker(sf::RenderWindow& window, sf::FloatRect area, const std::string& label, sf::Vector2f mouse);

    float width, height;
    std::string assetsPath;

    sf::Font font;
    sf::Texture mapTex;
    std::optional<sf::Sprite> mapSprite;

    std::array<sf::FloatRect, LEVEL_COUNT> levelAreas;

    sf::CircleShape gearIcon;
    bool showingSettings = false;

    std::unique_ptr<SettingsPanel> settings;
};
