#pragma once
#include "Menu.h"
#include "SpriteRenderer.h"

class SplashScreenMenu : public Menu
{
public:
	SplashScreenMenu();

	void Render(sf::RenderWindow* window);
};

