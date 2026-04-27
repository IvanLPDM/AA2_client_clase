#pragma once
#include "Component.h"
#include <string>

class PlayerInfoComponent : public Component
{
public:
    PlayerInfoComponent(int playerIndex, const std::string& nickname = "");

    int GetPlayerIndex() const;
    std::string GetNickname() const;
    int GetScore() const;
    bool IsSpectator() const;
    int GetFinishPosition() const;

    void SetNickname(const std::string& nickname);
    void AddScore(int points);
    void SetSpectator(bool spectator);
    void SetFinishPosition(int position);
    void SetEliminated(bool eliminated);
    bool IsEliminated() const;

    const std::type_index GetType() override;

private:
    int _playerIndex;
    std::string _nickname;
    int _score;
    bool _isSpectator;
    int _finishPosition; 
    bool _isEliminated = false;
};
