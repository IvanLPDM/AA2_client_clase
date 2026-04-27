#include "PlayerComponent.h"

PlayerComponent::PlayerComponent(int playerID) : _playerID(playerID)
{
}

int PlayerComponent::GetPlayerID() const
{
	return _playerID;
}

void PlayerComponent::SetPlayerID(int playerID)
{
	if (_playerID == playerID) return;
	_playerID = playerID;
}

const std::type_index PlayerComponent::GetType()
{
	return typeid(PlayerComponent);
}
