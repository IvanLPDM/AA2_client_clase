#include "PlayerInfoComponent.h"

PlayerInfoComponent::PlayerInfoComponent(int playerIndex, const std::string& nickname)
    : _playerIndex(playerIndex), _nickname(nickname), _score(0), _isSpectator(false), _finishPosition(0)
{

}

int PlayerInfoComponent::GetPlayerIndex() const { return _playerIndex; }

std::string PlayerInfoComponent::GetNickname() const { return _nickname; }

int PlayerInfoComponent::GetScore() const { return _score; }

bool PlayerInfoComponent::IsSpectator() const { return _isSpectator; }

int PlayerInfoComponent::GetFinishPosition() const { return _finishPosition; }

void PlayerInfoComponent::SetNickname(const std::string& nickname) { _nickname = nickname; }

void PlayerInfoComponent::AddScore(int points) { _score += points; }

void PlayerInfoComponent::SetSpectator(bool spectator) { _isSpectator = spectator; }

void PlayerInfoComponent::SetFinishPosition(int position) { _finishPosition = position; }

void PlayerInfoComponent::SetEliminated(bool eliminated) { _isEliminated = eliminated; }

bool PlayerInfoComponent::IsEliminated() const { return _isEliminated; }

const std::type_index PlayerInfoComponent::GetType()
{
    return typeid(PlayerInfoComponent);
}
