#include "DXUT.h"
#include <fstream>
#include <sstream>

struct ObjectConstants
{
    XMFLOAT4X4 WorldViewProj =
    { 1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f };
};

class BufferMulti : public App
{
private:
    ID3D12RootSignature* rootSignature = nullptr;

    //Criar duas pipeline state
    ID3D12PipelineState* pipelineState[2];
    vector<Object> scene;
    vector<Geometry> vertices;
    Timer timer;
    bool spinning = true;

    XMFLOAT4X4 Identity = {};
    XMFLOAT4X4 View = {};
    XMFLOAT4X4 Proj = {};

    D3D12_VIEWPORT views[4];

    float theta = 0;
    float phi = 0;
    float radius = 0;
    float lastMousePosX = 0;
    float lastMousePosY = 0;
    int tab = -1;
    bool viewport = false;

    //Malha para linhas que dividem a tela
    Mesh* LinhasDivisorias;
    //Vetor de vertices para as linhas
    Vertex linhas[4] = {};

public:
    void Init();
    void Update();
    void Draw();
    void Finalize();
    void AddObjectToScene(Geometry& newObj, float scaleX, float scaleY, float scaleZ);
    void DeleteObjectToScene();
    void SelectObjectInScene();
    void StartViewPorts();
    void StartDivisionLines();
    void DeselectObject();
    void ObjectScale(float x, float y, float z);
    void ObjectRotation(float x, float y, float z);
    void ObjectTranslate(float x, float y, float z);
    void LoadObject(const std::string& filename);

    void BuildRootSignature();
    void BuildPipelineState();
};

void BufferMulti::Init()
{
    graphics->ResetCommands();

    //Par�metros Iniciais da C�mera

    //Controla rota��o da c�mera
    theta = XM_PIDIV4;
    phi = 1.3f;
    radius = 5.0f;

    //�ltima posi��o do mouse
    lastMousePosX = (float)input->MouseX();
    lastMousePosY = (float)input->MouseY();

    //Inicializa as matrizes Identity e View para a identidade
    Identity = View = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f };

    //Inicializa a matriz de proje��o
    XMStoreFloat4x4(&Proj, XMMatrixPerspectiveFovLH(
        XMConvertToRadians(45.0f),
        window->AspectRatio(),
        1.0f, 100.0f));

    //Cria��o da Geometria: V�rtices e �ndices

    Grid grid(3.0f, 3.0f, 20, 20);

    for (auto& v : grid.vertices) v.color = XMFLOAT4(DirectX::Colors::DimGray);

    vertices.push_back(grid);

    //Aloca��o e C�pia de Vertex, Index e Constant Buffers para a GPU

    //Grid
    Object gridObj;
    gridObj.mesh = new Mesh();
    gridObj.world = Identity;
    gridObj.mesh->VertexBuffer(grid.VertexData(), grid.VertexCount() * sizeof(Vertex), sizeof(Vertex));
    gridObj.mesh->IndexBuffer(grid.IndexData(), grid.IndexCount() * sizeof(uint), DXGI_FORMAT_R32_UINT);
    gridObj.mesh->ConstantBuffer(sizeof(ObjectConstants), 4);
    gridObj.submesh.indexCount = grid.IndexCount();
    scene.push_back(gridObj);

    //Inicializa as viewports
    StartViewPorts();
    //Inicializa linhas divisorias
    StartDivisionLines();

    BuildRootSignature();
    BuildPipelineState();

    graphics->SubmitCommands();

    timer.Start();
}
void BufferMulti::AddObjectToScene(Geometry& newObj, float scaleX = 0.5f, float scaleY = 0.5f, float scaleZ = 0.5f) {
    graphics->ResetCommands();
    //Colocando cor nos vertices
    for (auto& v : newObj.vertices) {
        v.color = XMFLOAT4(DirectX::Colors::DimGray);
    }

    //Colocando no vetor de vertices que estou usando para mudar a cor
    vertices.push_back(newObj);

    //Objeto
    Object obj;
    XMStoreFloat4x4(&obj.world,
        XMMatrixScaling(scaleX, scaleY, scaleZ) *
        XMMatrixTranslation(0.0f, 0.0f, 0.0f));

    obj.mesh = new Mesh();
    obj.mesh->VertexBuffer(newObj.VertexData(), newObj.VertexCount() * sizeof(Vertex), sizeof(Vertex));
    obj.mesh->IndexBuffer(newObj.IndexData(), newObj.IndexCount() * sizeof(uint), DXGI_FORMAT_R32_UINT);
    obj.mesh->ConstantBuffer(sizeof(ObjectConstants), 4);
    obj.submesh.indexCount = newObj.IndexCount();
    scene.push_back(obj);

    graphics->SubmitCommands();
}

void BufferMulti::DeleteObjectToScene() {
    graphics->ResetCommands();
    if (scene.size() > 0) {
        scene.erase(scene.begin() + tab);
        vertices.erase(vertices.begin() + tab);
        tab = -1;
    }
    graphics->SubmitCommands();
}

void BufferMulti::SelectObjectInScene() {
    graphics->ResetCommands();

    //Reverte a cor do objeto atual antes de selecionar o pr�ximo
    if (!scene.empty() && tab >= 0) {
        for (auto& v : vertices[tab].vertices) {
            v.color = XMFLOAT4(DirectX::Colors::DimGray);
        }

        //Atualiza o buffer do objeto anterior
        scene[tab].mesh->VertexBuffer(vertices[tab].VertexData(), vertices[tab].VertexCount() * sizeof(Vertex), sizeof(Vertex));
        scene[tab].mesh->IndexBuffer(vertices[tab].IndexData(), vertices[tab].IndexCount() * sizeof(uint), DXGI_FORMAT_R32_UINT);
        scene[tab].mesh->ConstantBuffer(sizeof(ObjectConstants), 4);
        scene[tab].submesh.indexCount = vertices[tab].IndexCount();
    }

    //Avan�a para o pr�ximo objeto
    tab++;
    if (tab >= scene.size()) {
        //Volta ao primeiro objeto se ultrapassar o tamanho do vetor
        tab = 0;
    }

    OutputDebugString(std::to_string(tab).c_str());

    //Altera a cor do novo objeto selecionado
    if (!scene.empty()) {
        for (auto& v : vertices[tab].vertices) {
            v.color = XMFLOAT4(DirectX::Colors::Red);
        }

        //Atualiza o buffer do objeto novo
        scene[tab].mesh->VertexBuffer(vertices[tab].VertexData(), vertices[tab].VertexCount() * sizeof(Vertex), sizeof(Vertex));
        scene[tab].mesh->IndexBuffer(vertices[tab].IndexData(), vertices[tab].IndexCount() * sizeof(uint), DXGI_FORMAT_R32_UINT);
        scene[tab].mesh->ConstantBuffer(sizeof(ObjectConstants), 4);
        scene[tab].submesh.indexCount = vertices[tab].IndexCount();
    }

    graphics->SubmitCommands();
}

void BufferMulti::StartViewPorts() {
    //Viewport esquerda cima
    views[0] = { 0.0f, 0.0f, float(window->Width() / 2), float(window->Height() / 2), 0.0f, 1.0f };

    //Viewport direita cima
    views[1] = { float(window->Width() / 2), 0.0f, float(window->Width() / 2), float(window->Height() / 2), 0.0f, 1.0f };

    //Viewport esquerda baixo
    views[2] = { 0.0f, float(window->Height() / 2), float(window->Width() / 2), float(window->Height() / 2), 0.0f, 1.0f };

    //Viewport direita baixo
    views[3] = { float(window->Width() / 2), float(window->Height() / 2), float(window->Width() / 2), float(window->Height() / 2), 0.0f, 1.0f };
}

void BufferMulti::StartDivisionLines() {
    LinhasDivisorias = new Mesh();

    float screenWidth = (float)window->Width();
    float screenHeight = (float)window->Height();

    //Inicio Y
    linhas[0] = { XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT4(DirectX::Colors::White) };
    //Fim Y
    linhas[1] = { XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(DirectX::Colors::White) };
    //Inicio X
    linhas[2] = { XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(DirectX::Colors::White) };
    //Fim Y
    linhas[3] = { XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(DirectX::Colors::White) };

    //Ordem dos indices
    int indexBuffer[4] = { 0,1,2,3 };

    ObjectConstants constants;
    LinhasDivisorias->VertexBuffer(linhas, sizeof(Vertex) * 4, sizeof(Vertex));
    LinhasDivisorias->IndexBuffer(indexBuffer, sizeof(int) * 4, DXGI_FORMAT_R32_UINT);
    LinhasDivisorias->ConstantBuffer(sizeof(constants));
    LinhasDivisorias->CopyConstants(&constants);
}

void BufferMulti::DeselectObject() {
    graphics->ResetCommands();
    if (!scene.empty() && tab >= 0) {
        //Reverte a cor do objeto selecionado para a cor padr�o
        for (auto& v : vertices[tab].vertices) {
            v.color = XMFLOAT4(DirectX::Colors::DimGray);
        }

        //Atualiza o buffer do objeto
        scene[tab].mesh->VertexBuffer(vertices[tab].VertexData(), vertices[tab].VertexCount() * sizeof(Vertex), sizeof(Vertex));
        scene[tab].mesh->IndexBuffer(vertices[tab].IndexData(), vertices[tab].IndexCount() * sizeof(uint), DXGI_FORMAT_R32_UINT);
        scene[tab].mesh->ConstantBuffer(sizeof(ObjectConstants), 4);
        scene[tab].submesh.indexCount = vertices[tab].IndexCount();
    }

    //Reseta o �ndice de sele��o
    tab = -1;
    graphics->SubmitCommands();
}

void BufferMulti::ObjectScale(float x, float y, float z) {
    graphics->ResetCommands();
    //Mudando escala
    XMMATRIX newWorld = XMMatrixScaling(x, y, z) * XMLoadFloat4x4(&scene[tab].world);
    XMStoreFloat4x4(&scene[tab].world,
        newWorld
    );

    ObjectConstants constants;
    XMMATRIX wvp = newWorld * XMLoadFloat4x4(&View) * XMLoadFloat4x4(&Proj);
    XMStoreFloat4x4(&constants.WorldViewProj, XMMatrixTranspose(wvp));
    scene[tab].mesh->CopyConstants(&constants);

    graphics->SubmitCommands();
}

void BufferMulti::ObjectRotation(float x, float y, float z) {
    graphics->ResetCommands();
    //Convertendo para radianos
    x = XMConvertToRadians(x);
    y = XMConvertToRadians(y);
    z = XMConvertToRadians(z);

    XMMATRIX w = XMLoadFloat4x4(&scene[tab].world);

    //Aplica rota��o
    w = XMMatrixRotationX(x) * XMMatrixRotationY(y) * XMMatrixRotationZ(z) * w;


    XMStoreFloat4x4(&scene[tab].world,
        w);

    //Atualiza o buffer
    XMMATRIX wvp = w * XMLoadFloat4x4(&View) * XMLoadFloat4x4(&Proj);

    ObjectConstants constants;
    XMStoreFloat4x4(&constants.WorldViewProj, XMMatrixTranspose(wvp));
    scene[tab].mesh->CopyConstants(&constants);


    graphics->SubmitCommands();
}

void BufferMulti::ObjectTranslate(float x, float y, float z) {
    graphics->ResetCommands();
    //Matriz de mundo
    XMMATRIX w = XMLoadFloat4x4(&scene[tab].world);

    //Realizando transla��o
    XMMATRIX t = XMMatrixTranslation(x, y, z);

    XMMATRIX newWorld = t * w;

    XMStoreFloat4x4(&scene[tab].world, newWorld);

    XMMATRIX wvp = newWorld * XMLoadFloat4x4(&View) * XMLoadFloat4x4(&Proj);

    ObjectConstants constants;
    XMStoreFloat4x4(&constants.WorldViewProj, XMMatrixTranspose(wvp));
    scene[tab].mesh->CopyConstants(&constants);

    graphics->SubmitCommands();
}

void BufferMulti::LoadObject(const std::string& filename) {
    //Novo objeto que vir� dos arquivos
    Geometry newObj;

    std::ifstream file(filename);

    if (!file.is_open()) {
        return;
    }

    std::string line;
    while (getline(file, line)) {
        std::istringstream iss(line);
        string prefix;
        iss >> prefix;

        if (prefix == "v") {
            //V�rtices (posi��es)
            Vertex position;
            iss >> position.pos.x >> position.pos.y >> position.pos.z;
            position.color = XMFLOAT4(DirectX::Colors::DimGray);
            newObj.vertices.push_back(position);
        }
        else if (prefix == "f") {
            //Faces (�ndices de v�rtices)
            uint32_t v1, v2, v3;
            uint32_t n1, n2, n3;
            char slash;
            std::string faceStr;
            getline(iss, faceStr);
            std::istringstream faceStream(faceStr);

            if (faceStr.find("//") != std::string::npos) {
                faceStream >> v1 >> slash >> slash >> n1
                    >> v2 >> slash >> slash >> n2
                    >> v3 >> slash >> slash >> n3;
                newObj.indices.push_back(v1 - 1);
                newObj.indices.push_back(v2 - 1);
                newObj.indices.push_back(v3 - 1);
            }
            else {
                uint32_t vt1, vt2, vt3;
                faceStream >> v1 >> slash >> vt1 >> slash >> n1
                    >> v2 >> slash >> vt2 >> slash >> n2
                    >> v3 >> slash >> vt3 >> slash >> n3;
                newObj.indices.push_back(v1 - 1);
                newObj.indices.push_back(v2 - 1);
                newObj.indices.push_back(v3 - 1);
            }
        }
    }

    if (newObj.vertices.size() > 0) {
        AddObjectToScene(newObj);
    }

    file.close();
}

void BufferMulti::Update()
{
    //Comandos de adi��o
    {
        //Adicionar BOX: B
        if (input->KeyPress('B') || input->KeyPress('b')) {
            //Cria novo Box
            Box newBox(2.0f, 2.0f, 2.0f);
            AddObjectToScene(newBox);
        }

        //Adicionar Cylinder: C
        if (input->KeyPress('C') || input->KeyPress('c')) {
            //Cria novo Cylinder
            Cylinder newCylinder(1.0f, 0.5f, 3.0f, 20, 10);
            AddObjectToScene(newCylinder);
        }

        //Adicionar Sphere: S 
        if (input->KeyPress('S') || input->KeyPress('s')) {
            //Cria nova Sphere
            Sphere newSphere(1.0f, 20, 20);
            AddObjectToScene(newSphere);
        }

        //Adicionar GeoSphere: G 
        if (input->KeyPress('G') || input->KeyPress('g')) {
            //Cria nova GeoSphere
            GeoSphere newGeoSphere(1.0f, 20);
            AddObjectToScene(newGeoSphere);
        }

        //Adicionar Plane(Grid): P
        if (input->KeyPress('P') || input->KeyPress('p')) {
            //Cria novo Grid
            Grid newGrid(3.0f, 3.0f, 20, 20);
            AddObjectToScene(newGrid);
        }

        //Adicionar Quad: Q
        if (input->KeyPress('Q') || input->KeyPress('q')) {
            //Cria novo Quad
            Quad newQuad(2.0f, 2.0f);
            AddObjectToScene(newQuad);
        }
    }

    //Comandos de transforma��o de mundo
    {
        //Tirar tab
        if (input->KeyPress('K') || input->KeyPress('k')) {
            DeselectObject();
        }

        //Apertar DEL para deletar figura selecionada;
        if (input->KeyPress(VK_DELETE)) {

            if (tab > -1) {
                DeleteObjectToScene();
            }

        }

        //Tab para selecionar figura
        if (input->KeyPress(VK_TAB)) {
            SelectObjectInScene();
        }

        // -Aumentar escala: CTRL + E + Seta cima
        // -Diminuir escala: CTRL + E + Seta baixo

         // -Rotacionar no eixo X: CTRL + X + R
         // -Rotacionar no eixo Y: CTRL + Y + R
         // -Rotacionar no eixo Z: CTRL + Z + R

        // -Transacionar no eixo X para Direira: CTRL + X + Seta direita
        // -Transacionar no eixo X para Esquerda: CTRL + X + Seta esquerda

        // -Transacionar no eixo Y para Cima: CTRL + Y + Seta cima
        // -Transacionar no eixo Y para Direira: CTRL + Y + Seta baixo

        // -Transacionar no eixo Z para Tras: CTRL + Z + Seta direita
        // -Transacionar no eixo Z para frente: CTRL + Z + Seta esquerda

        //Aumentar escala do objeto selecionado com combina��o de teclas
        if (input->KeyDown(VK_CONTROL) && ((input->KeyDown('E') || input->KeyDown('e')) && input->KeyPress(VK_UP))) {
            OutputDebugString("Entrei");
            if (tab > -1 && tab < scene.size()) {
                ObjectScale(1.1f, 1.1f, 1.1f);
            }
        }

        //Diminuir escala do objeto selecionado com combina��o de teclas
        if (input->KeyDown(VK_CONTROL) && ((input->KeyDown('E') || input->KeyDown('e')) && input->KeyPress(VK_DOWN))) {
            if (tab > -1 && tab < scene.size()) {
                ObjectScale(0.9f, 0.9f, 0.9f);
            }
        }

        //Rodar no eixo X
        if (input->KeyDown(VK_CONTROL) && (input->KeyDown('X') || input->KeyDown('x')) && (input->KeyPress('R') || (input->KeyPress('r')))) {
            OutputDebugString("Entrei");
            if (tab > -1 && tab < scene.size()) {
                ObjectRotation(-10.0f, 0.0f, 0.0f);
            }
        }

        //Rodar no eixo Y
        if (input->KeyDown(VK_CONTROL) && (input->KeyDown('Y') || input->KeyDown('y')) && (input->KeyPress('R') || (input->KeyPress('r')))) {
            if (tab > -1 && tab < scene.size()) {
                ObjectRotation(0.0f, -10.0f, 0.0f);
            }
        }

        //Rodar no eixo Z
        if (input->KeyDown(VK_CONTROL) && (input->KeyDown('Z') || input->KeyDown('z')) && (input->KeyPress('R') || (input->KeyPress('r')))) {
            if (tab > -1 && tab < scene.size()) {
                ObjectRotation(0.0f, 0.0f, -10.0f);
            }
        }

        //Translacionar no eixo X para direita
        if (input->KeyDown(VK_CONTROL) && (input->KeyDown('X') || input->KeyDown('x')) && input->KeyPress(VK_RIGHT)) {
            OutputDebugString("Entrei");
            if (tab > -1 && tab < scene.size()) {
                ObjectTranslate(1.0f, 0.0f, 0.0f);
            }
        }

        //Translacionar no eixo X para esquerda
        if (input->KeyDown(VK_CONTROL) && (input->KeyDown('X') || input->KeyDown('x')) && input->KeyPress(VK_LEFT)) {
            if (tab > -1 && tab < scene.size()) {
                ObjectTranslate(-1.0f, 0.0f, 0.0f);
            }
        }

        //Translacionar no eixo Y para cima
        if (input->KeyDown(VK_CONTROL) && (input->KeyDown('Y') || input->KeyDown('y')) && input->KeyPress(VK_DOWN)) {
            if (tab > -1 && tab < scene.size()) {
                ObjectTranslate(0.0f, -1.0f, 0.0f);
            }
        }

        //Translacionar no eixo Y para baixo
        if (input->KeyDown(VK_CONTROL) && (input->KeyDown('Y') || input->KeyDown('y')) && input->KeyPress(VK_UP)) {
            if (tab > -1 && tab < scene.size()) {
                ObjectTranslate(0.0f, 1.0f, 0.0f);
            }
        }

        //Translacionar no eixo Z para tras
        if (input->KeyDown(VK_CONTROL) && (input->KeyDown('Z') || input->KeyDown('Z')) && input->KeyPress(VK_RIGHT)) {
            if (tab > -1 && tab < scene.size()) {
                ObjectTranslate(0.0f, 0.0f, -1.0f);
            }
        }

        //Translacionar no eixo Z para frente
        if (input->KeyDown(VK_CONTROL) && (input->KeyDown('Z') || input->KeyDown('Z')) && input->KeyPress(VK_LEFT)) {
            if (tab > -1 && tab < scene.size()) {
                ObjectTranslate(0.0f, 0.0f, 1.0f);
            }
        }
    }

    //Comando de mudan�a de visualiza��o e Carregamento de Objetos
    {
        if (input->KeyPress('V') || input->KeyPress('v')) {
            //Inverte booleano
            viewport = !viewport;
        }

        if (input->KeyPress('1')) {
            LoadObject("thorus.obj");
        }

        if (input->KeyPress('2')) {
            LoadObject("monkey.obj");
        }
            
        if (input->KeyPress('3')) {
            LoadObject("house.obj");
        }

        if (input->KeyPress('4')) {
            LoadObject("ball.obj");
        }

        if (input->KeyPress('5')) {
            LoadObject("capsule.obj");
        }
    }

    //Fechar: ESC
    if (input->KeyPress(VK_ESCAPE))
        window->Close();

    //Ativa ou desativa o giro do objeto: S
    if (input->KeyPress('S'))
    {
        spinning = !spinning;

        if (spinning)
            timer.Start();
        else
            timer.Stop();
    }

    float mousePosX = (float)input->MouseX();
    float mousePosY = (float)input->MouseY();

    if (input->KeyDown(VK_LBUTTON))
    {
        //Cada pixel corresponde a 1/4 de grau
        float dx = XMConvertToRadians(0.25f * (mousePosX - lastMousePosX));
        float dy = XMConvertToRadians(0.25f * (mousePosY - lastMousePosY));

        //Atualiza �ngulos com base no deslocamento do mouse 
        //Orbitar a c�mera ao redor da caixa
        theta += dx;
        phi += dy;

        //Restringe o �ngulo de phi ]0-180[ graus
        phi = phi < 0.1f ? 0.1f : (phi > (XM_PI - 0.1f) ? XM_PI - 0.1f : phi);
    }
    else if (input->KeyDown(VK_RBUTTON))
    {
        //Cada pixel corresponde a 0.05 unidades
        float dx = 0.05f * (mousePosX - lastMousePosX);
        float dy = 0.05f * (mousePosY - lastMousePosY);

        //Atualiza o raio da c�mera com base no deslocamento do mouse 
        radius += dx - dy;

        //Restringe o raio (3 a 15 unidades)
        radius = radius < 3.0f ? 3.0f : (radius > 15.0f ? 15.0f : radius);
    }

    lastMousePosX = mousePosX;
    lastMousePosY = mousePosY;

    //Converte coordenadas esf�ricas para cartesianas
    float x = radius * sinf(phi) * cosf(theta);
    float z = radius * sinf(phi) * sinf(theta);
    float y = radius * cosf(phi);

    //Vis�es 
    {
        //Constr�i a matriz da c�mera (view matrix) Pespectiva
        XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
        XMVECTOR target = XMVectorZero();
        XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
        XMStoreFloat4x4(&View, view);

        //Carrega matriz de proje��o em uma XMMATRIX
        XMMATRIX proj = XMLoadFloat4x4(&Proj);

        //Proje��o ortogr�fica
        XMMATRIX O = XMMatrixOrthographicLH(5, 5, 1.0f, 100.0f);

        //Matriz de visualiza��o ortografica e visao cima
        XMVECTOR posUp = XMVectorSet(0, 10, 0, 1);
        XMVECTOR targetUp = XMVectorZero();
        XMVECTOR upUp = XMVectorSet(0, 0, -1, 0);
        XMMATRIX VUp = XMMatrixLookAtLH(posUp, targetUp, upUp);

        //Matriz de visualiza��o ortogr�fica para a vis�o frontal
        XMVECTOR posFront = XMVectorSet(0, 0, -10, 1);
        XMVECTOR targetFront = XMVectorZero();
        XMVECTOR upFront = XMVectorSet(0, 1, 0, 0);
        XMMATRIX VFront = XMMatrixLookAtLH(posFront, targetFront, upFront);

        //Matriz de visualiza��o ortogr�fica para a vis�o direita
        XMVECTOR posSide = XMVectorSet(10, 0, 0, 1);
        XMVECTOR targetSide = XMVectorZero();
        XMVECTOR upSide = XMVectorSet(0, 1, 0, 0);
        XMMATRIX VSide = XMMatrixLookAtLH(posSide, targetSide, upSide);

        //Ajusta o buffer constante de cada objeto

        for (auto& obj : scene)
        {
            //Carrega matriz de mundo em uma XMMATRIX
            XMMATRIX world = XMLoadFloat4x4(&obj.world);

            //Constr�i matriz combinada (world x view x proj)
            XMMATRIX WorldViewProj = world * view * proj;

            //Atualiza o buffer constante com a matriz combinada
            ObjectConstants constants;
            XMStoreFloat4x4(&constants.WorldViewProj, XMMatrixTranspose(WorldViewProj));
            obj.mesh->CopyConstants(&constants); //Pespectiva

            //Constr�i matriz combinada (world x view x proj)
            WorldViewProj = world * VFront * O;

            //Atualiza o buffer constante com a matriz combinada
            XMStoreFloat4x4(&constants.WorldViewProj, XMMatrixTranspose(WorldViewProj));
            obj.mesh->CopyConstants(&constants, 1); //Ortogonal FRONT

            //Constr�i matriz combinada (world x view x proj)
            WorldViewProj = world * VUp * O;

            //Atualiza o buffer constante com a matriz combinada
            XMStoreFloat4x4(&constants.WorldViewProj, XMMatrixTranspose(WorldViewProj));
            obj.mesh->CopyConstants(&constants, 2); //Ortogonal UP

            //Constr�i matriz combinada (world x view x proj)
            WorldViewProj = world * VSide * O;

            //Atualiza o buffer constante com a matriz combinada
            XMStoreFloat4x4(&constants.WorldViewProj, XMMatrixTranspose(WorldViewProj));
            obj.mesh->CopyConstants(&constants, 3); //Ortogonal SIDE
        }
    }
}

void BufferMulti::Draw()
{
    //Limpa o backbuffer
    graphics->Clear(pipelineState[0]);

    if (!viewport) {
        //Desenha objetos da cena
        for (auto& obj : scene)
        {
            //Comandos de configura��o do pipeline
            ID3D12DescriptorHeap* descriptorHeap = obj.mesh->ConstantBufferHeap();
            graphics->CommandList()->SetDescriptorHeaps(1, &descriptorHeap);
            graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
            graphics->CommandList()->IASetVertexBuffers(0, 1, obj.mesh->VertexBufferView());
            graphics->CommandList()->IASetIndexBuffer(obj.mesh->IndexBufferView());
            graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            //Ajusta o buffer constante associado ao vertex shader
            graphics->CommandList()->SetGraphicsRootDescriptorTable(0, obj.mesh->ConstantBufferHandle(0));

            //Desenha objeto
            graphics->CommandList()->DrawIndexedInstanced(
                obj.submesh.indexCount, 1,
                obj.submesh.startIndex,
                obj.submesh.baseVertex,
                0);
        }
    }
    else {
        //Seta Pipeline que desenha linhas
        graphics->CommandList()->SetPipelineState(pipelineState[1]);
        ID3D12DescriptorHeap* descriptorHeapLinhas = LinhasDivisorias->ConstantBufferHeap();
        graphics->CommandList()->SetDescriptorHeaps(1, &descriptorHeapLinhas);
        graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
        graphics->CommandList()->IASetVertexBuffers(0, 1, LinhasDivisorias->VertexBufferView());
        graphics->CommandList()->IASetIndexBuffer(LinhasDivisorias->IndexBufferView());
        graphics->CommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);

        //Ajusta o buffer constante associado ao vertex shader
        graphics->CommandList()->SetGraphicsRootDescriptorTable(0, LinhasDivisorias->ConstantBufferHandle(0));

        //Desenha objeto
        graphics->CommandList()->DrawIndexedInstanced(
            4, 1,
            0,
            0,
            0);

        //Volta para triangulos
        graphics->CommandList()->SetPipelineState(pipelineState[0]);
        //For que passara pelos indices do ConstantBuffer que guardam as vis�es diferentes
        for (int i{}; i < 4; i++) {

            graphics->CommandList()->RSSetViewports(1, &views[i]);

            for (auto& obj : scene)
            {
                //Comandos de configura��o do pipeline
                ID3D12DescriptorHeap* descriptorHeap = obj.mesh->ConstantBufferHeap();
                graphics->CommandList()->SetDescriptorHeaps(1, &descriptorHeap);
                graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
                graphics->CommandList()->IASetVertexBuffers(0, 1, obj.mesh->VertexBufferView());
                graphics->CommandList()->IASetIndexBuffer(obj.mesh->IndexBufferView());
                graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

                //Ajusta o buffer constante associado ao vertex shader
                graphics->CommandList()->SetGraphicsRootDescriptorTable(0, obj.mesh->ConstantBufferHandle(i));

                //Desenha objeto
                graphics->CommandList()->DrawIndexedInstanced(
                    obj.submesh.indexCount, 1,
                    obj.submesh.startIndex,
                    obj.submesh.baseVertex,
                    0);
            }
        }
    }

    //Apresenta o backbuffer na tela
    graphics->Present();
}

void BufferMulti::Finalize()
{
    rootSignature->Release();
    pipelineState[0]->Release();
    pipelineState[1]->Release();

    delete LinhasDivisorias;

    for (auto& obj : scene)c
        delete obj.mesh;
}

//D3D                                      

void BufferMulti::BuildRootSignature()
{
    //Cria uma �nica tabela de descritores de CBVs
    D3D12_DESCRIPTOR_RANGE cbvTable = {};
    cbvTable.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    cbvTable.NumDescriptors = 1;
    cbvTable.BaseShaderRegister = 0;
    cbvTable.RegisterSpace = 0;
    cbvTable.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    //Define par�metro raiz com uma tabela
    D3D12_ROOT_PARAMETER rootParameters[1];
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
    rootParameters[0].DescriptorTable.pDescriptorRanges = &cbvTable;

    //Assinatura raiz � um vetor de par�metros raiz
    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.NumParameters = 1;
    rootSigDesc.pParameters = rootParameters;
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

    if (error != nullptr)
    {
        OutputDebugString((char*)error->GetBufferPointer());
    }

    //Cria uma assinatura raiz com um �nico slot que aponta para  
    //Faixa de descritores consistindo de um �nico buffer constante
    ThrowIfFailed(graphics->Device()->CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature)));
}

void BufferMulti::BuildPipelineState()
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
    //Rasterizer.FillMode = D3D12_FILL_MODE_SOLID;
    rasterizer.FillMode = D3D12_FILL_MODE_WIREFRAME;
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

    //Color Blender
    D3D12_BLEND_DESC blender = {};
    blender.AlphaToCoverageEnable = FALSE;
    blender.IndependentBlendEnable = FALSE;
    const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
    {
        FALSE,FALSE,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL,
    };
    for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
        blender.RenderTarget[i] = defaultRenderTargetBlendDesc;

    //Depth Stencil
    D3D12_DEPTH_STENCIL_DESC depthStencil = {};
    depthStencil.DepthEnable = TRUE;
    depthStencil.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencil.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    depthStencil.StencilEnable = FALSE;
    depthStencil.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    depthStencil.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
    const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
    { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
    depthStencil.FrontFace = defaultStencilOp;
    depthStencil.BackFace = defaultStencilOp;
    
    //Pipeline State Object (PSO)
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso = {};
    pso.pRootSignature = rootSignature;
    pso.VS = { reinterpret_cast<BYTE*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
    pso.PS = { reinterpret_cast<BYTE*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
    pso.BlendState = blender;
    pso.SampleMask = UINT_MAX;
    pso.RasterizerState = rasterizer;
    pso.DepthStencilState = depthStencil;
    pso.InputLayout = { inputLayout, 2 };
    pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso.NumRenderTargets = 1;
    pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pso.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    pso.SampleDesc.Count = graphics->Antialiasing();
    pso.SampleDesc.Quality = graphics->Quality();
    graphics->Device()->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipelineState[0]));

    pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE; //Nova topologia para o segundo pipeline
    graphics->Device()->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipelineState[1]));

    vertexShader->Release();
    pixelShader->Release();
}


//WinMain                                      

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow)
{
    try
    {
        //Cria motor e configura a janela
        Engine* engine = new Engine();
        engine->window->Mode(WINDOWED);
        engine->window->Size(1024, 720);
        engine->window->Color(25, 25, 25);
        engine->window->Title("BufferMulti");
        engine->window->Icon(IDI_ICON);
        engine->window->Cursor(IDC_CURSOR);
        engine->window->LostFocus(Engine::Pause);
        engine->window->InFocus(Engine::Resume);

        //Cria e executa a aplica��o
        engine->Start(new BufferMulti());

        //Finaliza execu��o
        delete engine;
    }
    catch (Error & e)
    {
        //Exibe mensagem em caso de erro
        MessageBox(nullptr, e.ToString().data(), "BufferMulti", MB_OK);
    }

    return 0;
}