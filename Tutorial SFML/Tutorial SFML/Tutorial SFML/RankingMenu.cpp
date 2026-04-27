#include "RankingMenu.h"
#include <sstream>
#include <iomanip>

static std::string FormatRankRow(int rank, const std::string& nick, int pts, int wins, int losses)
{
    std::ostringstream ss;
    ss << "#" << std::left << std::setw(3) << rank
       << std::setw(14) << nick
       << "Pts: " << std::setw(5) << pts
       << "V: " << std::setw(4) << wins
       << "D: " << losses;
    return ss.str();
}

RankingMenu::RankingMenu(EventHandler* eventHandler, Client* client)
    : _client(client)
{
    _rowLabels.fill(nullptr);

    // Title
    _titleLabel = AddComponent<NormalTextComponent>(
    sf::Vector2f(10.f, 5.f), sf::Vector2f(ROW_WIDTH, ROW_HEIGHT), "=== RANKING ===");

    // Header
    _headerLabel = AddComponent<NormalTextComponent>(
    sf::Vector2f(ROW_X, HEADER_Y), sf::Vector2f(ROW_WIDTH, ROW_HEIGHT), "Pos  Jugador          Pts     V     D");

    // Estado
    _statusLabel = AddComponent<NormalTextComponent>(
    sf::Vector2f(ROW_X, FIRST_ROW_Y), sf::Vector2f(ROW_WIDTH, ROW_HEIGHT), "Cargando ranking...");

    // top 10 jugadores
    for (int i = 0; i < MAX_TOP_ROWS; ++i)
    {
        float y = FIRST_ROW_Y + i * ROW_HEIGHT;
        _rowLabels[i] = AddComponent<NormalTextComponent>(sf::Vector2f(ROW_X, y), sf::Vector2f(ROW_WIDTH, ROW_HEIGHT), "");
    }

    float sepY = FIRST_ROW_Y + MAX_TOP_ROWS * ROW_HEIGHT;
    _separatorLabel = AddComponent<NormalTextComponent>(sf::Vector2f(ROW_X, sepY), sf::Vector2f(ROW_WIDTH, ROW_HEIGHT), "");

    _playerRowLabel = AddComponent<NormalTextComponent>(sf::Vector2f(ROW_X, sepY + ROW_HEIGHT), sf::Vector2f(ROW_WIDTH, ROW_HEIGHT), "");

    // Back button
    _backButton = AddComponent<ButtonComponent>(sf::Vector2f(250.f, 650.f), sf::Vector2f(220.f, 50.f), "Volver al Lobby", eventHandler);

    _backButton->onClick.Subscribe([this]() { onReturn.Invoke(); });

    _client->ClearRankingData();
    _client->RequestRanking();
}

RankingMenu::~RankingMenu() {}

void RankingMenu::Update(float deltaTime)
{
    Event<> noop;
    _client->HandleServerMessages(noop);

    if (_client->HasRankingData() && !_displayed)
    {
        RefreshDisplay();
        _displayed = true;
    }
}

void RankingMenu::RefreshDisplay()
{
    const auto& entries = _client->GetRankingEntries();

    // te dice si estas por debajodel top 10
    bool hasPlayerRow = !entries.empty() && (int)entries.size() > MAX_TOP_ROWS;

    int topCount = hasPlayerRow ? MAX_TOP_ROWS : (int)entries.size();

    _statusLabel->SetText("");

    // llenar topm 10
    for (int i = 0; i < MAX_TOP_ROWS; ++i)
    {
        if (i < topCount)
        {
            const RankingEntry& e = entries[i];
            _rowLabels[i]->SetText(FormatRankRow(e.rank, e.nickname, e.points, e.wins, e.losses));
        }
        else
        {
            _rowLabels[i]->SetText("");
        }
    }

    // Player debajo de top 10
    if (hasPlayerRow)
    {
        _separatorLabel->SetText("...");
        const RankingEntry& e = entries.back();
        _playerRowLabel->SetText(FormatRankRow(e.rank, e.nickname, e.points, e.wins, e.losses));
    }
    else
    {
        _separatorLabel->SetText("");
        _playerRowLabel->SetText("");
    }
}

void RankingMenu::Render(sf::RenderWindow* window)
{
    _titleLabel->Render(window);
    _headerLabel->Render(window);
    _statusLabel->Render(window);

    for (int i = 0; i < MAX_TOP_ROWS; ++i)
    {
        if (_rowLabels[i])
            _rowLabels[i]->Render(window);
    }

    _separatorLabel->Render(window);
    _playerRowLabel->Render(window);
    _backButton->Render(window);
}
