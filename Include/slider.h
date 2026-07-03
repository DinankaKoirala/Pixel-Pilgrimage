

#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <algorithm>   // std::max, std::min — for clamping value to [0,1]

class Slider {
public:

    sf::RectangleShape track;
    sf::RectangleShape thumb;  
    sf::Text           label;

    float value;   
    float x, y, w; 

    // px, py = top-left position on screen
    // pw     = total width of the slider in pixels
    // lbl    = label string shown above
    // font   = SFML font
    // val    = initial value [0..1], default 0.7 (70%)

    Slider(float px, float py, float pw,
        const std::string& lbl, sf::Font& font,
        float val = 0.7f)
        : label(font, lbl), value(val), x(px), y(py), w(pw)
    {
          // Track bar
        track.setPosition({ px, py + 10.f });
        track.setSize({ pw, 6.f });
        track.setFillColor(sf::Color(80, 80, 80));
        track.setOutlineColor(sf::Color::White);
        track.setOutlineThickness(1);

        //Thumb handle
        thumb.setSize({ 14.f, 20.f });
        thumb.setFillColor(sf::Color(100, 200, 255));
        thumb.setOutlineColor(sf::Color::White);
        thumb.setOutlineThickness(1);
        updateThumb();   // Snap to initial value position

        // Label 
        label.setCharacterSize(13);
        label.setFillColor(sf::Color::White);
        label.setPosition({ px, py - 18.f });
    }


    

    void updateThumb() {
        thumb.setPosition({ x + value * (w - 14.f), y + 3.f });
        // y+3 vertically centers the 20px thumb over the track at y+10 with height 6
    }

    bool drag(sf::Vector2f mouse, bool pressed) {
        if (pressed && track.getGlobalBounds().contains(mouse)) {
            value = std::max(0.f, std::min(1.f, (mouse.x - x) / (w - 14.f)));
            updateThumb();
            return true;
        }
        return false;
    }

 
    void draw(sf::RenderWindow& win) {
        win.draw(track);
        win.draw(thumb);
        win.draw(label);
    }
};
