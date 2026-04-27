#pragma once
#include "Menu.h"
#include "ButtonComponent.h"
#include "TextFieldComponent.h"
#include "NormalTextComponent.h"
#include "SpriteRenderer.h"
#include "EventHandler.h"
#include "Client.h"

class MatchmakingMenu : public Menu
{
public:
    MatchmakingMenu(EventHandler* eventHandler, Client* client);

    void Update(float deltaTime);
    void Render(sf::RenderWindow* window);
    ButtonComponent* GetCreateRoomButton();
    ButtonComponent* GetJoinRoomButton();
    std::string GetCreateIDText();
    std::string GetJoinIDText();
    Event<> onStartMatch;
    Event<> onShowRanking;

private:
    TextFieldComponent* _idCreateRoomField;
    TextFieldComponent* _idJoinRoomField;
    ButtonComponent* _createRoomButton;
    ButtonComponent* _joinRoomButton;
    ButtonComponent* _rankingButton;
    NormalTextComponent* _roomInfoLabel;
    Client* _client;
};

