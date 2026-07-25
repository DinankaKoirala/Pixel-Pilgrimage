#include "WinScreen.h"
#include <iostream>

WinScreen::WinScreen(float width, float height, const std::string& fontPath)
    : width(width), height(height),
      winTitle(font, "YOU WIN!", 72),
      winSubtitle(font, "Congratulations! You completed the game!", 28),
      btnBackText(font, "BACK TO MENU", 22),
      btnExitText(font, "EXIT", 22),
      princessHead(18.f)
{
    if (!font.openFromFile(fontPath)) {
        std::cerr << "FAILED to load font for WinScreen!" << std::endl;
    }

    overlay.setSize({ width, height });
    overlay.setFillColor(sf::Color(0, 0, 0, 200));

    winTitle.setFillColor(sf::Color(255, 215, 0));
    winTitle.setOutlineColor(sf::Color::Black);
    winTitle.setOutlineThickness(4.f);
    sf::FloatRect tb = winTitle.getLocalBounds();
    winTitle.setOrigin({ tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f });
    winTitle.setPosition({ width / 2.f, 130.f });

    winSubtitle.setFillColor(sf::Color::White);
    tb = winSubtitle.getLocalBounds();
    winSubtitle.setOrigin({ tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f });
    winSubtitle.setPosition({ width / 2.f, 210.f });

    winBanner.setSize({ 300.f, 80.f });
    winBanner.setFillColor(sf::Color(255, 215, 0));
    winBanner.setOutlineColor(sf::Color::Black);
    winBanner.setOutlineThickness(4.f);
    winBanner.setOrigin({ 150.f, 40.f });
    winBanner.setPosition({ width / 2.f, 130.f });

    trophyCup.setPointCount(4);
    trophyCup.setPoint(0, { 0.f, 0.f });
    trophyCup.setPoint(1, { 100.f, 0.f });
    trophyCup.setPoint(2, { 75.f, 75.f });
    trophyCup.setPoint(3, { 25.f, 75.f });
    trophyCup.setFillColor(sf::Color(255, 215, 0));
    trophyCup.setOutlineColor(sf::Color::Black);
    trophyCup.setOutlineThickness(3.f);
    trophyCup.setPosition({ width / 4.f - 50.f, height / 2.f - 30.f });

    knightBody.setSize({ 50.f, 90.f });
    knightBody.setFillColor(sf::Color(180, 180, 180));
    knightBody.setOutlineColor(sf::Color::Black);
    knightBody.setOutlineThickness(2.f);
    knightBody.setOrigin({ 25.f, 45.f });
    knightBody.setPosition({ width / 2.f, height / 2.f + 60.f });

    princessDress.setPointCount(3);
    princessDress.setPoint(0, { 0.f, 0.f });
    princessDress.setPoint(1, { -50.f, 100.f });
    princessDress.setPoint(2, { 50.f, 100.f });
    princessDress.setFillColor(sf::Color(255, 105, 180));
    princessDress.setOutlineColor(sf::Color::Black);
    princessDress.setOutlineThickness(2.f);
    princessDress.setPosition({ width * 0.75f, height / 2.f + 20.f });

    princessHead.setFillColor(sf::Color(255, 220, 180));
    princessHead.setOutlineColor(sf::Color::Black);
    princessHead.setOutlineThickness(2.f);
    princessHead.setOrigin({ 18.f, 18.f });
    princessHead.setPosition({ width * 0.75f, height / 2.f - 15.f });

    btnBack.setSize({ 220.f, 60.f });
    btnBack.setPosition({ width / 2.f - 260.f, 550.f });
    btnBack.setFillColor(sf::Color(50, 168, 82));
    btnBack.setOutlineColor(sf::Color::White);
    btnBack.setOutlineThickness(3.f);

    btnBackText.setFillColor(sf::Color::White);
    tb = btnBackText.getLocalBounds();
    btnBackText.setOrigin({ tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f });
    btnBackText.setPosition({ width / 2.f - 260.f + 110.f, 550.f + 30.f });

    btnExit.setSize({ 220.f, 60.f });
    btnExit.setPosition({ width / 2.f + 40.f, 550.f });
    btnExit.setFillColor(sf::Color(168, 50, 50));
    btnExit.setOutlineColor(sf::Color::White);
    btnExit.setOutlineThickness(3.f);

    btnExitText.setFillColor(sf::Color::White);
    tb = btnExitText.getLocalBounds();
    btnExitText.setOrigin({ tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f });
    btnExitText.setPosition({ width / 2.f + 40.f + 110.f, 550.f + 30.f });
}

WinScreenResult WinScreen::run(sf::RenderWindow& window)
{
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
                return WinScreenResult::Exit;
            }

            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mousePressed->button != sf::Mouse::Button::Left) continue;

                sf::Vector2f clickPos = window.mapPixelToCoords(mousePressed->position);

                if (btnBack.getGlobalBounds().contains(clickPos))
                    return WinScreenResult::BackToMenu;

                if (btnExit.getGlobalBounds().contains(clickPos))
                {
                    window.close();
                    return WinScreenResult::Exit;
                }
            }
        }

        sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        btnBack.setFillColor(btnBack.getGlobalBounds().contains(mouse)
            ? sf::Color(70, 200, 100) : sf::Color(50, 168, 82));
        btnExit.setFillColor(btnExit.getGlobalBounds().contains(mouse)
            ? sf::Color(200, 70, 70) : sf::Color(168, 50, 50));

        window.clear(sf::Color(150, 220, 150));

        window.setView(window.getDefaultView());
        window.draw(overlay);
        window.draw(winBanner);
        window.draw(winTitle);
        window.draw(winSubtitle);
        window.draw(trophyCup);
        window.draw(knightBody);
        window.draw(princessDress);
        window.draw(princessHead);
        window.draw(btnBack);
        window.draw(btnBackText);
        window.draw(btnExit);
        window.draw(btnExitText);

        window.display();
    }

    return WinScreenResult::Exit;
}
