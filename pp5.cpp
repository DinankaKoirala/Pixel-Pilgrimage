#include "pp5.h"
#include "Level5.hpp"
#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(800, 600)), "PP5 - Level 5");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        return -1;
    }

    Level5 level(window, font);
    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        while (true) {
            std::optional<sf::Event> event = window.pollEvent();
            if (!event.has_value()) break;
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            level.handleEvent(*event);
        }

        level.update(dt);

        window.clear(sf::Color(30, 30, 40));
        level.draw();
        window.display();
    }

    return 0;
}
