#pragma once
#include "Component.h"

class PlayerComponent : public Component
{
public:
	PlayerComponent(int playerID);

	int  GetPlayerID() const;
	void SetPlayerID(int playerID);
	const std::type_index GetType() override;

private:
	int _playerID;
};
