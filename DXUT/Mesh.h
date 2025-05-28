#ifndef DXUT_MESH_H
#define DXUT_MESH_H

#include <d3d12.h>
#include "types.h"
#include <string>
#include <unordered_map>
using std::string;
using std::unordered_map;

struct SubMesh
{
	uint indexCount = 0;
	uint startIndex = 0;
	uint baseVertex = 0;
};

struct Mesh
{
	// identificador para recuperar a malha pelo seu nome
	string id;



	// buffers de Upload CPU -> GPU
	ID3D12Resource* vertexBufferUpload;
	ID3D12Resource* vertexBufferGPU;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	uint vertexBufferStride;
	uint vertexBufferSize;


	// buffers na GPU
	ID3D12Resource* indexBufferUpload;
	ID3D12Resource* indexBufferGPU;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	DXGI_FORMAT indexFormat;
	uint indexBufferSize;


	ID3D12DescriptorHeap* cbufferHeap;           // heap de descritores do buffer constante
	ID3D12Resource* cBufferUpload;               // buffer de Upload CPU -> GPU
	byte* cbufferData;                           // buffer na CPU
	uint cbufferDescriptorSize;                                             // tamanho do descritor 
	uint cbufferElementSize;                                                // tamanho de um elemento no buffer 


	// uma malha pode armazenar múltiplas sub-malhas
	unordered_map<string, SubMesh> subMesh;

	Mesh();                                             // construtor
	~Mesh();                                            // destrutor
	Mesh(string name);

	void VertexBuffer(const void* vb, uint vbSize, uint vbStride);          // aloca e copia vértices para vertex buffer 
	void IndexBuffer(const void* ib, uint ibSize, DXGI_FORMAT ibFormat);    // aloca e copia índices para index buffer 
	void ConstantBuffer(uint cbSize, uint objCount = 1);                                       // aloca constant buffer com tamanho solicitado
	
	void CopyConstants(const void* cb, uint cbIndex = 0);    // copia dados para o constant buffer

	// retorna descritor (view) do Vertex Buffer
	D3D12_VERTEX_BUFFER_VIEW * VertexBufferView();
	D3D12_INDEX_BUFFER_VIEW * IndexBufferView();
	ID3D12DescriptorHeap* ConstantBufferHeap();         // retorna heap de descritores
	D3D12_GPU_DESCRIPTOR_HANDLE ConstantBufferHandle(uint cbIndex = 0);     // retorna handle de um descritor

};

// -------------------------------------------------------------------------------

#endif