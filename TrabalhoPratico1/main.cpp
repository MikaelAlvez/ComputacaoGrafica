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

void CurvesBezier::Update()
{
    //Posição do mouse
    float cx = float(window->CenterX());
    float cy = float(window->CenterY());
    float mx = float(input->MouseX());
    float my = float(input->MouseY());

    float x = ((mx - cx) / cx);
    float y = ((cy - my) / cy);

    //Reinicialização dos pontos e linhas de controle
    if (pointsCount == 4) {
        //Removendo os dois pontos anteriores e mantendo somente os dois ultimos
        for (int i{}; i < VertexCount; i++) {
            points[1][i].Pos.x = points[3][i].Pos.x + (points[3][i].Pos.x - points[2][i].Pos.x);
            points[1][i].Pos.y = points[3][i].Pos.y + (points[3][i].Pos.y - points[2][i].Pos.y);
            points[0][i].Pos.x = points[3][i].Pos.x;
            points[0][i].Pos.y = points[3][i].Pos.y;
        }
        //Removendo as primeiras duas linhas e mantendo somente as duas ultimas e também continuando ela com a formula p4 = p4 + (p4-p3);
        lines[0] = lines[3];
        lines[1].Pos.x = lines[3].Pos.x + (lines[3].Pos.x - lines[2].Pos.x);
        lines[1].Pos.y = lines[3].Pos.y + (lines[3].Pos.y - lines[2].Pos.y);
        pointsCount = 2;

        //Removendo os primeiros dois pontos e mantendo somente os dois ultimos;
        pointsLocation[0] = pointsLocation[3];
        pointsLocation[1].x = pointsLocation[3].x + (pointsLocation[3].x - pointsLocation[2].x);
        pointsLocation[1].y = pointsLocation[3].y + (pointsLocation[3].y - pointsLocation[2].y);

        //Mudando ponteiro da linha que segue o mouse;
        linee[0] = { XMFLOAT3(pointsLocation[1].x, pointsLocation[1].y, 0.0f), XMFLOAT4(Colors::DarkViolet) };
        //Mandando comandos de desenho para o buffer;
        graphics->ResetCommands();
        graphics->Copy(points, pointsMesh->vertexBufferSize, pointsMesh->vertexBufferUpload, pointsMesh->vertexBufferGPU);
        graphics->SubmitCommands();

        graphics->ResetCommands();
        graphics->Copy(lines, linesMesh->vertexBufferSize, linesMesh->vertexBufferUpload, linesMesh->vertexBufferGPU);
        graphics->SubmitCommands();

        curvesCount += 20;

        if (curvesCount >= amount) {
            if (amount * 2 <= 25600) {
                //Crescendo vetor dinamico;
                int newAmount = amount * 2;
                Vertex* aux = new Vertex[newAmount];

                for (int i{}; i < curvesCount; i++) {
                    aux[i] = curves[i];
                }

                delete[] curves; //Deleta vetor atual;
                amount = newAmount; //Novo tamanho;
                curves = aux; //Ponteiro aponta para auxiliar;
                delete curvesMesh; //Deleta a mesh antiga;

                curvesMesh = new Mesh(&curves, (amount * sizeof(Vertex)), sizeof(Vertex)); //Cria nova mesh com novo tamanho;

                //Atualiza comandos para o buffer;
                graphics->ResetCommands();
                graphics->Copy(curves, curvesMesh->vertexBufferSize, curvesMesh->vertexBufferUpload, curvesMesh->vertexBufferGPU);
                graphics->SubmitCommands();
            }
            else {
                //Limite de crescimento alcançado, vetor não cresce mais
                curvesCount = 25580;
            }
        }
    }

    //Criação das curvas
    if (pointsCount > 1) {  //Ultimos 2 pontos seguirem o mouse
        if (pointsCount == 2) {
            pointsLocation[2] = pointsLocation[3] = { x, y };
        }
        else if (pointsCount == 3) {
            pointsLocation[3] = { x, y };
        }
    }

    //Curva aparece após segundo clique;
    if (pointsCount > 1) {

        for (int i{}; i < curvesPoints; i++) {
            curves[i + curvesCount] = {
             XMFLOAT3(Bernstein(i / 20.0, pointsLocation[0].x, pointsLocation[1].x,
             pointsLocation[2].x, pointsLocation[3].x),
             Bernstein(i / 20.0, pointsLocation[0].y, pointsLocation[1].y,
             pointsLocation[2].y, pointsLocation[3].y), 0.0f),
             XMFLOAT4(Colors::Yellow)
            };
        }

        graphics->ResetCommands();
        graphics->Copy(curves, curvesMesh->vertexBufferSize, curvesMesh->vertexBufferUpload, curvesMesh->vertexBufferGPU);
        graphics->SubmitCommands();
    }

    //Movimentação do icone
    for (int i{}; i < VertexCount; i++) {
        vertices[i].Pos.x = x + vertexPoints[i].x;
        vertices[i].Pos.y = y + vertexPoints[i].y;
    }

    graphics->ResetCommands();
    graphics->Copy(vertices, geometry->vertexBufferSize, geometry->vertexBufferUpload, geometry->vertexBufferGPU);
    graphics->SubmitCommands();

    if (pointsCount >= 1 && pointsCount <= 3) {
        linee[1] = { XMFLOAT3(x, y, 0.0f), XMFLOAT4(Colors::DarkViolet) };

        graphics->ResetCommands();
        graphics->Copy(linee, lineMesh->vertexBufferSize, lineMesh->vertexBufferUpload, lineMesh->vertexBufferGPU);
        graphics->SubmitCommands();
    }

    //Comandos do teclado
    //Clique do mouse coloca pontos
    if (input->KeyPress(VK_LBUTTON)) {
        //Colocando pontos de controle
        if (pointsCount >= 0 && pointsCount <= 3) { //Index 0 ate 3
            //Pontos saem aqui
            for (int i{}; i < VertexCount; i++) {
                points[pointsCount][i].Pos.x = x + vertexPoints[i].x;
                points[pointsCount][i].Pos.y = y + vertexPoints[i].y;

                points[pointsCount][i].Color = { XMFLOAT4(Colors::Purple) };
            }

            pointsLocation[pointsCount] = { x, y };

            graphics->ResetCommands();
            graphics->Copy(points, pointsMesh->vertexBufferSize, pointsMesh->vertexBufferUpload, pointsMesh->vertexBufferGPU);
            graphics->SubmitCommands();

            lines[pointsCount] = { XMFLOAT3(x, y, 0.0f), XMFLOAT4(Colors::DarkViolet) };
            linee[0] = { XMFLOAT3(x, y, 0.0f), XMFLOAT4(Colors::DarkViolet) };

            graphics->ResetCommands();
            graphics->Copy(lines, linesMesh->vertexBufferSize, linesMesh->vertexBufferUpload, linesMesh->vertexBufferGPU);
            graphics->SubmitCommands();

            pointsCount++;
        }

    }

    //Exclusão de curvas
    if (input->KeyPress(VK_DELETE) || input->KeyPress('d') || input->KeyPress('D')) {
        pointsCount = 0;
        curvesCount = 0;
        delete[] curves;
        curves = new Vertex[amount];

        //Mandando comandos de desenho para o buffer
        graphics->ResetCommands();
        graphics->Copy(points, pointsMesh->vertexBufferSize, pointsMesh->vertexBufferUpload, pointsMesh->vertexBufferGPU);
        graphics->SubmitCommands();

        graphics->ResetCommands();
        graphics->Copy(lines, linesMesh->vertexBufferSize, linesMesh->vertexBufferUpload, linesMesh->vertexBufferGPU);
        graphics->SubmitCommands();

        graphics->ResetCommands();
        graphics->Copy(curves, curvesMesh->vertexBufferSize, curvesMesh->vertexBufferUpload, curvesMesh->vertexBufferGPU);
        graphics->SubmitCommands();
    }

    //Sai com o pressionamento da tecla ESC
    if (input->KeyPress(VK_ESCAPE))
        window->Close();

    //Salvar dados em arquivo
    if (input->KeyPress('s') || input->KeyPress('S')) {
        std::ofstream arquivo("curvas.bin", std::ios::binary);

        if (arquivo.is_open()) {
            arquivo.write(reinterpret_cast<const char*>(&amount), sizeof(int)); //Escreve tamanho atual do vetor
            arquivo.write(reinterpret_cast<const char*>(&curvesCount), sizeof(int)); //Escreve quantidade de curvas no vetor

            for (int i{}; i < ctrlCount; i++) {
                arquivo.write(reinterpret_cast<const char*>(&lines[i]), sizeof(Vertex));  //Escreve linhas no arquivo
            }

            arquivo.write(reinterpret_cast<const char*>(&pointsCount), sizeof(int)); //Escreve quantidade de pontos atuais

            for (int i{}; i < ctrlCount; i++) {
                for (int j{}; j < VertexCount; j++) {
                    arquivo.write(reinterpret_cast<const char*>(&points[i][j]), sizeof(Vertex));  //Escreve pontos
                }
            }

            for (int i{}; i < ctrlCount; i++) {
                arquivo.write(reinterpret_cast<const char*>(&pointsLocation[i]), sizeof(Point));  //Escreve localização dos pontos
            }


            for (int i{}; i < 2; i++) {
                arquivo.write(reinterpret_cast<const char*>(&linee[i]), sizeof(Vertex));  //Escreve localização dos pontos
            }

            for (int i{}; i < curvesCount; i++) {
                arquivo.write(reinterpret_cast<const char*>(&curves[i]), sizeof(Vertex)); //Escreve curvas
            }

            arquivo.close();
        }
    }

    //Carregar dados do arquivo
    if (input->KeyPress('l') || input->KeyPress('L')) {
        std::ifstream arquivo("curvas.bin", std::ios::binary);

        if (arquivo) {
            int i = 0;

            //Leitura da quantidade total de curvas e inicialização do vetor de curvas
            arquivo.read(reinterpret_cast<char*>(&amount), sizeof(int));
            curves = new Vertex[amount];

            //Leitura da quantidade de curvas ativas
            arquivo.read(reinterpret_cast<char*>(&curvesCount), sizeof(int));

            //Leitura das linhas de controle
            for (i = 0; i < ctrlCount; i++) {
                arquivo.read(reinterpret_cast<char*>(&lines[i]), sizeof(Vertex));
            }

            //Leitura da quantidade de pontos
            arquivo.read(reinterpret_cast<char*>(&pointsCount), sizeof(int));

            //Leitura dos pontos
            for (i = 0; i < ctrlCount; i++) {
                for (int j = 0; j < VertexCount; j++) {
                    arquivo.read(reinterpret_cast<char*>(&points[i][j]), sizeof(Vertex));
                }
            }

            //Leitura das localizações dos pontos
            for (i = 0; i < ctrlCount; i++) {
                arquivo.read(reinterpret_cast<char*>(&pointsLocation[i]), sizeof(Point));
            }

            //Escreve localização dos pontos
            for (int i{}; i < 2; i++) {
                arquivo.read(reinterpret_cast<char*>(&linee[i]), sizeof(Vertex));
            }

            //Leitura das curvas
            for (i = 0; i < curvesCount; i++) {
                arquivo.read(reinterpret_cast<char*>(&curves[i]), sizeof(Vertex));
            }

            curvesMesh = new Mesh(&curves, (amount * sizeof(Vertex)), sizeof(Vertex));

            arquivo.close();

            //Mandando comandos de desenho para o buffer
            graphics->ResetCommands();
            graphics->Copy(points, pointsMesh->vertexBufferSize, pointsMesh->vertexBufferUpload, pointsMesh->vertexBufferGPU);
            graphics->SubmitCommands();

            graphics->ResetCommands();
            graphics->Copy(lines, linesMesh->vertexBufferSize, linesMesh->vertexBufferUpload, linesMesh->vertexBufferGPU);
            graphics->SubmitCommands();

            graphics->ResetCommands();
            graphics->Copy(curves, curvesMesh->vertexBufferSize, curvesMesh->vertexBufferUpload, curvesMesh->vertexBufferGPU);
            graphics->SubmitCommands();

            graphics->ResetCommands();
            graphics->Copy(linee, lineMesh->vertexBufferSize, lineMesh->vertexBufferUpload, lineMesh->vertexBufferGPU);
            graphics->SubmitCommands();

        }
    }

    Display();
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
                                   
//Assinatura raiz do pipeline
void CurvesBezier::BuildRootSignature()
{
    //Descrição para uma assinatura vazia
    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.NumParameters = 0;
    rootSigDesc.pParameters = nullptr;
    rootSigDesc.NumStaticSamplers = 0;
    rootSigDesc.pStaticSamplers = nullptr;
    rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    //Serializa assinatura raiz
    ID3DBlob* serializedRootSig = nullptr;
    ID3DBlob* error = nullptr;

    ThrowIfFailed(D3D12SerializeRootSignature(
        &rootSigDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        &serializedRootSig,
        &error));

    //Cria uma assinatura raiz vazia
    ThrowIfFailed(graphics->Device()->CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature)));
}

void CurvesBezier::BuildPipelineState()
{
    //Input Layout
    D3D12_INPUT_ELEMENT_DESC inputLayout[2] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    //Shaders
    ID3DBlob* vertexShader;
    ID3DBlob* pixelShader;

    D3DReadFileToBlob(L"Shaders/Vertex.cso", &vertexShader);
    D3DReadFileToBlob(L"Shaders/Pixel.cso", &pixelShader);

    //Rasterizer
    D3D12_RASTERIZER_DESC rasterizer = {};
    rasterizer.FillMode = D3D12_FILL_MODE_SOLID;
    //rasterizer.FillMode = D3D12_FILL_MODE_WIREFRAME;
    rasterizer.CullMode = D3D12_CULL_MODE_BACK;
    rasterizer.FrontCounterClockwise = FALSE;
    rasterizer.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rasterizer.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasterizer.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasterizer.DepthClipEnable = TRUE;
    rasterizer.MultisampleEnable = FALSE;
    rasterizer.AntialiasedLineEnable = FALSE;
    rasterizer.ForcedSampleCount = 0;
    rasterizer.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

   
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
