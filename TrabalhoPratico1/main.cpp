#include "DXUT.h"
#include <vector>
#include <fstream>

struct Vertex
{
    XMFLOAT3 Pos;
    XMFLOAT4 Color;
};

struct Point {
    float x;
    float y;
};

class CurvesBezier : public App
{

    //Uso do motor - WinMain
    int APIENTRY WinMain(_In_ HINSTANCE hInstance,
        _In_opt_ HINSTANCE hPrevInstance,
        _In_ LPSTR lpCmdLine,
        _In_ int nCmdShow)
    {
        try
        {
            // cria motor e configura a janela
            Engine* engine = new Engine();
            engine->window->Mode(WINDOWED);
            engine->window->Size(600, 600);
            engine->window->ResizeMode(ASPECTRATIO);
            engine->window->Color(0, 0, 0);
            engine->window->Title("Triangle");
            engine->window->Icon(IDI_ICON);
            engine->window->LostFocus(Engine::Pause);
            engine->window->InFocus(Engine::Resume);

            // cria e executa a aplicação
            engine->Start(new CurvesBezier());

            // finaliza execução
            delete engine;
        }
        catch (Error& e)
        {
            // exibe mensagem em caso de erro
            MessageBox(nullptr, e.ToString().data(), "Triangle", MB_OK);
        }

        return 0;
    }