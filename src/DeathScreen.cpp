#include"Header/deathscreen.h"
#include<SFML/Graphics.hpp>
#include <iostream>

DeathScreen::DeathScreen(float width , float height)
:gameOverText(font),noText(font),yesText(font),restartText(font)
{
    if (!font.openFromFile("../src/Resources/Vipnagorgialla Bd.otf")) {
    std::cerr << "FAILED to load font!" << std::endl;
}
    overlay.setSize({width, height});
    overlay.setFillColor(sf::Color(0,0,0,160));

    noButton.setSize({100.f,50.f});
    sf::FloatRect bounds = noButton.getLocalBounds();
    noButton.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
    noButton.setPosition({width/2+120 , height/2+80});
    

    yesButton.setSize({100.f,50.f});
    bounds = yesButton.getLocalBounds();
    yesButton.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
    yesButton.setPosition({width/2-120 , height/2+80});
    

    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(40);
    bounds = gameOverText.getLocalBounds();
    gameOverText.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
    gameOverText.setFillColor(sf::Color(255,255,255));
    gameOverText.setPosition({width / 2.f, height / 2.f - 100.f});
    

    noText.setString("No");
    noText.setCharacterSize(20);
    bounds = noText.getLocalBounds();
    noText.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
    noText.setFillColor(sf::Color(255,255,255));
    noText.setPosition({width/2.f + 120.f , height/2.f + 80.f });

    yesText.setString("Yes");
    yesText.setCharacterSize(20);
    bounds = yesText.getLocalBounds();
    yesText.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
    yesText.setFillColor(sf::Color(255,255,255));
    yesText.setPosition({width/2.f - 120.f , height/2.f + 80.f });

    restartText.setString("RESTART GAME ?");
    restartText.setCharacterSize(25);
    bounds = restartText.getLocalBounds();
    restartText.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
    restartText.setFillColor(sf::Color(255,255,255));
    restartText.setPosition({width / 2.f, height / 2.f});

}
void DeathScreen::draw(sf::RenderWindow& window){
    sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
        if(yesButton.getGlobalBounds().contains(mousePos)){
            yesButton.setFillColor(sf::Color(70, 200, 100));
       }
       else{
        yesButton.setFillColor(sf::Color(50, 168, 82));
       }

       if (noButton.getGlobalBounds().contains(mousePos)){
        noButton.setFillColor(sf::Color(200, 70, 70));
       }
       else{
        noButton.setFillColor(sf::Color(168, 50, 50));
       }

    window.draw(overlay);
    window.draw(gameOverText);
    window.draw(restartText);
    window.draw(yesButton);
    window.draw(yesText);
    window.draw(noButton);
    window.draw(noText);
        
}

DeathScreenResult DeathScreen::getInput(sf::RenderWindow& window){
    if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
        sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
        if(yesButton.getGlobalBounds().contains(mousePos)){
            return DeathScreenResult::Restart;
       }
       else if (noButton.getGlobalBounds().contains(mousePos)){
        return DeathScreenResult::Exit;
       }
    }
        return DeathScreenResult::None;
}