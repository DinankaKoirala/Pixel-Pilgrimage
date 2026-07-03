#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Button {
    public:

    sf::RectangleShape box;    
    sf::Text           label;  
    sf::Color          normal;
    sf::Color          hover;  

    Button(float x, float y, float w, float h,
        const std::string& text, sf::Font& font,
        sf::Color nc = sf::Color(60, 60, 120),
        sf::Color hc = sf::Color(100, 100, 200))
        : normal(nc), hover(hc), label(font, text)
    {
        // Background box
        box.setPosition({ x, y });          
        box.setSize({ w, h });              
        box.setFillColor(nc);
        box.setOutlineColor(sf::Color::White);
        box.setOutlineThickness(2);

        // Text centered inside the box
        label.setCharacterSize(14);
        label.setFillColor(sf::Color::White);

        // To center text:
   
        sf::FloatRect tb = label.getLocalBounds();
        label.setOrigin({ tb.position.x + tb.size.x / 2.f,   
                          tb.position.y + tb.size.y / 2.f });
        label.setPosition({ x + w / 2.f, y + h / 2.f });
    }


    bool contains(sf::Vector2f p) {
  
        return box.getGlobalBounds().contains(p);
    }


    void draw(sf::RenderWindow& w, sf::Vector2f mouse) {
        box.setFillColor(contains(mouse) ? hover : normal);
        w.draw(box);
        w.draw(label);
    }
};
