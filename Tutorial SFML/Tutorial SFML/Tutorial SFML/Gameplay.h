#pragma once
#include "GameObject.h"
#include "Client.h"
#include "Enums.hpp"
#include "EventHandler.h"
#include "BoardComponent.h"
#include "TurnComponent.h"
#include "TimerComponent.h"
#include "PlayerInfoComponent.h"
#include "SpectatorComponent.h"
#include "SpriteRenderer.h"
#include "NormalTextComponent.h"
#include <array>

class Gameplay : public GameObject {
public:
    static constexpr int MAX_PLAYERS = 4;
    static constexpr int BOARD_SIZE = 6;
    static constexpr float CELL_SIZE = 90.f;
    static constexpr float BOARD_ORIGIN_X = 90.f;
    static constexpr float BOARD_ORIGIN_Y = 90.f;
    static constexpr float TURN_DURATION = 20.f;
    static constexpr int WIN_LENGTH = 3;

    // Points awarded by finish position (1st → last)
    static constexpr int POINTS_1ST  = 4;
    static constexpr int POINTS_2ND  = 2;
    static constexpr int POINTS_3RD  = 1;
    static constexpr int POINTS_LAST = 0;

    Gameplay(Client* client, int playerIndex, int numPlayers, EventHandler* eventHandler);
    ~Gameplay();

    void Update(float deltaTime);
    void Render(sf::RenderWindow* window);

    Event<> onWinMatch;
    Event<> onLoseMatch;

private:
    Client* _client;
    EventHandler* _eventHandler;
    int _playerIndex;
    int _numPlayers;
    bool _gameOver;

    // ECS components (owned by this GameObject)
    BoardComponent* _boardComp;
    TurnComponent*  _turnComp;
    TimerComponent* _timerComp;

    // One entity per player slot — each holds a PlayerInfoComponent
    std::array<GameObject*, MAX_PLAYERS> _playerEntities;

    // UI GameObjects
    GameObject* _boardObject;
    GameObject* _timerLabel;
    GameObject* _turnLabel;
    std::array<GameObject*, MAX_PLAYERS> _playerLabels;

    Event<sf::Vector2f>::ListenerID _clickListenerID;

    // -- Systems --
    void TimerSystem(float deltaTime);
    void NetworkSystem();

    // -- Game logic --
    void PlaceMark(int row, int col, int actingPlayer);
    void SkipTurn();
    void AdvanceTurn();
    bool CheckWin(int row, int col, CellState mark) const;
    void HandlePlayerWin(int playerIndex);
    void HandleGameEnd();

    // -- Network helpers --
    void BroadcastMove(int row, int col);

    // -- UI helpers --
    void SetupPlayerLabels();
    void UpdateLabels();

    // -- Rendering helpers --
    std::array<sf::Texture, MAX_PLAYERS> _markTextures;
    void DrawMark(sf::RenderWindow* window, int row, int col, CellState mark) const;
    void HighlightActiveCell(sf::RenderWindow* window, int row, int col) const;

    // -- Utilities --
    bool IsMyTurn() const;
    bool IsSpectator(int playerIndex) const;
    int CountSpectators() const;
    CellState PlayerIndexToCell(int index) const;
    sf::Color PlayerIndexToColor(int index) const;
    sf::Vector2f GetCellCenter(int row, int col) const;
    bool         GetCellFromMouse(sf::Vector2f mousePos, int& outRow, int& outCol) const;
};
