#ifndef DXUT_APP_H
#define DXUT_APP_H

#include "Graphics.h"
#include "Window.h"
#include "Input.h"


class App
{

protected:
	static Graphics*& graphics;					// componente gr�fico
	static Window*& window;						// janela da aplica��o
	static Input*& input;						// dispositivos de entrada
	static double& frameTime;					// tempo do �ltimo quadro

public:
	App();										// construtor
	virtual ~App();								// destrutor

	virtual void Init() = 0;					// inicializa��o
	virtual void Update() = 0;					// atualiza��o
	virtual void Finalize() = 0;				// finaliza��o	

	virtual void Draw() {}						// desenho
	virtual void Display() {}					// exibi��o
	virtual void OnPause() { Sleep(10); }		// em pausa
};

#endif