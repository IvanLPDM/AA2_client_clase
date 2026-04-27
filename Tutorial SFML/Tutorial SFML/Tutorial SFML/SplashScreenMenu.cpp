#include "SplashScreenMenu.h"

SplashScreenMenu::SplashScreenMenu()
{
	AddComponent<SpriteRenderer>("Assets/Splashscreen/splash.png", sf::Color::White, false);
}

void SplashScreenMenu::Render(sf::RenderWindow* window)
{
	GetComponent<SpriteRenderer>()->Draw(window, GetComponent<Transform>());
}


