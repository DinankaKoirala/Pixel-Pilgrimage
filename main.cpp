#include <SFML/Graphics.hpp>
#include <optional>
#include <iostream>

// =========================
// BUTTON CLASS
// =========================
class Button
{
public:
    sf::RectangleShape shape;
    std::optional<sf::Text> text;

    Button(sf::Font& font, std::string label, sf::Vector2f size, sf::Vector2f pos)
    {
        shape.setSize(size);
        shape.setPosition(pos);
        shape.setFillColor(sf::Color(110, 75, 45));
        shape.setOutlineColor(sf::Color(245, 222, 179));
        shape.setOutlineThickness(4.f);

        text.emplace(font, label, 32);

        text->setFillColor(sf::Color(255, 240, 180));

        auto bounds = text->getGlobalBounds();

        text->setPosition(sf::Vector2f{
            pos.x + (size.x - bounds.size.x) / 2.f,
            pos.y + (size.y - bounds.size.y) / 2.f - 5.f
            });
    }

    void setHover(bool hover)
    {
        shape.setFillColor(
            hover ? sf::Color(160, 110, 55)
            : sf::Color(110, 75, 45)
        );
    }

    bool isClicked(sf::Vector2f mousePos)
    {
        return shape.getGlobalBounds().contains(mousePos);
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(shape);
        window.draw(*text);
    }
};

// =========================
// SETTINGS BUTTON
// =========================
class SettingsButton
{
public:
    sf::CircleShape shape;

    SettingsButton(sf::Vector2f pos)
    {
        shape.setRadius(25.f);
        shape.setPosition(pos);
        shape.setFillColor(sf::Color(200, 200, 200));
        shape.setOutlineColor(sf::Color::Black);
        shape.setOutlineThickness(2.f);
    }

    bool isClicked(sf::Vector2f mousePos)
    {
        return shape.getGlobalBounds().contains(mousePos);
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(shape);
    }
};

// =========================
// GAME WINDOW FUNCTION
// =========================
void openGameWindow()
{
    sf::RenderWindow gameWindow(
        sf::VideoMode({ 1280, 720 }),
        "Pixel Pilgrimage - Game"
    );

    gameWindow.setFramerateLimit(60);

    while (gameWindow.isOpen())
    {
        while (const std::optional event = gameWindow.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                gameWindow.close();
        }

        gameWindow.clear(sf::Color(40, 40, 80));
        gameWindow.display();
    }
}

// =========================
// MAIN
// =========================
int main()
{
    sf::RenderWindow window(
        sf::VideoMode({ 1280, 720 }),
        "Fantasy Kingdom - Menu"
    );

    window.setFramerateLimit(60);

    // BACKGROUND
    sf::Texture bgTexture;

    if (!bgTexture.loadFromFile("assets/background.png"))
    {
        std::cout << "Could not load background.png\n";
        return -1;
    }

    sf::Sprite background(bgTexture);

    background.setScale(sf::Vector2f{
        1280.f / bgTexture.getSize().x,
        720.f / bgTexture.getSize().y
        });

    // FONT
    sf::Font font;

    if (!font.openFromFile("assets/Cinzel-Regular.ttf"))
    {
        std::cout << "Could not load font\n";
        return -1;
    }

    // BUTTONS
    Button startButton(font, "START GAME", { 340.f, 90.f }, { 470.f, 240.f });
    Button continueButton(font, "CONTINUE", { 340.f, 90.f }, { 470.f, 360.f });
    Button exitButton(font, "EXIT", { 340.f, 90.f }, { 470.f, 480.f });

    SettingsButton settingsButton({ 1180.f, 30.f });

    // LOOP
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* mousePressed =
                event->getIf<sf::Event::MouseButtonPressed>())
            {
                sf::Vector2f mousePos =
                    window.mapPixelToCoords(mousePressed->position);

                // START GAME → OPEN NEW WINDOW
                if (startButton.isClicked(mousePos))
                {
                    std::cout << "START GAME\n";
                    openGameWindow();
                }

                if (continueButton.isClicked(mousePos))
                    std::cout << "CONTINUE\n";

                if (exitButton.isClicked(mousePos))
                    window.close();

                if (settingsButton.isClicked(mousePos))
                    std::cout << "SETTINGS\n";
            }
        }

        // HOVER EFFECT
        sf::Vector2f mousePos =
            window.mapPixelToCoords(sf::Mouse::getPosition(window));

        startButton.setHover(startButton.isClicked(mousePos));
        continueButton.setHover(continueButton.isClicked(mousePos));
        exitButton.setHover(exitButton.isClicked(mousePos));

        // DRAW
        window.clear();

        window.draw(background);

        startButton.draw(window);
        continueButton.draw(window);
        exitButton.draw(window);

        settingsButton.draw(window);

        window.display();
    }

    return 0;
}