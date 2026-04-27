#include "RankingEntryComponent.h"
#include <sstream>
#include <iomanip>

RankingEntryComponent::RankingEntryComponent(int rank, const std::string& nickname, int points, int wins, int losses)
    : _rank(rank), _nickname(nickname), _points(points), _wins(wins), _losses(losses)
{

}

int RankingEntryComponent::GetRank() const { return _rank; }

std::string RankingEntryComponent::GetNickname() const { return _nickname; }

int RankingEntryComponent::GetPoints() const { return _points; }

int RankingEntryComponent::GetWins() const { return _wins; }

int RankingEntryComponent::GetLosses() const { return _losses; }

std::string RankingEntryComponent::FormatRow() const
{
    std::ostringstream ss;
    ss << "#" << std::left << std::setw(3) << _rank
       << std::setw(14) << _nickname
       << "Pts: " << std::setw(5) << _points
       << "V: " << std::setw(4) << _wins
       << "D: " << _losses;
    return ss.str();
}

const std::type_index RankingEntryComponent::GetType()
{
    return typeid(RankingEntryComponent);
}
