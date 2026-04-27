#pragma once
#include "Menu.h"
#include "ButtonComponent.h"
#include "NormalTextComponent.h"
#include "RankingEntryComponent.h"
#include "EventHandler.h"
#include "Client.h"
#include <array>

class RankingMenu : public Menu
{
public:
    RankingMenu(EventHandler* eventHandler, Client* client);
    ~RankingMenu();

    void Update(float deltaTime);
    void Render(sf::RenderWindow* window);

    Event<> onReturn;

private:
    static constexpr int   MAX_TOP_ROWS  = 10;
    static constexpr float ROW_HEIGHT    = 46.f;
    static constexpr float ROW_X         = 10.f;
    static constexpr float ROW_WIDTH     = 700.f;
    static constexpr float HEADER_Y      = 55.f;
    static constexpr float FIRST_ROW_Y   = 110.f;

    void RefreshDisplay();

    Client* _client;
    NormalTextComponent* _titleLabel;
    NormalTextComponent* _headerLabel;
    NormalTextComponent* _statusLabel;
    NormalTextComponent* _separatorLabel;
    NormalTextComponent* _playerRowLabel;

    std::array<NormalTextComponent*, MAX_TOP_ROWS> _rowLabels;

    ButtonComponent* _backButton;

    bool _displayed = false; // true once ranking has been rendered at least once
};
