#include <SFML/Graphics.hpp>
#include <optional>
#include "Header/tilemap.h"
#include "Header/player.h"
#include<iostream>

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({ 1280, 720 }),
        "Pixel Pilgrimage"
    );
    window.setFramerateLimit(60);

    Tilemap tilemap;
    if(!tilemap.loadFromImage("../src/Resources/sprite-level1.png")){
        std::cout << "Failed to load sprite-level1!" << std::endl;
    }
    if(!tilemap.loadTexture("../src/Resources/stone.png", "Stone")){
        std::cout << "Failed to load stone textures!" << std::endl;
    }
    if(!tilemap.loadTexture("../src/Resources/grass.png", "Grass")){
        std::cout << "Failed to load Grass textures!" << std::endl;
    }
    std::vector<sf::FloatRect> solids = tilemap.getSolidTiles();

     Player player(100.f , 100.f);
     if(!player.loadTextures()){
        std::cout << "Failed to load player textures!" << std::endl;
     }

    sf::Clock clock;
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