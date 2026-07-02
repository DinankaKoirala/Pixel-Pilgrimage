#include "Header/tilemap.h"

#include<vector>
#include <SFML/Graphics.hpp>


bool Tilemap::loadFromImage(const std::string& path){
    sf::Image image;
    if(!image.loadFromFile("../src/Resources/sprite-level1.png")){
        return false;
    }

    tileCountX = image.getSize().x;
    tileCountY = image.getSize().y;

    tileGrid.resize(tileCountY);
    for(auto& row : tileGrid){
        row.resize(tileCountX , TileType::Air);
    }

    for(unsigned int rows = 0 ; rows<tileCountY ; rows++){
    for(unsigned int cols = 0 ; cols<tileCountX ; cols++){
            sf::Color color = image.getPixel({cols,rows});
            TileType tile = TileType::Air;

        if (color.a == 0) {
        tile = TileType::Air;
        }
        else if (color.r == 0x22 && color.g == 0x8B && color.b == 0x22) {
        tile = TileType::Grass;
        }
        else if (color.r == 0x8B && color.g == 0x45 && color.b == 0x13) {
        tile = TileType::Ground;
        }
        else if (color.r == 0xFF && color.g == 0xD7 && color.b == 0x00){
        tile = TileType::Coin;
        }

        else if (color.r == 0xFF && color.g == 0x00 && color.b == 0x00){
        tile = TileType::Enemy;
        }

        else if (color.r == 0x00 && color.g == 0x00 && color.b == 0xFF){
            tile = TileType::PlayerSpawn;
        }

        else if (color.r == 0xFF && color.g == 0x00 && color.b == 0xFF){
            tile = TileType::LevelEnd;
        }

        else if (color.r == 0xA4 && color.g == 0xE8 && color.b == 0xBB){
            tile = TileType::ObstacleGround;
        }

        else if (color.r == 0xFF && color.g == 0xA5 && color.b == 0x00){
            tile = TileType::Trigger;}
        tileGrid[rows][cols] = tile;
        }
    }
    return true;
}

void Tilemap::draw(sf::RenderWindow& window) const {
    sf::RectangleShape rect({(float)TileSize , (float)TileSize});
    for(int rows = 0 ; rows < tileCountY ; rows++){
        for(int cols = 0 ; cols < tileCountX ; cols++){
            TileType tile = tileGrid[rows][cols];
            if(tile == TileType::Air){
                continue;
            }
            rect.setPosition({(float)(cols*TileSize),(float)(rows*TileSize)});
            if(tile == TileType::Grass){
            rect.setFillColor(sf::Color::Green);
            }
            else if (tile == TileType::Ground){
                rect.setFillColor(sf::Color(211,211,211));
            }
            else if (tile == TileType::Coin){
                rect.setFillColor(sf::Color::Yellow);
            }
            else if (tile == TileType::ObstacleGround){
                rect.setFillColor(sf::Color(135, 206, 235));
            }
            else if (tile == TileType::Trigger){
                rect.setFillColor(sf::Color::Red);
            }
            else{
                rect.setFillColor(sf::Color::Black);
            }
            window.draw(rect);
        }
    }
}


std::vector<sf::FloatRect> Tilemap::getSolidTiles() const {
    std::vector<sf::FloatRect> solids;
    for (int rows = 0; rows < tileCountY; rows++) {
        for (int cols = 0; cols < tileCountX; cols++) {
            TileType tile = tileGrid[rows][cols];
            if(tile == TileType::Grass || tile == TileType::Ground || tile == TileType::ObstacleGround || tile == TileType::Trigger){
               sf::FloatRect rect({(float)(cols*TileSize),(float)(rows*TileSize)},{(float)TileSize, (float)TileSize});
               solids.push_back(rect); 
            }
        }
    }
    return solids;
}