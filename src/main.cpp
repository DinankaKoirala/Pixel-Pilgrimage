#include <SFML/Graphics.hpp>
#include <optional>
#include "Header/tilemap.h"
#include "Header/player.h"

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({ 1280, 720 }),
        "Pixel Pilgrimage"
    );
    window.setFramerateLimit(60);

    Tilemap tilemap;
    tilemap.loadFromImage("/home/dinankakoirala/Desktop/Pixel-Pilgrimage/src/Resources/sprite-level1.png");
    tilemap.loadTexture("/home/dinankakoirala/Desktop/Pixel-Pilgrimage/src/Resources/stone.png" , "Stone");
    tilemap.loadTexture("/home/dinankakoirala/Desktop/Pixel-Pilgrimage/src/Resources/grass.png" , "Grass");
    std::vector<sf::FloatRect> solids = tilemap.getSolidTiles();

    sf::Clock clock;
    Player player(100.f , 100.f);
    sf::View camera(sf::FloatRect({0.f, 0.f}, {1280.f, 720.f}));

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        float dt = clock.restart().asSeconds();
        player.handleInput();
        player.update(dt,solids);

        camera.setCenter(player.getPosition());
        window.setView(camera);

        window.clear(sf::Color(40, 40, 80));
        tilemap.draw(window);
        player.draw(window); 
        window.display();
    }

    return 0;
}