#include <SFML/Graphics.hpp>
#include <optional>
#include "Header/tilemap.h"

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({ 1280, 720 }),
        "Pixel Pilgrimage"
    );
    window.setFramerateLimit(60);

    Tilemap tilemap;
    tilemap.loadFromImage("/home/dinankakoirala/Desktop/Pixel-Pilgrimage/src/Resources/sprite-level1.png");

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color(40, 40, 80));
        tilemap.draw(window);
        window.display();
    }

    return 0;
}