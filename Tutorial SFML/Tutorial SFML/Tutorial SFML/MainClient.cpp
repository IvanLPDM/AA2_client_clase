#include "GameManager.h"
#include <iostream>

int main() 
{
    GameManager* gameManager = new GameManager();

    std::cout << "[Client] Starting Game Manager Initialization... " << std::endl;

    gameManager->Init();

    std::cout << "[Client] Starting Game Manager Run..." << std::endl;

    gameManager->Run();

    return 0;
}