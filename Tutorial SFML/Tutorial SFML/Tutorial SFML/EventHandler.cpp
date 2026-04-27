#include "EventHandler.h"
#include <iostream>

void EventHandler::HandleEvent(const sf::Event& event, Window& window)
{
    if (event.is<sf::Event::Closed>())
    {
        window.Close();
    }

    //Keyboard
    if (const sf::Event::KeyPressed* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        switch (keyPressed->code) {
        case sf::Keyboard::Key::Escape:
            //std::cout << "Escape pressed, Stop the Game..." << std::endl;
            window.Close();
            break;
        default:
            break;
        }
    }

    //Escribir en teclado
    if (const sf::Event::TextEntered* textEntered = event.getIf<sf::Event::TextEntered>()) 
    {
        //std::cout << "OnTextEntered Event Invoked..." << std::endl;

        onTextEntered.Invoke(textEntered->unicode);
    }

    //Mouse
    if (const sf::Event::MouseButtonPressed* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) 
    {
        switch (mousePressed->button)
        {
        case sf::Mouse::Button::Left:
            //std::cout << "Left mouse button pressed at: " << mousePressed->position.x << ", " << mousePressed->position.y << "onClick Event invoked" << std::endl;
            onClick.Invoke(sf::Vector2f(mousePressed->position.x, mousePressed->position.y));
            break;
        default:
            break;
        }
    }
}
