#include "LoginMenu.h"
#include <iostream>

LoginMenu::LoginMenu(EventHandler* eventHandler, Client* client) {

    AddComponent<SpriteRenderer>("Assets/Splashscreen/login.png", sf::Color::White, false);

    _usernameField = AddComponent<TextFieldComponent>(sf::Vector2f(160, 300), sf::Vector2f(400, 50), "Username", eventHandler);

    _passwordField = AddComponent<TextFieldComponent>(sf::Vector2f(160, 370), sf::Vector2f(400, 50), "Password", eventHandler);

    _loginButton = AddComponent<ButtonComponent>(sf::Vector2f(170, 450), sf::Vector2f(180, 50), "Login", eventHandler);

    _registerButton = AddComponent<ButtonComponent>(sf::Vector2f(370, 450), sf::Vector2f(180, 50), "Register", eventHandler);

    _client = client;

    SubcribeToButtons();
}

LoginMenu::~LoginMenu()
{
    onLoginSucces.UnsubscribeAll();
    onRegisterSucces.UnsubscribeAll();
}

void LoginMenu::Update(float deltaTime)
{
    _usernameField->Update(deltaTime);
    _passwordField->Update(deltaTime);
}

void LoginMenu::Render(sf::RenderWindow* window)
{
    GetComponent<SpriteRenderer>()->Draw(window, GetComponent<Transform>());
    _usernameField->Render(window);
    _passwordField->Render(window);
    _loginButton->Render(window);
    _registerButton->Render(window);
}

ButtonComponent* LoginMenu::GetLoginButton()
{
    return _loginButton;
}

ButtonComponent* LoginMenu::GetRegisterButton()
{
    return _registerButton;
}

std::string LoginMenu::GetUsernameText()
{
    return _usernameField->GetText();
}

std::string LoginMenu::GetPasswordText()
{
    return _passwordField->GetText();
}

void LoginMenu::SubcribeToButtons()
{
    _loginButton->onClick.Subscribe([this]() {

        std::cout << "[Client] Send Login with username: " + GetUsernameText() + " and password: " + GetPasswordText() << std::endl;

        if (_client->SendLogin(GetUsernameText(), GetPasswordText()))
        {
            _client->SetMyUsername(GetUsernameText());
            auto optPacket = _client->WaitForServerMessage(2.f);
            if (optPacket.has_value())
            {
                sf::Packet& response = optPacket.value();
                std::string reply;
                response >> reply;

                if (reply == "LOGIN_OK")
                {
                    onLoginSucces.Invoke();
                }
                else
                {
                    std::cout << "[Client] Login failed: " << reply << std::endl;
                }
            }
            else
            {
                std::cout << "[Client] Error reciving server answer." << std::endl;
            }
        }
        else
        {
            std::cout << "[Client] Error sending login." << std::endl;
        }
    });

    _registerButton->onClick.Subscribe([this]() {

        std::cout << "[Client] Send Register with username: " + GetUsernameText() + " and password: " + GetPasswordText() << std::endl;

        if (_client->SendRegister(GetUsernameText(), GetPasswordText()))
        {
            _client->SetMyUsername(GetUsernameText());

            auto optPacket = _client->WaitForServerMessage(2.f);
            if (optPacket.has_value())
            {
                sf::Packet& response = optPacket.value();
                std::string reply;
                response >> reply;

                if (reply == "REGISTER_OK")
                {
                    onRegisterSucces.Invoke();
                }
                else
                {
                    std::cout << "[Client] Register failed: " << reply << std::endl;
                }
            }
            else
            {
                std::cout << "[Client] Error reciving answer from server" << std::endl;
            }
        }
        else
        {
            std::cout << "[Client] Error sending register" << std::endl;
        }
    });
}
