#include "Gameplay.h"
#include "Transform.h"
#include <iostream>
#include <sstream>
#include <algorithm>

static constexpr float MARK_PADDING = 15.f;
static constexpr float MARK_SIZE = Gameplay::CELL_SIZE - MARK_PADDING * 2.f;

static constexpr int POINTS_BY_POSITION[Gameplay::MAX_PLAYERS] = {
    Gameplay::POINTS_1ST,
    Gameplay::POINTS_2ND,
    Gameplay::POINTS_3RD,
    Gameplay::POINTS_LAST
};

Gameplay::Gameplay(Client* client, int playerIndex, int numPlayers, EventHandler* eventHandler) : _client(client), _eventHandler(eventHandler)
    , _playerIndex(playerIndex), _numPlayers(numPlayers), _gameOver(false), _boardObject(nullptr), _timerLabel(nullptr), _turnLabel(nullptr)
{
    _playerEntities.fill(nullptr);
    _playerLabels.fill(nullptr);

    _boardComp = AddComponent<BoardComponent>();
    _turnComp  = AddComponent<TurnComponent>(numPlayers);
    _timerComp = AddComponent<TimerComponent>(TURN_DURATION);

    // una por jugador
    for (int i = 0; i < _numPlayers; ++i)
    {
        _playerEntities[i] = new GameObject();
        std::string name = (i == _playerIndex) ? _client->GetMyUsername() : "";
        _playerEntities[i]->AddComponent<PlayerInfoComponent>(i, name);
    }

    // Board image
    _boardObject = new GameObject();
    _boardObject->AddComponent<SpriteRenderer>("Assets/GameAssets/board_6x6.png", sf::Color::White, false);
    _boardObject->GetComponent<Transform>()->position = sf::Vector2f(BOARD_ORIGIN_X, BOARD_ORIGIN_Y);

    // Bottom UI labels
    _turnLabel = new GameObject();
    _turnLabel->AddComponent<NormalTextComponent>(
    sf::Vector2f(90.f, 640.f), sf::Vector2f(270.f, 36.f), "Turno: J1");

    _timerLabel = new GameObject();
    _timerLabel->AddComponent<NormalTextComponent>(
    sf::Vector2f(370.f, 640.f), sf::Vector2f(260.f, 36.f), "20s");

    SetupPlayerLabels();

    //Fichas de los jugadores
    const std::string markPaths[MAX_PLAYERS] = {
        "Assets/GameAssets/mark_p1.png",
        "Assets/GameAssets/mark_p2.png",
        "Assets/GameAssets/mark_p3.png",
        "Assets/GameAssets/mark_p4.png"
    };
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        if (!_markTextures[i].loadFromFile(markPaths[i]))
        std::cerr << "[Gameplay] Failed to load mark texture: " << markPaths[i] << '\n';
    }

    //para los clicks del raton
    _clickListenerID = _eventHandler->onClick.Subscribe([this](sf::Vector2f mousePos) {
        if (!IsMyTurn() || _gameOver) return;
        int row, col;
        if (GetCellFromMouse(mousePos, row, col))
        {
            if (_boardComp->IsCellEmpty(row, col))
            {
                PlaceMark(row, col, _playerIndex);
                BroadcastMove(row, col);
            }
        }
    });


    _client->SendUsername();

    std::cout << "[Gameplay] Started. Player index: " << _playerIndex << " | Players: " << _numPlayers << '\n';
}

Gameplay::~Gameplay()
{
    _eventHandler->onClick.UnSubscribe(_clickListenerID);

    delete _boardObject;
    delete _turnLabel;
    delete _timerLabel;

    for (auto* entity : _playerEntities) delete entity;
    for (auto* label : _playerLabels)  delete label;

    _client->ClearPeers();
}

void Gameplay::Update(float deltaTime)
{
    _client->UpdateP2PConnections();
    TimerSystem(deltaTime);
    NetworkSystem();
}

void Gameplay::Render(sf::RenderWindow* window)
{
    // Board image
    _boardObject->GetComponent<SpriteRenderer>()->Draw(
    window, _boardObject->GetComponent<Transform>());

    // casillas marcadas
    for (int r = 0; r < BOARD_SIZE; ++r)
    {
        for (int c = 0; c < BOARD_SIZE; ++c)
        {
            CellState cell = _boardComp->GetCell(r, c);
            if (cell != CellState::EMPTY)
            DrawMark(window, r, c, cell);
        }
    }

    if (IsMyTurn() && !_gameOver)
    {
        // Feedback de que te toca, con tu color de jugador
        sf::RectangleShape indicator(sf::Vector2f(BOARD_SIZE * CELL_SIZE, 4.f));
        indicator.setFillColor(PlayerIndexToColor(_playerIndex));
        indicator.setPosition(sf::Vector2f(BOARD_ORIGIN_X, BOARD_ORIGIN_Y - 6.f));
        window->draw(indicator);
    }

    // UI labels
    _turnLabel->GetComponent<NormalTextComponent>()->Render(window);
    _timerLabel->GetComponent<NormalTextComponent>()->Render(window);
    for (auto* label : _playerLabels)
    {
        if (label) label->GetComponent<NormalTextComponent>()->Render(window);
    }
}


void Gameplay::TimerSystem(float deltaTime)
{
    if (_gameOver) return;

    _timerComp->Tick(deltaTime);
    UpdateLabels();

    if (IsMyTurn() && _timerComp->HasExpired())
    {
        SkipTurn();
    }
}


void Gameplay::NetworkSystem()
{
    auto packetOpt = _client->WaitForPeerMessage(0.f);

    while (packetOpt.has_value())
    {
        sf::Packet original = packetOpt.value();
        sf::Packet packet   = original;

        int msgTypeInt;
        packet >> msgTypeInt;
        MessageType msgType = static_cast<MessageType>(msgTypeInt);

        switch (msgType)
        {
            case MessageType::PLAYER_PROFILE:
            {
                int index;
                std::string name;
                packet >> index >> name;

                if (index >= 0 && index < _numPlayers && _playerEntities[index])
                {
                    _playerEntities[index]->GetComponent<PlayerInfoComponent>()->SetNickname(name);
                    
                    auto senderIp = _client->GetLastSenderIp();
                    if (senderIp.has_value())
                    {
                        _client->SetPeerPlayerIndexByAddr(senderIp.value(),_client->GetLastSenderPort(),index);
                    }
                    UpdateLabels();
                    std::cout << "[Gameplay] Player " << index << " name: " << name << '\n';
                }
                break;
            }
            case MessageType::MOVE_REQUEST:
            {
                int row, col, actingPlayer;
                packet >> row >> col >> actingPlayer;

                if (actingPlayer != _playerIndex && _boardComp->IsCellEmpty(row, col))
                {
                    PlaceMark(row, col, actingPlayer);
                    std::cout << "[Gameplay] Player " << actingPlayer << " placed at (" << row << "," << col << ")\n";
                }
                break;
            }
            case MessageType::TURN_CHANGE:
            {
                int newTurn;
                packet >> newTurn;
                _turnComp->SetCurrentPlayer(newTurn);
                _timerComp->Reset();
                UpdateLabels();
                std::cout << "[Gameplay] Turn changed to player " << newTurn << '\n';
                break;
            }
            case MessageType::GAME_OVER:
            {
                if (!_gameOver)
                {
                    int loserIndex;
                    packet >> loserIndex;
                    _gameOver = true;

                    
                    int numEntries;
                    packet >> numEntries;
                    std::vector<GameResultEntry> results;
                    results.reserve(numEntries);
                    for (int i = 0; i < numEntries; ++i)
                    {
                        GameResultEntry e;
                        int elim = 0;
                        packet >> e.nickname >> e.finishPosition >> elim;
                        e.isDisconected = (elim != 0);
                        results.push_back(e);
                    }
                    _client->StoreGameResults(results);

                    if (loserIndex == _playerIndex)
                        onLoseMatch.Invoke();
                    else
                        onWinMatch.Invoke();
                }
                break;
            }
            case MessageType::PLAYER_DISCONNECTED:
            {
                int disconnectedIndex;
                packet >> disconnectedIndex;
                std::cout << "[Gameplay] Player " << disconnectedIndex << " disconnected.\n";

                // Saltar turno
                if (disconnectedIndex >= 0 && disconnectedIndex < _numPlayers
                    && _playerEntities[disconnectedIndex])
                {
                    auto* info = _playerEntities[disconnectedIndex]
                                     ->GetComponent<PlayerInfoComponent>();
                    if (info && !info->IsSpectator())
                    {
                        info->SetSpectator(true);
                        info->SetEliminated(true);
                        // Jugadores desconectados quedan ultimos
                        info->SetFinishPosition(_numPlayers);
                        if (_turnComp->GetCurrentPlayer() == disconnectedIndex)
                            AdvanceTurn();
                    }
                }
                break;
            }
            default:
                std::cout << "[Gameplay] Unknown message: " << msgTypeInt << '\n';
                break;
        }

        if (_playerIndex == 0)
            _client->BroadcastToPeers(original);

        packetOpt = _client->WaitForPeerMessage(0.f);
    }
}


void Gameplay::PlaceMark(int row, int col, int actingPlayer)
{
    CellState mark = PlayerIndexToCell(actingPlayer);
    _boardComp->SetCell(row, col, mark);

    std::cout << "[Gameplay] Mark placed at (" << row << "," << col << ") by player " << actingPlayer << '\n';

    if (CheckWin(row, col, mark))
    {
        HandlePlayerWin(actingPlayer);
        return;
    }

    if (_boardComp->IsFull())
    {
        std::cout << "[Gameplay] Board full — draw.\n";
        _gameOver = true;
        onLoseMatch.Invoke();
        return;
    }

    if (actingPlayer == _turnComp->GetCurrentPlayer())
        AdvanceTurn();
}

void Gameplay::SkipTurn()
{
    std::cout << "[Gameplay] Turn timeout — skipping player " << _turnComp->GetCurrentPlayer() << '\n';

    AdvanceTurn();

    sf::Packet packet;
    packet << static_cast<int>(MessageType::TURN_CHANGE) << _turnComp->GetCurrentPlayer();
    _client->BroadcastToPeers(packet);
}

void Gameplay::AdvanceTurn()
{
    _timerComp->Reset();

    int next = (_turnComp->GetCurrentPlayer() + 1) % _numPlayers;
    int checked = 0;

    // Skip espectadores
    while (IsSpectator(next) && checked < _numPlayers)
    {
        next = (next + 1) % _numPlayers;
        ++checked;
    }

    _turnComp->SetCurrentPlayer(next);
    UpdateLabels();
}

bool Gameplay::CheckWin(int row, int col, CellState mark) const
{
    auto countDir = [&](int dr, int dc) -> int
    {
        int count = 0;
        int r = row + dr, c = col + dc;
        while (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE && _boardComp->GetCell(r, c) == mark)
        {
            ++count;
            r += dr;
            c += dc;
        }
        return count;
    };

    // Mirar las 4 direcciones
    const int dx[] = {1, 0, 1,  1};
    const int dy[] = {0, 1, 1, -1};

    for (int i = 0; i < 4; ++i)
    {
        int total = 1 + countDir(dx[i], dy[i]) + countDir(-dx[i], -dy[i]);
        if (total >= WIN_LENGTH) return true;
    }
    return false;
}


void Gameplay::HandlePlayerWin(int playerIndex)
{
    auto* info = _playerEntities[playerIndex]->GetComponent<PlayerInfoComponent>();
    if (info->IsSpectator()) return;

    int position = CountSpectators() + 1;
    info->SetSpectator(true);
    info->SetFinishPosition(position);
    info->AddScore(POINTS_BY_POSITION[position - 1]);

    _playerEntities[playerIndex]->AddComponent<SpectatorComponent>();

    std::cout << "[Gameplay] Player " << playerIndex << " wins! Finish position: " << position << '\n';

    if (CountSpectators() >= _numPlayers - 1)
    {
        HandleGameEnd();
    }
    else
    {
        AdvanceTurn();
        UpdateLabels();
    }
}

void Gameplay::HandleGameEnd()
{
    _gameOver = true;

    // Encontrar al perdedor
    int loserIndex = -1;
    for (int i = 0; i < _numPlayers; ++i)
    {
        if (!IsSpectator(i)) { loserIndex = i; break; }
    }

    std::cout << "[Gameplay] Game over. Loser: player " << loserIndex << '\n';

    std::vector<GameResultEntry> results;
    for (int i = 0; i < _numPlayers; ++i)
    {
        if (!_playerEntities[i]) continue;
        auto* info = _playerEntities[i]->GetComponent<PlayerInfoComponent>();
        if (!info) continue;

        std::string name = info->GetNickname();
        if (name.empty()) name = "J" + std::to_string(i + 1);

        int pos = info->GetFinishPosition();
        if (pos == 0) pos = _numPlayers; 

        results.push_back({name, pos, info->IsEliminated()});
    }
    _client->StoreGameResults(results);

    sf::Packet packet;
    packet << static_cast<int>(MessageType::GAME_OVER) << loserIndex;
    packet << static_cast<int>(results.size());
    for (const auto& r : results)
        packet << r.nickname << r.finishPosition << static_cast<int>(r.isDisconected);
    _client->BroadcastToPeers(packet);

    if (loserIndex == _playerIndex)
        onLoseMatch.Invoke();
    else
        onWinMatch.Invoke();
}


void Gameplay::BroadcastMove(int row, int col)
{
    sf::Packet packet;
    packet << static_cast<int>(MessageType::MOVE_REQUEST) << row << col << _playerIndex;
    _client->BroadcastToPeers(packet);
}


void Gameplay::SetupPlayerLabels()
{
    static constexpr float LABEL_WIDTH  = 175.f;
    static constexpr float LABEL_HEIGHT = 36.f;
    static constexpr float LABEL_TOP    = 5.f;
    static constexpr float LABEL_GAP    = 180.f;

    const std::string symbols[MAX_PLAYERS] = {"[X]", "[O]", "[T]", "[C]"};

    for (int i = 0; i < _numPlayers; ++i)
    {
        _playerLabels[i] = new GameObject();
        _playerLabels[i]->AddComponent<NormalTextComponent>(
            sf::Vector2f(i * LABEL_GAP, LABEL_TOP),
            sf::Vector2f(LABEL_WIDTH, LABEL_HEIGHT),
            symbols[i] + " J" + std::to_string(i + 1));
    }
}

void Gameplay::UpdateLabels()
{
    int cur = _turnComp->GetCurrentPlayer();

    // Turn label
    if (_turnLabel)
    {
        std::string text = IsMyTurn() ? "Tu turno" : ("Turno: J" + std::to_string(cur + 1));
        _turnLabel->GetComponent<NormalTextComponent>()->SetText(text);
    }

    // Timer label
    if (_timerLabel)
    {
        int secs = static_cast<int>(_timerComp->GetRemaining()) + 1;
        _timerLabel->GetComponent<NormalTextComponent>()->SetText(std::to_string(secs) + "s");
    }

    // Player labels
    const std::string symbols[MAX_PLAYERS] = {"[X]", "[O]", "[T]", "[C]"};
    for (int i = 0; i < _numPlayers; ++i)
    {
        if (!_playerLabels[i] || !_playerEntities[i]) continue;

        auto* info = _playerEntities[i]->GetComponent<PlayerInfoComponent>();
        if (!info) continue;

        std::string name = info->GetNickname();
        if (name.empty()) name = "J" + std::to_string(i + 1);
        if (name.size() > 8) name = name.substr(0, 8);

        std::string label = symbols[i] + " " + name;
        if (info->IsSpectator())
            label += info->IsEliminated() ? " ELIM" : " WIN";
        if (i == cur)
            label += " *";

        _playerLabels[i]->GetComponent<NormalTextComponent>()->SetText(label);
    }
}


void Gameplay::DrawMark(sf::RenderWindow* window, int row, int col, CellState mark) const
{
    int pi = static_cast<int>(mark) - 1;
    if (pi < 0 || pi >= MAX_PLAYERS) return;

    const sf::Texture& tex = _markTextures[pi];
    sf::Vector2u sz = tex.getSize();
    if (sz.x == 0 || sz.y == 0) return;

    sf::Sprite sprite(tex);
    float scale = MARK_SIZE / static_cast<float>(std::max(sz.x, sz.y));
    sprite.setScale({scale, scale});
    sprite.setOrigin({sz.x / 2.f, sz.y / 2.f});
    sprite.setPosition(GetCellCenter(row, col));
    window->draw(sprite);
}



bool Gameplay::IsMyTurn() const
{
    return _turnComp->GetCurrentPlayer() == _playerIndex && !IsSpectator(_playerIndex);
}

bool Gameplay::IsSpectator(int playerIndex) const
{
    if (!_playerEntities[playerIndex]) return false;
    auto* info = _playerEntities[playerIndex]->GetComponent<PlayerInfoComponent>();
    return info && info->IsSpectator();
}

int Gameplay::CountSpectators() const
{
    int count = 0;
    for (int i = 0; i < _numPlayers; ++i)
        if (IsSpectator(i)) ++count;
    return count;
}

CellState Gameplay::PlayerIndexToCell(int index) const
{
    switch (index)
    {
        case 0: return CellState::PLAYER1;
        case 1: return CellState::PLAYER2;
        case 2: return CellState::PLAYER3;
        case 3: return CellState::PLAYER4;
        default: return CellState::EMPTY;
    }
}

sf::Color Gameplay::PlayerIndexToColor(int index) const
{
    switch (index)
    {
        case 0: return sf::Color(50,  120, 220);  // Azul   — PJ1
        case 1: return sf::Color(220, 50,  50);   // Rojo   — PJ2
        case 2: return sf::Color(50,  200, 80);   // Verde  — PJ3
        case 3: return sf::Color(230, 120, 30);   // Naranja — PJ4
        default: return sf::Color::White;
    }
}

sf::Vector2f Gameplay::GetCellCenter(int row, int col) const
{
    return sf::Vector2f(
        BOARD_ORIGIN_X + col * CELL_SIZE + CELL_SIZE / 2.f,
        BOARD_ORIGIN_Y + row * CELL_SIZE + CELL_SIZE / 2.f
    );
}

bool Gameplay::GetCellFromMouse(sf::Vector2f mousePos, int& outRow, int& outCol) const
{
    float relX = mousePos.x - BOARD_ORIGIN_X;
    float relY = mousePos.y - BOARD_ORIGIN_Y;

    if (relX < 0.f || relY < 0.f) return false;

    outCol = static_cast<int>(relX / CELL_SIZE);
    outRow = static_cast<int>(relY / CELL_SIZE);

    return outCol < BOARD_SIZE && outRow < BOARD_SIZE;
}
