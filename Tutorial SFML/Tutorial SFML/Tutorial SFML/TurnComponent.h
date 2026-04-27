#pragma once
#include "Component.h"

class TurnComponent : public Component
{
public:
    explicit TurnComponent(int numPlayers);

    int GetCurrentPlayer() const;
    void SetCurrentPlayer(int playerIndex);
    int GetNumPlayers() const;

    const std::type_index GetType() override;

private:
    int _currentPlayer;
    int _numPlayers;
};
