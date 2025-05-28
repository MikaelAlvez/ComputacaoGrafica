#include "DXUT.h"
#include <D3DCompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <sstream>

using std::stringstream;
using namespace DirectX;

// ------------------------------------------------------------------------------

class WinApp : public App
{
private:
	stringstream textSize;
	stringstream textMode;
	stringstream textMouse;

	int lastX, lastY;

public:
	void Init();
	void Update();
	void Display();
	void Finalize();
};


void WinApp::Init()
{
	// captura posição do mouse
	lastX = input->MouseX();
	lastY = input->MouseY();

	// captura tamanho e tipo de janela
	textSize << "Tamanho: " << window->Width() << " x " << window->Height();
	textMode << "Formato: " << (window->Mode() == WINDOWED ? "Em Janela" : "Tela Cheia");
	textMouse << input->MouseX() << " x " << input->MouseY();
}


void WinApp::Update()
{
	// sai com o pressionamento da tecla ESC
	if (input->KeyPress(VK_ESCAPE))
		window->Close();

	// converte posição do mouse em texto para exibição
	textMouse.str("");
	textMouse << input->MouseX() << " x " << input->MouseY();

	// se o mouse mudou de posição
	if (lastX != input->MouseX() || lastY != input->MouseY())
	{
		window->Clear();
		Display();
	}

	// atualiza posição do mouse
	lastX = input->MouseX();
	lastY = input->MouseY();
}


void WinApp::Display()
{
	window->Print("Window App Demo", 10, 10, RGB(255, 255, 255));
	window->Print(textSize.str(), 10, 50, RGB(255, 255, 255));
	window->Print(textMode.str(), 10, 75, RGB(255, 255, 255));
	window->Print(textMouse.str(), 10, 100, RGB(255, 255, 255));
}


void WinApp::Finalize()
{
}

// ------------------------------------------------------------------------------

struct Vertex{ 
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

class DXApp :
	public App
{
	private:
	ID3D12RootSignature * rootSignature;
	ID3D12PipelineState * pipelineState;


	Mesh * geometry;

	public:
	void Init();
	void Update();
	void Display();
	void Finalize();
	void OnPause();


	void BuildGeometry();
	void BuildRootSignature();
	void BuildPipelineState();	
};

void DXApp::Init()
{
	// contrói geometria e inicializa pipeline
	graphics->ResetCommands();
	// ---------------------------------------
	BuildGeometry();
	BuildRootSignature();
	BuildPipelineState();
	// ---------------------------------------
	graphics->SubmitCommands();
}

void DXApp::Update()
{
	// sai com o pressionamento da tecla ESC
	if (input->KeyPress(VK_ESCAPE))
		window->Close();
}

void DXApp::Display()
{
	graphics->Clear(pipelineState);

	// submete comandos de configuração do pipeline
	graphics->CommandList()->SetGraphicsRootSignature(rootSignature);

	graphics->CommandList()->IASetVertexBuffers(0, 1, geometry->VertexBufferView());

	graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// submete comandos de desenho
	graphics->CommandList()->DrawInstanced(6, 1, 0, 0);

	

	graphics->Present();
}

void DXApp::OnPause()
{
}

void DXApp::Finalize()
{
	rootSignature->Release();
	delete geometry;
	pipelineState->Release();
}

// ------------------------------------------------------------------------------
//                                  D3D                                      
// ------------------------------------------------------------------------------

void DXApp::BuildGeometry()
{
	// vértices da geometria
	const uint numVertices = 6;
	Vertex vertices[numVertices] =
	{
		{ XMFLOAT3(-.5f, .5f, .01f), XMFLOAT4(Colors::Aquamarine) },
		{ XMFLOAT3(.5f, -.5f, .0f), XMFLOAT4(Colors::CornflowerBlue) },
		{ XMFLOAT3(-.5f, -.5f, .5f), XMFLOAT4(Colors::CornflowerBlue) },
		{ XMFLOAT3(-.5f, .5f, .0f), XMFLOAT4(Colors::Aqua) },
		{ XMFLOAT3(.5f, .5f, .5f), XMFLOAT4(Colors::Aqua) },
		{ XMFLOAT3(.5f, -.5f, .01f), XMFLOAT4(Colors::CornflowerBlue	) },
	};


	// tamanho em bytes dos vértices
	const uint vbSize = numVertices * sizeof(Vertex);

	// cria malha 3D
	geometry = new Mesh("Triangle");

	// ajusta atributos da malha 3D
	geometry->vertexBufferStride = sizeof(Vertex);
	geometry->vertexBufferSize = vbSize;

	// aloca recursos para o vertex buffer
	//graphics->Allocate(vbSize, &geometry->vertexBufferCPU);
	graphics->Allocate(UPLOAD, vbSize, &geometry->vertexBufferUpload);
	graphics->Allocate(GPU, vbSize, &geometry->vertexBufferGPU);

	// copia vértices para o armazenamento local da malha
	//graphics->Copy(vertices, vbSize, geometry->vertexBufferCPU);

	// copia vértices para o buffer da GPU usando o buffer de Upload
	graphics->Copy(vertices, vbSize, geometry->vertexBufferUpload, geometry->vertexBufferGPU);
}

void DXApp::BuildRootSignature()
{
	// descrição para uma assinatura vazia
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
	rootSigDesc.NumParameters = 0;
	rootSigDesc.pParameters = nullptr;
	rootSigDesc.NumStaticSamplers = 0;
	rootSigDesc.pStaticSamplers = nullptr;
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// serializa assinatura raiz
	ID3DBlob* serializedRootSig = nullptr;
	ID3DBlob* error = nullptr;

	ThrowIfFailed(D3D12SerializeRootSignature(
		&rootSigDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&serializedRootSig,
		&error));

	// cria uma assinatura raiz vazia
	ThrowIfFailed(graphics->Device()->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature)));

}

void DXApp::BuildPipelineState()
{
	// --------------------
	// --- Input Layout ---
	// --------------------

	D3D12_INPUT_ELEMENT_DESC inputLayout[2] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// --------------------
	// ----- Shaders ------
	// --------------------

	ID3DBlob* vertexShader;
	ID3DBlob* pixelShader;

	D3DReadFileToBlob(L"../x64/Debug/Vertex.cso", &vertexShader);
	D3DReadFileToBlob(L"../x64/Debug/Pixel.cso", &pixelShader);

	// --------------------
	// ---- Rasterizer ----
	// --------------------

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

	// ---------------------
	// --- Color Blender ---
	// ---------------------

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

	// ---------------------
	// --- Depth Stencil ---
	// ---------------------

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

	// -----------------------------------
	// --- Pipeline State Object (PSO) ---
	// -----------------------------------

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso = {};
	pso.pRootSignature = rootSignature;
	pso.VS = { reinterpret_cast<BYTE*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
	pso.PS = { reinterpret_cast<BYTE*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
	pso.BlendState = blender;
	pso.SampleMask = UINT_MAX;
	pso.RasterizerState = rasterizer;
	pso.DepthStencilState = depthStencil;
	pso.InputLayout = { inputLayout, 2 };
	pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	pso.NumRenderTargets = 1;
	pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pso.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	pso.SampleDesc.Count = graphics->Antialiasing();
	pso.SampleDesc.Quality = graphics->Quality();
	graphics->Device()->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipelineState));

	vertexShader->Release();
	pixelShader->Release();

}

// ------------------------------------------------------------------------------
//                                  WinMain                                      
// ------------------------------------------------------------------------------

//int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
//	_In_ LPSTR lpCmdLine, _In_ int nCmdShow)
//{
//	try
//	{
//		// cria motor
//		Engine* engine = new Engine();
//
//		// configura a janela
//		engine->window->Mode(WINDOWED);
//		engine->window->Size(1024, 600);
//		engine->window->Color(0, 0, 0);
//		engine->window->Title("DXGI DXUT");
//		engine->window->Icon(IDI_ICON);
//		engine->window->Cursor(IDC_CURSOR);
//
//		// aplicação pausa/resume ao perder/ganhar o foco
//		engine->window->LostFocus(Engine::Pause);
//		engine->window->InFocus(Engine::Resume);
//
//		// cria e executa a aplicação
//		int exitCode = engine->Start(new DXApp());
//
//		// finaliza execução
//		delete engine;
//		return exitCode;
//	}
//	catch (Error& e)
//	{
//		MessageBox(nullptr, e.ToString().data(), "DXGI DXUT", MB_OK);
//		return 0;
//	}
//}

