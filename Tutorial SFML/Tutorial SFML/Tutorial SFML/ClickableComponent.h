#pragma once
#include "Component.h"
#include "EventHandler.h"
#include "Event.h"
#include "SpriteRenderer.h"
#include "Transform.h"

class ClickableComponent : public Component {
public:
    ClickableComponent(EventHandler* handler, Transform* transform, SpriteRenderer* spriteRenderer);
    ~ClickableComponent();

    Event<> onClick;
    const std::type_index GetType() override;
    void RenderTest(sf::RenderWindow* window);
private:
    Transform* _transform;
    SpriteRenderer* _spriteRenderer;
    void OnGlobalClick(sf::Vector2f pos);
};
