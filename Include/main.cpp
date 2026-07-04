#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <optional>
#include "Level5.hpp"

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(800, 600)), "Level 5 - The Monk's Cave");
    window.setFramerateLimit(60);

    // Font used by the HUD. Try the project's own asset first, then fall
    // back to a system font (confirmed working: Windows' bundled Arial).
    sf::Font font;
    if (!font.openFromFile("assets/font.ttf")) {
        if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
            // TODO: handle missing font gracefully (fallback rendering, etc.)
        }
    }

    Level5 level(window, font);

    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            level.handleEvent(*event);
        }

        level.update(dt);

        window.clear(sf::Color(20, 15, 25)); // dark cave background placeholder
        level.draw();
        window.display();
    }

    return 0;
}