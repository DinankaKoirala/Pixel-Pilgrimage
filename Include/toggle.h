

#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Toggle {
public:

    sf::RectangleShape box;   
    sf::Text           label; 
    bool               on;    

    Toggle(float px, float py,
        const std::string& lbl, sf::Font& font,
        bool initial = true)
        : on(initial), label(font, lbl)
    {
        // Checkbox square — 20x20 pixels
        box.setPosition({ px, py });          
        box.setSize({ 20.f, 20.f });        
        box.setOutlineColor(sf::Color::White);
        box.setOutlineThickness(2);

        label.setCharacterSize(13);
        label.setFillColor(sf::Color::White);
        label.setPosition({ px + 28.f, py });
    }

 

   
    void click(sf::Vector2f p) {

        if (box.getGlobalBounds().contains(p))
            on = !on;   // Flip: true→false or false→true
    }

    

    
    void draw(sf::RenderWindow& win) {
        box.setFillColor(on ? sf::Color(80, 200, 80)
            : sf::Color(60, 60, 60));
        win.draw(box);
        win.draw(label);
    }
};
