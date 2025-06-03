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
private:
    ID3D12RootSignature* rootSignature = nullptr;
    ID3D12PipelineState* pipelineState = nullptr;

    //Icone
    Mesh* geometry = nullptr; //Geometria do icone;

    // quantidade de vértices na geometria
    static const uint VertexCount = 6;
    // vértices da geometria
    Vertex vertices[VertexCount] =
    {
        { XMFLOAT3(-0.02f, 0.02f, 0.0f), XMFLOAT4(Colors::Purple) },
        { XMFLOAT3(0.02f, 0.02f, 0.0f), XMFLOAT4(Colors::Purple) },
        { XMFLOAT3(0.02f, -0.02f, 0.0f), XMFLOAT4(Colors::Purple) },
        { XMFLOAT3(0.02f, -0.02f, 0.0f), XMFLOAT4(Colors::Purple) },
        { XMFLOAT3(-0.02f, -0.02f, 0.0f), XMFLOAT4(Colors::Purple) },
        { XMFLOAT3(-0.02f, 0.02f, 0.0f), XMFLOAT4(Colors::Purple) }
    };

    Point vertexPoints[VertexCount] = {
        {-0.02f, 0.02f},
        {0.02f, 0.02f},
        {0.02f, -0.02f},
        {0.02f, -0.02f},
        {-0.02f, -0.02f},
        {-0.02f, 0.02f}
    };

    //Curvas
    Mesh* curvesMesh = nullptr; //Geometria das curvas
    uint curvesCount = 0; //Quantidade de curvas no vetor
    static const uint curvesPoints = 20; //Quantidade de pontos para fazer a curva
    int amount = 400; //Tamanho inicial do vetor
    Vertex* curves; //Vetor dinamico
    Point pointsLocation[4]{}; //Pontos do clique

    //Pontos de controle desenhados na tela
    Mesh* pointsMesh = nullptr; //Geometria pontos de controle
    static const uint ctrlCount = 4; //Quantidade de vertices de controle;
    Vertex points[ctrlCount][VertexCount]{};
    int pointsCount = 0;

    //Linhas de controle
    Mesh* linesMesh = nullptr; //Geometria das linhas de controle
    Vertex lines[ctrlCount]{}; //Vetor que cuidará das linhas de controle

    //Linha que segue o mouse
    Mesh* lineMesh = nullptr; //Linha que segue o mouse
    static const uint lineCount = 2;
    Vertex linee[lineCount]{};

public:
    void Init();
    void Update();
    void Display();
    void Finalize();


    float Bernstein(float, float, float, float, float);

    void BuildRootSignature();
    void BuildPipelineState();
};

void CurvesBezier::Init()
{
    graphics->ResetCommands();

    //Tamanho do buffer de vértices em bytes
    const uint vbSize = VertexCount * sizeof(Vertex);

    //Cria malha 3D
    geometry = new Mesh(vertices, vbSize, sizeof(Vertex));
    curvesMesh = new Mesh(&curves, (amount * sizeof(Vertex)), sizeof(Vertex));
    pointsMesh = new Mesh(points, (ctrlCount * VertexCount * sizeof(Vertex)), sizeof(Vertex));
    linesMesh = new Mesh(lines, (ctrlCount * sizeof(Vertex)), sizeof(Vertex));
    lineMesh = new Mesh(linee, lineCount * sizeof(Vertex), (uint)sizeof(Vertex));
    
    //Vetor dinamico de curvas
    curves = new Vertex[amount];

    BuildRootSignature();
    BuildPipelineState();

    graphics->SubmitCommands();
}

void CurvesBezier::Display()
{
    //Limpa backbuffer
    graphics->Clear(pipelineState);

    //Submete comandos de configuração do pipeline
    graphics->CommandList()->SetGraphicsRootSignature(rootSignature);

    //Desenha icone;
    graphics->CommandList()->IASetVertexBuffers(0, 1, geometry->VertexBufferView());
    graphics->CommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //Submete comando de desenho
    graphics->CommandList()->DrawInstanced(6, 1, 0, 0);

    //Desenha pontos de controle
    graphics->CommandList()->IASetVertexBuffers(0, 1, pointsMesh->VertexBufferView());
    graphics->CommandList()->DrawInstanced(6 * pointsCount, 1, 0, 0);

    //Desenha linhas de controle
    graphics->CommandList()->IASetVertexBuffers(0, 1, linesMesh->VertexBufferView());
    graphics->CommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
    graphics->CommandList()->DrawInstanced(pointsCount, 1, 0, 0);

    //Desenha linha que segue mouse
    graphics->CommandList()->IASetVertexBuffers(0, 1, lineMesh->VertexBufferView());
    graphics->CommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
    if (pointsCount >= 1) {
        graphics->CommandList()->DrawInstanced(lineCount, 1, 0, 0);
    }

    //Desenha a curva;
    graphics->CommandList()->IASetVertexBuffers(0, 1, curvesMesh->VertexBufferView());
    graphics->CommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
    graphics->CommandList()->DrawInstanced(amount, 1, 0, 0);

    //Apresenta backbuffer
    graphics->Present();
}

void CurvesBezier::Finalize()
{
    rootSignature->Release();
    pipelineState->Release();
    delete geometry;
    delete pointsMesh;
    delete linesMesh;
    delete curvesMesh;
    delete lineMesh;
    delete curves;
}

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
