

#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Toggle {
public:

    sf::RectangleShape box;   // 20x20 square — green when on, grey when off
    sf::Text           label; // Text shown to the right of the box
    bool               on;    // Current state: true=enabled, false=disabled

    
    
    // px, py   = screen position of the checkbox square
    // lbl      = text label beside the box
    // font     = SFML font
    // initial  = starting on/off state (default: true = ON)
    
    Toggle(float px, float py,
        const std::string& lbl, sf::Font& font,
        bool initial = true)
        : on(initial), label(font, lbl)
    {
        // Checkbox square — 20x20 pixels
        box.setPosition({ px, py });          // SFML 3: brace-init
        box.setSize({ 20.f, 20.f });          // SFML 3: brace-init
        box.setOutlineColor(sf::Color::White);
        box.setOutlineThickness(2);
        // Fill color is applied in draw() each frame based on 'on'

        // Text label — 28px to the right (20px box + 8px gap)
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