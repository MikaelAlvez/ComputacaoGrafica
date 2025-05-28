#include "App.h"
#include "Engine.h"

// -------------------------------------------------------------------------------
// Inicializa��o de membros est�ticos da classe
Graphics* & App::graphics = Engine::graphics;
Window* & App::window = Engine::window;		// ponteiro para a janela
Input* & App::input = Engine::input;		// ponteiro para a entrada
double & App::frameTime = Engine::frameTime;

// -------------------------------------------------------------------------------

App::App()
{
	if (!window)
	{
		// ATEN��O: assume que a inst�ncia da engine �  
		// criada antes da inst�ncia da aplica��o
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