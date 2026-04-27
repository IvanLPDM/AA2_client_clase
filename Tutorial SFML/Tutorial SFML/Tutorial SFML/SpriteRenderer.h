#pragma once
#include <SFML/Graphics.hpp>
#include "Component.h"
#include "Transform.h"
#include "Enums.hpp"

class SpriteRenderer : public Component 
{
public:
	SpriteRenderer(const std::string& texturePath, sf::Color color, bool centerOrigin);

	void Draw(sf::RenderWindow* window, Transform* transform);

	sf::Sprite GetSprite();

	const std::type_index GetType() override;

private:
	sf::Texture _texture;
	sf::Sprite _sprite;
};