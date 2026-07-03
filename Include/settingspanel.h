
#pragma once
#include <SFML/Graphics.hpp>
#include "button.h"
#include "slider.h"
#include "toggle.h"

struct SettingsPanel {

    bool visible;  // true = panel is drawn and accepts input

    // Background
    sf::RectangleShape panel;
    sf::Text           title;

    // Sliders
    Slider musicVol;    // Background music level
    Slider sfxVol;      // Sound effects level
    Slider brightness;  // Screen brightness
    Slider contrast; //contrast

    // Toggles
    Toggle fullscreen;  // Fullscreen mode on/off
    Toggle showFPS;     // FPS counter on/off
    Toggle particles;   // Particle effects on/off

    // Difficulty row
    sf::Text diffLabel;
    Button   btnEasy;
    Button   btnMed;
    Button   btnHard;

    // Close
    Button btnClose;

    SettingsPanel(sf::Font& font)
        : visible(false),
        title(font, "SETTINGS"),
        diffLabel(font, "Difficulty:"),

        // Sliders — each offset down the panel by 60px
        musicVol(312 + 50, 184 + 80, 300, "Music Volume", font, 0.7f),
        sfxVol(312 + 50, 184 + 140, 300, "SFX Volume", font, 0.8f),
        brightness(312 + 50, 184 + 200, 300, "Brightness", font, 0.6f),
        contrast(312 + 50, 184 + 250, 300, "contrast", font, 0.6f),

        // Toggles — stacked 30px apart
        fullscreen(312 + 50, 184 + 270, "Fullscreen", font, false),
        showFPS(312 + 50, 184 + 300, "Show FPS", font, true),
        particles(312 + 50, 184 + 330, "Particles", font, true),

        // Difficulty buttons — sit side by side
        btnEasy(312 + 150, 184 + 355, 60, 24, "Easy", font, sf::Color(40, 120, 40), sf::Color(60, 180, 60)),
        btnMed(312 + 220, 184 + 355, 60, 24, "Medium", font, sf::Color(120, 100, 20), sf::Color(200, 160, 30)),
        btnHard(312 + 290, 184 + 355, 60, 24, "Hard", font, sf::Color(120, 30, 30), sf::Color(200, 50, 50)),

        // Close button — centered at panel bottom (below difficulty row)
        btnClose(312 + 150, 184 + 395, 100, 34, "CLOSE", font, sf::Color(80, 30, 30), sf::Color(160, 50, 50))
    {
        //Panel background rectangle
        panel.setPosition({ 312.f, 184.f });   // SFML 3: brace-init
        panel.setSize({ 400.f, 440.f });
        panel.setFillColor(sf::Color(20, 20, 40, 230));    // Dark blue, 90% opaque
        panel.setOutlineColor(sf::Color(180, 140, 60));    // Gold border
        panel.setOutlineThickness(3);

        //SETTINGS" title
        title.setCharacterSize(22);
        title.setFillColor(sf::Color(255, 220, 80));  // Gold text

        // Center title horizontally inside the 400px-wide panel
        
        float tw = title.getLocalBounds().size.x;
        title.setPosition({ 312.f + 200.f - tw / 2.f, 184.f + 14.f });

        // ---- "Difficulty:" static label ----
        diffLabel.setCharacterSize(13);
        diffLabel.setFillColor(sf::Color::White);
        diffLabel.setPosition({ 312.f + 50.f, 184.f + 360.f });
    }

    
    // handleClick()
    //   Route a mouse click to the correct widget.
    //   Returns true if the panel was closed (CLOSE button hit).
    //
    //   mp = mouse position at moment of click
    
    bool handleClick(sf::Vector2f mp) {
        fullscreen.click(mp);
        showFPS.click(mp);
        particles.click(mp);

        if (btnClose.contains(mp)) {
            visible = false;
            return true;   // Signal to caller: panel closed
        }
        return false;
    }

    // ---------------------------------------------------------------
    // handleDrag()
    //   Must be called every frame while mouse button is held.
    //   Routes drag to all three sliders.
    //
    //   mouse   = current mouse position
    //   pressed = true if left mouse button is currently held
    // ---------------------------------------------------------------
    void handleDrag(sf::Vector2f mouse, bool pressed) {
        musicVol.drag(mouse, pressed);
        sfxVol.drag(mouse, pressed);
        brightness.drag(mouse, pressed);
    }


    void draw(sf::RenderWindow& win, sf::Vector2f mouse) {
        win.draw(panel);
        win.draw(title);

        musicVol.draw(win);
        sfxVol.draw(win);
        brightness.draw(win);
        contrast.draw(win);

        fullscreen.draw(win);
        showFPS.draw(win);
        particles.draw(win);

        win.draw(diffLabel);
        btnEasy.draw(win, mouse);
        btnMed.draw(win, mouse);
        btnHard.draw(win, mouse);

        btnClose.draw(win, mouse);
    }
};