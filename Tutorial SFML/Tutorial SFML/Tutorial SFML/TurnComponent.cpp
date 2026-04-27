#include "TurnComponent.h"

TurnComponent::TurnComponent(int numPlayers) : _currentPlayer(0), _numPlayers(numPlayers)
{

}

int TurnComponent::GetCurrentPlayer() const { return _currentPlayer; }

void TurnComponent::SetCurrentPlayer(int playerIndex) { _currentPlayer = playerIndex; }

int TurnComponent::GetNumPlayers() const { return _numPlayers; }

const std::type_index TurnComponent::GetType()
{
    return typeid(TurnComponent);
}
