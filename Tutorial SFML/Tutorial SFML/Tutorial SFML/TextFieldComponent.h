#pragma once
#include <SFML/Graphics.hpp>
#include "Component.h"
#include "Window.h"
#include "EventHandler.h"
#include "FontManager.h"

class TextFieldComponent : public Component
{
public:
    TextFieldComponent(sf::Vector2f position, sf::Vector2f size, const std::string& placeholder, EventHandler* eventHandler);
    ~TextFieldComponent();

    void Update(float deltaTime);
    void Render(sf::RenderWindow* window);
    void HandleInput(std::uint32_t unicode);
    std::string GetText() const;
    void SetText(const std::string& text);

    const std::type_index GetType() override;

private:
    void OnGlobalClick(sf::Vector2f clickPos);

    sf::RectangleShape _box;
    sf::Text _text;

    std::string _input;
    std::string _placeholder;

    bool _focused;
    float _cursorBlink;
    bool _cursorVisible;
    bool _firstClick = true;

    //For clearing memory in destructor
    uint64_t _listenerClickId;
    uint64_t _listenerTextId;
    EventHandler* _eventHandler;

};

