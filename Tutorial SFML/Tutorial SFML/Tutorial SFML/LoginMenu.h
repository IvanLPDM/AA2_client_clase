#pragma once
#include "Menu.h"
#include "ButtonComponent.h"
#include "TextFieldComponent.h"
#include "SpriteRenderer.h"
#include "EventHandler.h"
#include "Client.h"

class LoginMenu : public Menu
{
public:
    LoginMenu(EventHandler* eventHandler, Client* client);
    ~LoginMenu();
    void Update(float deltaTime);
    void Render(sf::RenderWindow* window);
    ButtonComponent* GetLoginButton();
    ButtonComponent* GetRegisterButton();
    std::string GetUsernameText();
    std::string GetPasswordText();
    Event<> onLoginSucces;
    Event<> onRegisterSucces;

private:
    void SubcribeToButtons();
    TextFieldComponent* _usernameField;
    TextFieldComponent* _passwordField;
    ButtonComponent* _loginButton;
    ButtonComponent* _registerButton;
    Client* _client;

};

