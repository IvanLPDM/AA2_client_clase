#include "NormalTextComponent.h"

NormalTextComponent::NormalTextComponent(sf::Vector2f position, sf::Vector2f size, const std::string& text)
    : _text(FontManager::GetMainFont(), "", 24)
{
    _box.setPosition(position);
    _box.setSize(size);
    _box.setFillColor(sf::Color(255, 255, 255, 200));
    _box.setOutlineThickness(2);
    _box.setOutlineColor(sf::Color::Black);

    _text.setFont(FontManager::GetMainFont());
    _text.setCharacterSize(24);
    _text.setPosition(sf::Vector2f(position.x + 10, position.y + 10));
    _text.setFillColor(sf::Color::Black);
    _text.setString(text);
}

void NormalTextComponent::Update(float deltaTime)
{
    
}

void NormalTextComponent::Render(sf::RenderWindow* window)
{
    window->draw(_box);
    window->draw(_text);
}

void NormalTextComponent::SetText(const std::string& text)
{
    _text.setString(text);
}

const std::type_index NormalTextComponent::GetType()
{
    return typeid(NormalTextComponent);
}
