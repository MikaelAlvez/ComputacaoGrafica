#ifndef DXUT_APP_H
#define DXUT_APP_H

#include "Graphics.h"
#include "Window.h"
#include "Input.h"


class App
{

protected:
	static Graphics*& graphics;					// componente gráfico
	static Window*& window;						// janela da aplicação
	static Input*& input;						// dispositivos de entrada
	static double& frameTime;					// tempo do último quadro

public:
	App();										// construtor
	virtual ~App();								// destrutor

	virtual void Init() = 0;					// inicialização
	virtual void Update() = 0;					// atualização
	virtual void Finalize() = 0;				// finalização	

	virtual void Draw() {}						// desenho
	virtual void Display() {}					// exibição
	virtual void OnPause() { Sleep(10); }		// em pausa
};

#endif