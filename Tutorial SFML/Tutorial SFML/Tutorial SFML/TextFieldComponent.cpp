#include "TextFieldComponent.h"

TextFieldComponent::TextFieldComponent(sf::Vector2f position, sf::Vector2f size, const std::string& placeholder, EventHandler* eventHandler)
    : _placeholder(placeholder), _focused(false), _cursorBlink(0.f), _cursorVisible(true), _text(FontManager::GetMainFont(), "", 24)
    , _eventHandler(eventHandler)
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

    _listenerClickId = _eventHandler->onClick.Subscribe([this](sf::Vector2f clickPos) 
        {
            OnGlobalClick(clickPos);
        });

    _listenerTextId = _eventHandler->onTextEntered.Subscribe([this](std::uint32_t unicode) 
        {
            HandleInput(unicode);
        });
}

TextFieldComponent::~TextFieldComponent()
{
    if (_eventHandler)
    {
        _eventHandler->onClick.UnSubscribe(_listenerClickId);

        _eventHandler->onTextEntered.UnSubscribe(_listenerTextId);
    }
}

void TextFieldComponent::Update(float deltaTime)
{
    if (_focused) {
        _cursorBlink += deltaTime;
        if (_cursorBlink >= 0.5f) {
            _cursorBlink = 0.f;
            _cursorVisible = !_cursorVisible;
        }
    }
}

void TextFieldComponent::Render(sf::RenderWindow* window)
{
    window->draw(_box);
    std::string toDisplay;
    if (_firstClick)
    {
        toDisplay = _input.empty() ? _placeholder : _input;

        if (_focused && _cursorVisible)
            toDisplay += "|";
    }
    else {
        toDisplay = _input;
        if (_focused && _cursorVisible)
            toDisplay += "|";
    }


    _text.setString(toDisplay);
        window->draw(_text);
}

// Escribe y borra la letra si es un delete
void TextFieldComponent::HandleInput(std::uint32_t unicode)
{
    if (!_focused) return;

    if (unicode == 8) 
    { 
        if (!_input.empty())
        {
            _input.pop_back();
        }
    }
    else if (unicode >= 32 && unicode < 128) 
    {
        _input += static_cast<char>(unicode);
    }
}

std::string TextFieldComponent::GetText() const { return _input; }

void TextFieldComponent::SetText(const std::string& text) { _input = text; }

const std::type_index TextFieldComponent::GetType() { return typeid(TextFieldComponent); }

void TextFieldComponent::OnGlobalClick(sf::Vector2f clickPos)
{
    if(_box.getGlobalBounds().contains(clickPos)) 
    {
        _focused = true;
        if (_firstClick) {
            _input.clear();
            _firstClick = false;
        }
    }
     else 
    {
        _focused = false;
    }
}
