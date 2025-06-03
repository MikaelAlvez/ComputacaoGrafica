#include "App.h"
#include "Engine.h"

Graphics*& App::graphics = Engine::graphics;    // componente gráfico 
Window*& App::window = Engine::window;          // janela da aplicação
Input*& App::input = Engine::input;             // dispositivos de entrada
double& App::frameTime = Engine::frameTime;     // tempo do último quadro

App::App()
{
}

App::~App()
{
}