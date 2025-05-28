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
	static bool paused;                 // estado do aplica��o

	float FrameTime();					// calcula o tempo do quadro
	int Loop();							// la�o principal do motor

public:
	static Graphics* graphics;			// dispositivo gr�fico
	static Window* window;				// janela da aplica��o
	static Input* input;				// dispositivos de entrada da aplica��o
	static App* app;					// aplica��o a ser executada
	static double frameTime;			// tempo do quadro atual

	Engine();							// construtor
	~Engine();							// destrutor

	int Start(App* application);		// inicia o execu��o da aplica��o

	static void Pause();                // pausa o motor
	static void Resume();               // reinicia o motor

	// trata eventos do Windows
	static LRESULT CALLBACK EngineProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

// -----------------------------------------------------------------------------
// Fun��es Inline

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