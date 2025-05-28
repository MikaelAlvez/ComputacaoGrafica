#include "App.h"
#include "Engine.h"

// -------------------------------------------------------------------------------
// Inicialização de membros estáticos da classe
Graphics* & App::graphics = Engine::graphics;
Window* & App::window = Engine::window;		// ponteiro para a janela
Input* & App::input = Engine::input;		// ponteiro para a entrada
double & App::frameTime = Engine::frameTime;

// -------------------------------------------------------------------------------

App::App()
{
	if (!window)
	{
		// ATENÇÃO: assume que a instância da engine é  
		// criada antes da instância da aplicação
		window = Engine::window;
		input = Engine::input;
		graphics = Engine::graphics;
	}
}

// -------------------------------------------------------------------------------

App::~App()
{
}

// -------------------------------------------------------------------------------