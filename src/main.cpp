#include <SFML/Graphics.hpp>
#include <iostream>
#include<cmath>
#include<vector>

#include "Header/deathscreen.h"

int main() {
	unsigned int width = 1280;
	unsigned int height = 720;
	sf::RenderWindow window(sf::VideoMode({ width, height }), "Brick Breaker");
	window.setFramerateLimit(60);

	DeathScreen deathscreen(static_cast<float>(width), static_cast<float>(height));


	//Game Loop
	while (window.isOpen()) {

		while (const std::optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}
			else if (const auto* keypress = event->getIf<sf::Event::KeyPressed>()) {
				if (keypress->scancode == sf::Keyboard::Scancode::Escape) {
					window.close();
				}
			}
		}
        DeathScreenResult result = deathscreen.getInput(window);
        if (result == DeathScreenResult::Exit){
                window.close();
            }

     
            window.clear();
			deathscreen.draw(window);
		//Draw 
		window.display();

	}

}