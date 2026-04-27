#pragma once
#include <SFML/Window/Event.hpp>
#include "Window.h"
#include "Event.h"

class EventHandler
{
public:
	void HandleEvent(const sf::Event& event, Window& window);

	Event<sf::Vector2f> onClick;
	Event<std::uint32_t> onTextEntered;
};

