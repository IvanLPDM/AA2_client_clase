#pragma once
#include <SFML/Graphics.hpp>
#include "Component.h"
#include "Event.h"
#include "EventHandler.h"
#include "FontManager.h"

class ButtonComponent : public Component
{
public:
    ButtonComponent(sf::Vector2f position, sf::Vector2f size, const std::string& label, EventHandler* eventHandler);
    ~ButtonComponent();

    void Render(sf::RenderWindow* window);

    Event<> onClick; 

    const std::type_index GetType() override;

private:
    void OnGlobalClick(sf::Vector2f clickPos);

    sf::Vector2f _size;
    sf::Vector2f _position;
    sf::RectangleShape _shape;
    sf::Text _label;

    uint64_t _listenerClickId = 0;
    EventHandler* _eventHandler = nullptr;
};

