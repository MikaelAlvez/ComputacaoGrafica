#ifndef DXUT_GRAPHICS_H
#define DXUT_GRAPHICS_H

// --------------------------------------------------------------------------------
// Inclus�es

#include <dxgi1_6.h>    // infraestrutura gr�fica do DirectX
#include <d3d12.h>      // principais fun��es do Direct3D
#include "Window.h"     // cria e configura uma janela do Windows
#include "Types.h"      // tipos espec�ficos da engine
#include <D3DCompiler.h>

enum AllocationType {GPU, UPLOAD, CBUFFER};

// --------------------------------------------------------------------------------

class Graphics
{
private:
    // configura��es
    uint backBufferCount;                                   // n�mero de buffers no swap chain
    uint antialiasing;                                      // n�mero de amostras para cada pixel na tela
    uint quality;                                           // qualidade de amostras de antialiasing
    bool vSync;                                             // vertical sync
    float bgColor[4];                                       // cor de fundo do back buffer

    // pipeline
    ID3D12Device8* device;                                  // dispositivo gr�fico
    IDXGIFactory6* factory;                                 // interface principal da DXGI
    IDXGISwapChain1* swapChain;                             // swap chain
    uint backBufferIndex;                                   // �ndice do backbuffer atual

    ID3D12CommandQueue* commandQueue;                       // fila de comandos da GPU
    ID3D12GraphicsCommandList* commandList;                 // lista de comandos a submeter para GPU
    ID3D12CommandAllocator* commandListAlloc;               // mem�ria utilizada pela lista de comandos

    ID3D12Resource** renderTargets;                         // buffers para renderiza��o (front e back)
    ID3D12Resource* depthStencil;                           // buffer de profundidade de estampa
    ID3D12DescriptorHeap* renderTargetHeap;                 // descriptor heap para os Render Targets
    ID3D12DescriptorHeap* depthStencilHeap;                 // descriptor heap para o Depth Stencil
    uint rtDescriptorSize;                                  // tamanho de cada descritor de Render Target
    D3D12_VIEWPORT viewport;                                // viewport
    D3D12_RECT scissorRect;                                 //ret�ngulo de corte

    // sincroniza��o                         
    ID3D12Fence* fence;						                // barreira para sincronizar CPU/GPU
    ullong currentFence;				                    // contador de barreiras

    // m�todos privados
    void LogHardwareInfo();									// mostra informa��es do hardware
    bool WaitCommandQueue();								// espera execu��o da fila de comandos

public:
    Graphics();												// constructor
    ~Graphics();											// destructor

    void VSync(bool state);                                 // liga/desliga o vertical sync
    void Initialize(Window* window);						// inicializa o Direct3D
    void Clear(ID3D12PipelineState* pso);                   // limpa o backbuffer com a cor de fundo
    void Present();                                         // apresenta o desenho na tela
    
    void ResetCommands();                                   // reinicia lista para receber novos comandos
    void SubmitCommands();									// submete para execu��o os comandos pendentes

    void Allocate(uint sizeInBytes,
        ID3DBlob** resource);                               // aloca mem�ria da CPU para recurso

    void Allocate(uint type,
        uint sizeInBytes,
        ID3D12Resource** resource);                         // aloca mem�ria da CPU para recurso

    void Copy(const void* vertice,
        uint sizeInBytes,
        ID3DBlob* bufferCPU);                               // copia vertices para Blob na CPU

    void Copy(const void* vertice, 
            uint sizeInBytes,
            ID3D12Resource* bufferUpload,
            ID3D12Resource* bufferGPU);                     // copia vertices para a GPU

    ID3D12Device4* Device();                                // retorna dispositivo Direct3D
    ID3D12GraphicsCommandList* CommandList();               // retorna lista de comando
    uint Antialiasing();                                    // retorna n�mero de amostra por pixel
    uint Quality();                                         // retorna a quantidade de amostras
};

// --------------------------------------------------------------------------------
// M�todods Inline

// liga/desliga vertical sync
inline void Graphics::VSync(bool state)
{ vSync = state; }

// retorna dispositivo Direct3D
inline ID3D12Device4* Graphics::Device()
{ return device; }

// retorna lista de comandos do dispositivo
inline ID3D12GraphicsCommandList* Graphics::CommandList()
{ return commandList; }

// retorna n�mero de amostras por pixel
inline uint Graphics::Antialiasing()
{ return antialiasing; }

// retorna qualidade das amostras
inline uint Graphics::Quality()
{ return quality; }

#endif