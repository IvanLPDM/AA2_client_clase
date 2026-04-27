#include "GameManager.h"


GameManager::GameManager()
{
    _window = new Window();
    _client = new Client();
    _eventHandler = new EventHandler();
    _currentState = GameState::None;
    _loginMenu = nullptr;
    _splashMenu = nullptr;
    _matchmakingMenu = nullptr;
    _gameplay = nullptr;
    _gameOverMenu = nullptr;
    _rankingMenu = nullptr;

    UpdateState(GameState::SplashScreen);
    _splashTime = 0;

    std::srand(std::time(nullptr));
}

GameManager::~GameManager()
{
	Shutdown();
}

void GameManager::Init()
{    
}

void GameManager::Run()
{
    while (_window->IsOpen()) {

        float deltaTime = _deltaClock.restart().asSeconds();

        HandleEvents();
        Update(deltaTime);
        Render();
    }
}

void GameManager::Shutdown()
{   
    delete _loginMenu;
    delete _matchmakingMenu;
    delete _splashMenu;
    delete _gameOverMenu;
    delete _rankingMenu;
    delete _client;
    delete _eventHandler;
    delete _gameplay;
    delete _window;
}


void GameManager::UpdateState(GameState newState)
{
    if (_currentState == newState) return;

    switch (_currentState) 
    {
        case GameState::SplashScreen:
            delete _splashMenu;
            _splashMenu = nullptr;
            break;

        case GameState::LoginMenu:
            delete _loginMenu;
            _loginMenu = nullptr;
            break;

        case GameState::MatchmakingMenu:
            delete _matchmakingMenu;
            _matchmakingMenu = nullptr;
            break;

        case GameState::Gameplay:
            delete _gameplay;
            _gameplay = nullptr;
            break;

        case GameState::GameOver:
            delete _gameOverMenu;
            _gameOverMenu = nullptr;
            break;

        case GameState::RankingMenu:
            delete _rankingMenu;
            _rankingMenu = nullptr;
            break;

        default:
            break;
    }

    switch (newState) 
    {
        case GameState::SplashScreen:
            _client->ConnectToBootstrapServer("127.0.0.1", 50000);
            std::cout << "[Client] Opening Splashscreen" << std::endl;
            _splashMenu = new SplashScreenMenu();
            break;
        case GameState::LoginMenu:
            std::cout << "[Client] Opening Login Menu" << std::endl;
            _loginMenu = new LoginMenu(_eventHandler, _client);
            _loginMenu->onLoginSucces.Subscribe([this]() {     _nextState = GameState::MatchmakingMenu;
            _shouldChangeState = true; });
            _loginMenu->onRegisterSucces.Subscribe([this]() { std::cout << "Registered succesfully..." << std::endl; });
            break;

        case GameState::MatchmakingMenu:
            std::cout << "[Client] Opening Matchmaking Menu" << std::endl;
            // Only reconnect to bootstrap when not coming from the ranking screen
            if (_currentState != GameState::RankingMenu)
            {
                _client->ConnectToBootstrapServer("127.0.0.1", 50000);
                if (_client->HasPendingGameResult())
                    _client->SendStoredGameResult();
            }
            _matchmakingMenu = new MatchmakingMenu(_eventHandler, _client);
            _matchmakingMenu->onStartMatch.Subscribe([this]() { _nextState = GameState::Gameplay; _shouldChangeState = true; });
            _matchmakingMenu->onShowRanking.Subscribe([this]() { _nextState = GameState::RankingMenu; _shouldChangeState = true; });
            break;

        case GameState::RankingMenu:
            std::cout << "[Client] Opening Ranking Menu" << std::endl;
            _rankingMenu = new RankingMenu(_eventHandler, _client);
            _rankingMenu->onReturn.Subscribe([this]() { _nextState = GameState::MatchmakingMenu; _shouldChangeState = true; });
            break;

        case GameState::Gameplay:
            std::cout << "[Client] Starting the game... " << std::endl;
            _gameplay = new Gameplay(_client, _client->GetPlayerIndex(), _client->GetNumPlayers(), _eventHandler);
            _gameplay->onWinMatch.Subscribe([this]  { _nextState = GameState::GameOver; _shouldChangeState = true; });
            _gameplay->onLoseMatch.Subscribe([this] { _nextState = GameState::GameOver; _shouldChangeState = true; });
            break;

        case GameState::GameOver:
            std::cout << "[Client] Game finished..." << std::endl;
            _gameOverMenu = new GameOverMenu(_eventHandler, _client);
            _gameOverMenu->onReturnMenu.Subscribe([this]() { _nextState = GameState::MatchmakingMenu; _shouldChangeState = true; });
            _gameOverMenu->onExitGame.Subscribe([this]() { _window->Close(); });
            break;

        default: break;
    }

    _currentState = newState;
}

void GameManager::Update(float deltaTime)
{
    switch (_currentState) {

        case GameState::SplashScreen:            

            _splashTime += deltaTime;

            if (_splashTime > 2) 
            {
                UpdateState(GameState::LoginMenu);
                _splashTime = 0;
            }
            break;

        case GameState::LoginMenu:

            _loginMenu->Update(deltaTime);
            break;

        case GameState::MatchmakingMenu:
            _matchmakingMenu->Update(deltaTime);
            break;

        case GameState::Gameplay:
            _gameplay->Update(deltaTime);
            break;

        case GameState::GameOver:
            _gameOverMenu->Update(deltaTime);
            break;

        case GameState::RankingMenu:
            _rankingMenu->Update(deltaTime);
            break;

    }

    if (_shouldChangeState) 
    {
        UpdateState(_nextState);
        _shouldChangeState = false;
    }
}

void GameManager::Render()
{
    _window->Clear();

    switch (_currentState) 
    {
        case GameState::SplashScreen:
            _splashMenu->Render(_window->GetWindow());
            break;

        case GameState::LoginMenu:
            _loginMenu->Render(_window->GetWindow());
            break;

        case GameState::MatchmakingMenu:
           _matchmakingMenu->Render(_window->GetWindow());
            break;

        case GameState::Gameplay:
            _gameplay->Render(_window->GetWindow());
            break;

        case GameState::GameOver:
            _gameOverMenu->Render(_window->GetWindow());
            break;

        case GameState::RankingMenu:
            _rankingMenu->Render(_window->GetWindow());
            break;
    }

    _window->Display();
}

void GameManager::HandleEvents()
{
    while (const std::optional event = _window->GetWindow()->pollEvent())
    {
        _eventHandler->HandleEvent(*event, *_window);
    }
}


