#ifndef DXUT_ENGINE_H
#define DXUT_ENGINE_H

// ---------------------------------------------------------------------------------

#include "Graphics.h"
#include "App.h"
#include "Window.h"
#include "Timer.h"

// ---------------------------------------------------------------------------------

class Engine
{
private:
	static Timer timer;					// medidor de tempo
	static bool paused;                 // estado do aplicação

	float FrameTime();					// calcula o tempo do quadro
	int Loop();							// laço principal do motor

public:
	static Graphics* graphics;			// dispositivo gráfico
	static Window* window;				// janela da aplicação
	static Input* input;				// dispositivos de entrada da aplicação
	static App* app;					// aplicação a ser executada
	static double frameTime;			// tempo do quadro atual

	Engine();							// construtor
	~Engine();							// destrutor

	int Start(App* application);		// inicia o execução da aplicação

	static void Pause();                // pausa o motor
	static void Resume();               // reinicia o motor

	// trata eventos do Windows
	static LRESULT CALLBACK EngineProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

// -----------------------------------------------------------------------------
// Funções Inline

inline void Engine::Pause()
{
	paused = true; timer.Stop();
}

inline void Engine::Resume()
{
	paused = false; timer.Start();
}

// ---------------------------------------------------------------------------------

#endif