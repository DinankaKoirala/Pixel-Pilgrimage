#include <SFML/Graphics.hpp>
#include <optional>
#include "Header/tilemap.h"
#include "Header/player.h"
#include"Header/enemy.h"
#include"Header/background.h"
#include<iostream>

int main()
{
    unsigned int width = 1280;
    unsigned int height = 720;

    sf::RenderWindow window(
        sf::VideoMode({ width, height }),
        "Pixel Pilgrimage"
    );
    window.setFramerateLimit(60);

    Tilemap tilemap;
    if(!tilemap.loadFromImage("../src/Resources/sprite-level1.png")){
        std::cout << "Failed to load sprite-level1!" << std::endl;
    }
    if(!tilemap.loadTexture("../src/Resources/dirt_tile.png", "Stone")){
        std::cout << "Failed to load stone textures!" << std::endl;
    }
    if(!tilemap.loadTexture("../src/Resources/grass_tile.png", "Grass")){
        std::cout << "Failed to load Grass textures!" << std::endl;
    }
    std::vector<sf::FloatRect> solids = tilemap.getSolidTiles();

    sf::Vector2f playerSpawn = tilemap.getPlayerSpawnPoint();
    Player player(playerSpawn.x, playerSpawn.y);
     if(!player.loadTextures()){
        std::cout << "Failed to load player textures!" << std::endl;
     }

     bool playerAlive = true;
    sf::Clock clock;
    sf::View camera(sf::FloatRect({0.f, 0.f}, {1280.f, 720.f}));
    std::vector<sf::Vector2f> enemySpawns = tilemap.getEnemySpawnPoints();
    std::vector<Enemy> enemies;
    for(const sf::Vector2f& enemyPos :enemySpawns){
         Enemy enemy(enemyPos.x , enemyPos.y);
         enemies.push_back(enemy);   
        }
        for(Enemy& enemy : enemies) {
            if(!enemy.loadTextures("../src/Resources/rhino_sheet.png")) {
                std::cout << "Failed to load enemy texture!" << std::endl;
            }
        }

        Background background;
        background.loadTexture("../src/Resources/background.png");


    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        float dt = clock.restart().asSeconds();
        if(playerAlive){
            player.handleInput();
            player.update(dt,solids);
            for(Enemy& enemy :enemies){
                enemy.update(dt,solids);
            }
            for (Enemy& enemy : enemies) {
                if (auto overlap = player.getPlayerHitbox().findIntersection(enemy.getEnemyHitbox())) {
                    playerAlive = false;
                }
                if(player.getPosition().y + 32 >= 720){
                    playerAlive = false;
                }
            }
        }


        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::R)){
            player.reset(playerSpawn.x, playerSpawn.y);
            for (size_t i = 0; i < enemies.size(); i++) {
                enemies[i].reset(enemySpawns[i].x, enemySpawns[i].y);
            }   
                playerAlive = true;
        }
        if(player.getPosition().x <= window.getSize().x / 2.f){
            camera.setCenter({1280/2.f, 360.f});
            window.setView(camera);
        }
        else {
            camera.setCenter({player.getPosition().x, 360.f});
            window.setView(camera);
        }

        window.clear();
        background.draw(window , camera.getCenter().x);
        tilemap.draw(window);
        player.draw(window); 
        for(Enemy& enemy :enemies){
            enemy.draw(window);
        }
        window.display();
    }

    return 0;
}