#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>
#include "../UIWidgets.h"

enum class StartScreenResult {
    None,
    StartGame,
    ShowMap,
    MapsLevel1,
    MapsLevel2,
    Exit
};

class StartScreen {
public:
    StartScreen(float width, float height, const std::string& assetsPath);

    StartScreenResult run(sf::RenderWindow& window);

private:
    void drawStartMenu(sf::RenderWindow& window, sf::Vector2f mouse);
    void drawSettingsPanel(sf::RenderWindow& window, sf::Vector2f mouse);
    StartScreenResult handleStartMenuClick(sf::Vector2f mouse);

    float width, height;
    std::string assetsPath;

    sf::Font font;
    sf::Texture bgTex;
    std::optional<sf::Sprite> background;

    std::optional<sf::Text> titleText;

    std::optional<Button> btnStart;
    std::optional<Button> btnMaps;
    std::optional<Button> btnExit;
    sf::CircleShape gearIcon;

    bool showingSettings = false;

    std::unique_ptr<SettingsPanel> settings;
};
