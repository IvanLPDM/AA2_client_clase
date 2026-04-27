#pragma once
#include <SFML/Graphics.hpp>
#include "Component.h"
#include "FontManager.h"

class NormalTextComponent : public Component
{
public:
    NormalTextComponent(sf::Vector2f position, sf::Vector2f size, const std::string& text);
    ~NormalTextComponent() = default;

    void Update(float deltaTime);
    void Render(sf::RenderWindow* window);

    void SetText(const std::string& text);
    const std::type_index GetType() override;

private:
    sf::RectangleShape _box;
    sf::Text _text;
};
