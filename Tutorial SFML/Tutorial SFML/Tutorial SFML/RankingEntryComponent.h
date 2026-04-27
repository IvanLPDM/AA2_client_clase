#pragma once
#include "Component.h"
#include <string>

class RankingEntryComponent : public Component
{
public:
    RankingEntryComponent(int rank, const std::string& nickname, int points, int wins, int losses);

    int GetRank() const;
    std::string GetNickname() const;
    int GetPoints() const;
    int GetWins() const;
    int GetLosses() const;

    // Returns the row formatted for display
    std::string FormatRow() const;

    const std::type_index GetType() override;

private:
    int _rank;
    std::string _nickname;
    int _points;
    int _wins;
    int _losses;
};
