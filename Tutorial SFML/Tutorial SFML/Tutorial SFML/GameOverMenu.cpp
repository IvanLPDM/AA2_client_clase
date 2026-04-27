#include "GameOverMenu.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

static const char* PositionLabel(int pos)
{
    switch (pos)
    {
        case 1: return "1ro";
        case 2: return "2do";
        case 3: return "3ro";
        default: return "4to";
    }
}

GameOverMenu::GameOverMenu(EventHandler* eventHandler, Client* client) : _client(client)
{
    _rowLabels.fill(nullptr);

    _titleLabel = AddComponent<NormalTextComponent>(sf::Vector2f(0.f, 30.f), sf::Vector2f(720.f, 60.f), "=== FIN DE PARTIDA ===");

    _subtitleLabel = AddComponent<NormalTextComponent>(sf::Vector2f(ROW_X, 130.f), sf::Vector2f(ROW_WIDTH, 36.f), "Pos   Jugador");

    for (int i = 0; i < MAX_ROWS; ++i)
    {
        float y = FIRST_ROW_Y + i * ROW_HEIGHT;
        _rowLabels[i] = AddComponent<NormalTextComponent>(sf::Vector2f(ROW_X, y), sf::Vector2f(ROW_WIDTH, ROW_HEIGHT - 4.f), "");
    }

    _exitMenuButton = AddComponent<ButtonComponent>(
        sf::Vector2f(120.f, 500.f), sf::Vector2f(220.f, 50.f), "Volver al Menu", eventHandler);

    _exitGameButton = AddComponent<ButtonComponent>(
        sf::Vector2f(390.f, 500.f), sf::Vector2f(220.f, 50.f), "Salir del Juego", eventHandler);

    _exitMenuButton->onClick.Subscribe([this]() { onReturnMenu.Invoke(); });
    _exitGameButton->onClick.Subscribe([this]() { onExitGame.Invoke(); });

    BuildStandings();
}

void GameOverMenu::BuildStandings()
{
    const auto& results = _client->GetPendingGameResults();
    if (results.empty()) return;

    std::vector<GameResultEntry> sorted = results;
    std::sort(sorted.begin(), sorted.end(),
        [](const GameResultEntry& a, const GameResultEntry& b) {
            return a.finishPosition < b.finishPosition;
        });

    int numPlayers = static_cast<int>(sorted.size());

    for (int i = 0; i < MAX_ROWS && i < static_cast<int>(sorted.size()); ++i)
    {
        const auto& e = sorted[i];
        std::string posLabel;

        if (e.isDisconected)
            posLabel = "Elim";
        else
            posLabel = PositionLabel(e.finishPosition);

        std::ostringstream ss;
        ss << std::left << std::setw(6) << posLabel << e.nickname;
        _rowLabels[i]->SetText(ss.str());
    }
}

void GameOverMenu::Update(float deltaTime) {}

void GameOverMenu::Render(sf::RenderWindow* window)
{
    _titleLabel->Render(window);
    _subtitleLabel->Render(window);

    for (int i = 0; i < MAX_ROWS; ++i)
        if (_rowLabels[i]) _rowLabels[i]->Render(window);

    _exitMenuButton->Render(window);
    _exitGameButton->Render(window);
}
