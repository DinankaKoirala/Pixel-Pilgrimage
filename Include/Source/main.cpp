#include <SFML/Graphics.hpp>   
#include <optional>            
#include "settingspanel.h"     

// Path to the background image
constexpr const char* IMAGE_PATH = "Assets/maps.png";

// Window dimensions
constexpr unsigned int WIN_W = 1024;
constexpr unsigned int WIN_H = 768;


static bool rectContains(float rx, float ry, float rw, float rh,
    sf::Vector2f p)
{
    return p.x >= rx && p.x <= rx + rw &&
        p.y >= ry && p.y <= ry + rh;
}

int main()
{

    sf::RenderWindow window(
        sf::VideoMode({ WIN_W, WIN_H }), 
        "Road Map",                      
        sf::Style::Close                 
    );

    
    window.setFramerateLimit(60);


    sf::Texture mapTex;

    
    if (!mapTex.loadFromFile(IMAGE_PATH))
    {
       
        return 1;
    }

    
    sf::Sprite mapSprite(mapTex);

   
    float sx = (float)WIN_W / (float)mapTex.getSize().x;
    float sy = (float)WIN_H / (float)mapTex.getSize().y;

    // Apply scaling
    mapSprite.setScale({ sx, sy });


    sf::Font font;
    bool fontLoaded = false;

    
    for (auto& path : {
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf",
        "/usr/share/fonts/TTF/DejaVuSans-Bold.ttf"
        })
    {
        if (font.openFromFile(path))
        {
            fontLoaded = true;
            break;
        }
    }

    
    if (!fontLoaded)
        return 1;

  
    constexpr float settX = 870;
    constexpr float settY = 8;
    constexpr float settW = 78;
    constexpr float settH = 55;


    constexpr float exitX = 958;
    constexpr float exitY = 8;
    constexpr float exitW = 62;
    constexpr float exitH = 55;


    SettingsPanel settings(font);


    sf::RectangleShape settingsHL({ settW, settH });

    settingsHL.setPosition({ settX, settY });
    settingsHL.setFillColor(sf::Color(255, 255, 255, 30)); // Semi-transparent
    settingsHL.setOutlineColor(sf::Color::Yellow);
    settingsHL.setOutlineThickness(0); // Hidden until mouse hovers


    sf::RectangleShape exitHL({ exitW, exitH });

    exitHL.setPosition({ exitX, exitY });
    exitHL.setFillColor(sf::Color(255, 255, 255, 30));
    exitHL.setOutlineColor(sf::Color::Yellow);
    exitHL.setOutlineThickness(0);


    while (window.isOpen())
    {

        sf::Vector2f mouse =
            sf::Vector2f(sf::Mouse::getPosition(window));

       
        bool pressing =
            sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        
        while (const std::optional event = window.pollEvent())
        {
           
            
            if (event->is<sf::Event::Closed>())
                window.close();

          
            if (const auto* mbp =
                event->getIf<sf::Event::MouseButtonPressed>())
            {
                
                if (mbp->button == sf::Mouse::Button::Left)
                {
                    
                    sf::Vector2f mp(
                        (float)mbp->position.x,
                        (float)mbp->position.y);

                    if (!settings.visible)
                    {
                       
                        if (rectContains(settX, settY,
                            settW, settH, mp))
                        {
                            settings.visible = true;
                        }

                       
                        if (rectContains(exitX, exitY,
                            exitW, exitH, mp))
                        {
                            window.close();
                        }
                    }
                 
                    else
                    {
                        
                        settings.handleClick(mp);
                    }
                }
            }
        }


        if (settings.visible)
            settings.handleDrag(mouse, pressing);

        // Show yellow outline when mouse hovers over Settings
        settingsHL.setOutlineThickness(
            rectContains(settX, settY,
                settW, settH, mouse)
            ? 2.f : 0.f);

        // Show yellow outline when mouse hovers over Exit
        exitHL.setOutlineThickness(
            rectContains(exitX, exitY,
                exitW, exitH, mouse)
            ? 2.f : 0.f);

   

        // Clear previous frame
        window.clear();

        // Draw the map
        window.draw(mapSprite);

        // Draw highlight boxes
        window.draw(settingsHL);
        window.draw(exitHL);

        // Draw settings panel if it is open
        if (settings.visible)
            settings.draw(window, mouse);

        // Display the completed frame
        window.display();
    }

    return 0;
}