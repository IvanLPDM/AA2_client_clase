	#include "MatchmakingMenu.h"
#include <iostream>

MatchmakingMenu::MatchmakingMenu(EventHandler* eventHandler, Client* client)
{
	AddComponent<SpriteRenderer>("Assets/Splashscreen/matchmaking.png", sf::Color::White, false);

	_idCreateRoomField = AddComponent<TextFieldComponent>(sf::Vector2f(160, 300), sf::Vector2f(400, 50), "ID create room", eventHandler);

	_idJoinRoomField = AddComponent<TextFieldComponent>(sf::Vector2f(160, 450), sf::Vector2f(400, 50), "ID join room", eventHandler);

	_createRoomButton = AddComponent<ButtonComponent>(sf::Vector2f(170, 370), sf::Vector2f(180, 50), "Create", eventHandler);

	_joinRoomButton = AddComponent<ButtonComponent>(sf::Vector2f(170, 520), sf::Vector2f(180, 50), "Join", eventHandler);

	_roomInfoLabel = AddComponent<NormalTextComponent>(sf::Vector2f(160, 210), sf::Vector2f(400, 40), "");

	_rankingButton = AddComponent<ButtonComponent>(sf::Vector2f(390, 370), sf::Vector2f(170, 50), "Ranking", eventHandler);

	_client = client;

	_createRoomButton->onClick.Subscribe([this]() {
		std::cout << "[Client] Creating a room..." << std::endl;

		if (_client->CreateRoom(_idCreateRoomField->GetText()))
		{
			std::cout << "[Client] Create room request sent, waiting for server confirmation..." << std::endl;
			_client->StartListeningForPeers();
		}
		else
		{
			std::cout << "[Client] Failed to create room." << std::endl;
		}
		});

	_joinRoomButton->onClick.Subscribe([this]() {
		std::cout << "[Client] Join room requested..." << std::endl;

		if (_client->JoinRoom(GetJoinIDText())) {
			_client->StartListeningForPeers();
			std::cout << "[Client] Join packet sent. Waiting for response..." << std::endl;
		}
		else {
			std::cout << "[Client] Failed to send join request." << std::endl;
		}
		});

	_rankingButton->onClick.Subscribe([this]() {
		onShowRanking.Invoke();
		});
}

void MatchmakingMenu::Update(float deltaTime)
{
	_idCreateRoomField->Update(deltaTime);
	_idJoinRoomField->Update(deltaTime);

	_client->UpdateP2PConnections();
	_client->HandleServerMessages(onStartMatch);

	// Actualizar num de personas en la room
	if (_client->GetRoomPlayerCount() > 0)
	{
		std::string text = "Sala: " + _client->GetRoomID()
			+ " (" + std::to_string(_client->GetRoomPlayerCount())
			+ "/" + std::to_string(_client->GetRoomMaxPlayers()) + ")";
		_roomInfoLabel->SetText(text);
	}
	else
	{
		_roomInfoLabel->SetText("");
	}
}

void MatchmakingMenu::Render(sf::RenderWindow* window)
{
	GetComponent<SpriteRenderer>()->Draw(window, GetComponent<Transform>());
	_roomInfoLabel->Render(window);
	_idCreateRoomField->Render(window);
	_idJoinRoomField->Render(window);
	_createRoomButton->Render(window);
	_joinRoomButton->Render(window);
	_rankingButton->Render(window);
}

ButtonComponent* MatchmakingMenu::GetCreateRoomButton() { return _createRoomButton; }

ButtonComponent* MatchmakingMenu::GetJoinRoomButton() { return _joinRoomButton; }

std::string MatchmakingMenu::GetJoinIDText() { return _idJoinRoomField->GetText(); }

std::string MatchmakingMenu::GetCreateIDText() { return _idCreateRoomField->GetText(); }
