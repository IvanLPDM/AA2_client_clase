#pragma once
#include "Menu.h"
#include "ButtonComponent.h"
#include "NormalTextComponent.h"
#include "EventHandler.h"
#include "Client.h"
#include "Event.h"
#include <array>

class GameOverMenu : public Menu
{
public:
    GameOverMenu(EventHandler* eventHandler, Client* client);
    void Update(float deltaTime);
    void Render(sf::RenderWindow* window);

    Event<> onExitGame;
    Event<> onReturnMenu;

private:
    static constexpr int   MAX_ROWS   = 4;
    static constexpr float ROW_HEIGHT = 60.f;
    static constexpr float ROW_X      = 80.f;
    static constexpr float ROW_WIDTH  = 560.f;
    static constexpr float FIRST_ROW_Y = 180.f;

    void BuildStandings();

    Client*              _client;
    NormalTextComponent* _titleLabel;
    NormalTextComponent* _subtitleLabel;
    std::array<NormalTextComponent*, MAX_ROWS> _rowLabels;
    ButtonComponent*     _exitMenuButton;
    ButtonComponent*     _exitGameButton;
};
