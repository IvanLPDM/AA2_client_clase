#pragma once
#include <SFML/Graphics.hpp>
#include "EventHandler.h"
#include "Enums.hpp"
#include "LoginMenu.h"
#include "SplashScreenMenu.h"
#include <SFML/Network.hpp>
#include "Client.h"
#include "MatchmakingMenu.h"
#include "Gameplay.h"
#include "GameOverMenu.h"
#include "RankingMenu.h"

class GameManager
{
public:
    GameManager();
    ~GameManager();
    void Init();
    void Run();
    void Shutdown();
    void UpdateState(GameState newState);

private:
    void Update(float deltaTime);
    void Render();
    void HandleEvents();

    GameState _currentState;
    GameState _nextState = GameState::None;
    bool _shouldChangeState;

    sf::TcpSocket _socket;
    Window* _window;
    EventHandler* _eventHandler;
    sf::Clock _deltaClock;

    Client* _client;

    // -- Menus / Gameplay -- //
    LoginMenu* _loginMenu;
    MatchmakingMenu* _matchmakingMenu;
    SplashScreenMenu* _splashMenu;
    Gameplay* _gameplay;
    GameOverMenu* _gameOverMenu;
    RankingMenu* _rankingMenu;

    float _splashTime;
    
};

