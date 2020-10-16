#include "stdafx.h"
#include "PostProcessingMaterial.h"
#include "RenderTarget.h"
#include "OverlordGame.h"
#include "ContentManager.h"

PostProcessingMaterial::PostProcessingMaterial(std::wstring effectFile, unsigned int renderIndex,
                                               std::wstring technique)
	: m_IsInitialized(false), 
	  m_pInputLayout(nullptr),
	  m_pInputLayoutSize(0),
	  m_effectFile(std::move(effectFile)),
	  m_InputLayoutID(0),
	  m_RenderIndex(renderIndex),
	  m_pRenderTarget(nullptr),
	  m_pVertexBuffer(nullptr),
	  m_pIndexBuffer(nullptr),
	  m_NumVertices(0),
	  m_NumIndices(0),
	  m_VertexBufferStride(0),
	  m_pEffect(nullptr),
	  m_pTechnique(nullptr),
	  m_TechniqueName(std::move(technique))
{
}

PostProcessingMaterial::~PostProcessingMaterial()
{
	//TODO: delete and/or release necessary objects and/or resources

	SafeRelease(m_pVertexBuffer);
	SafeRelease(m_pIndexBuffer);
	SafeRelease(m_pInputLayout);
	SafeDelete(m_pRenderTarget);
	m_pInputLayoutDescriptions.clear();
}

void PostProcessingMaterial::Initialize(const GameContext& gameContext)
{
	if (!m_IsInitialized)
	{
		// 1. LoadEffect (LoadEffect(...))
		if (!LoadEffect(gameContext, m_effectFile))
			Logger::LogWarning(L"PostProcessingMaterial::Initialize( ) >> Problem loading effect!");

		// CreateVertexBuffer (CreateVertexBuffer(...)) > As a TriangleStrip (FullScreen Quad)
		CreateVertexBuffer(gameContext);
		CreateIndexBuffer(gameContext);

		// 2. CreateInputLaytout (CreateInputLayout(...))
		bool res = EffectHelper::BuildInputLayout(gameContext.pDevice, m_pTechnique, &m_pInputLayout, m_pInputLayoutSize);
		if (!res) Logger::LogWarning(L"PostProcessingMaterial::Initialize( ) >> Problem creating the correct InputLayout!");

		// 3. Create RenderTarget (m_pRenderTarget)
		RENDERTARGET_DESC RT_Desc;
		RT_Desc.Width = OverlordGame::GetGameSettings().Window.Width;
		RT_Desc.Height = OverlordGame::GetGameSettings().Window.Height;
		RT_Desc.EnableColorSRV = true;
		RT_Desc.EnableColorBuffer = true;
		RT_Desc.EnableDepthBuffer = true;
		RT_Desc.EnableDepthSRV = true;
		RT_Desc.GenerateMipMaps_Color = true;

		m_pRenderTarget = new RenderTarget{ gameContext.pDevice };
		HRESULT resi = m_pRenderTarget->Create(RT_Desc);
		Logger::LogHResult(resi, L"PostProcessingMaterial::Initialize()");

		m_IsInitialized = true;
	}
}

bool PostProcessingMaterial::LoadEffect(const GameContext& gameContext, const std::wstring& effectFile)
{
	UNREFERENCED_PARAMETER(gameContext);

	//Load Effect through ContentManager
	m_pEffect = ContentManager::Load<ID3DX11Effect>(effectFile);
	if (!m_pEffect->IsValid())
	{
		Logger::LogWarning(L"PostProcessingMaterial::LoadEffect( ) >> Problem loading effect with file: \n" + effectFile);
		return false;
	}
	//Check if m_TechniqueName (default constructor parameter) is set
	// TODO: I would rather not!

	// If SET > Use this Technique (+ check if valid)
	// If !SET > Use Technique with index 0
	m_pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (!m_pTechnique)
	{
		Logger::LogWarning(L"PostProcessingMaterial::LoadEffect( ) >> Problem loading Technique at idx 0");
		return false;
	}

	//Call LoadEffectVariables
	LoadEffectVariables();

	return true;
}

void PostProcessingMaterial::Draw(const GameContext& gameContext, RenderTarget* previousRendertarget)
{
	//1. Clear the My RenderTarget (m_pRenderTarget) [Check RenderTarget Class]
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_pRenderTarget->Clear(gameContext, clearColor);

	// 2. Call UpdateEffectVariables(...)
	UpdateEffectVariables(previousRendertarget);

	// 3. Set InputLayout
	ID3D11DeviceContext* pDevCon = gameContext.pDeviceContext;
	pDevCon->IASetInputLayout(m_pInputLayout);

	// 4. Set VertexBuffer
	const size_t stride = sizeof(VertexPosTex);
	const size_t offset{};
	pDevCon->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDevCon->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// 5. Set PrimitiveTopology (TRIANGLELIST)
	pDevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 6. Draw 
	D3DX11_TECHNIQUE_DESC desc{};
	m_pTechnique->GetDesc(&desc);
	for (size_t p{}; p < desc.Passes; ++p)
	{
		m_pTechnique->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}

	// Generate Mips
	gameContext.pDeviceContext->GenerateMips(m_pRenderTarget->GetShaderResourceView());
}

void PostProcessingMaterial::CreateVertexBuffer(const GameContext& gameContext)
{
	if (m_pVertexBuffer) return;

	// Trianglestrip
	m_NumVertices = 4;

	// TODO: not goood?
	//Create vertex array containing three elements in system memory
	//fill a buffer description to copy the vertexdata into graphics memory
	//create a ID3D10Buffer in graphics memory containing the vertex info
	VertexPosTex buffer[4]{
		VertexPosTex{DirectX::XMFLOAT3{-1.f,1.f,0.f}, DirectX::XMFLOAT2{0,0}},
		VertexPosTex{DirectX::XMFLOAT3{1.f ,1.f,0.f}, DirectX::XMFLOAT2{1,0}},
		VertexPosTex{DirectX::XMFLOAT3{-1.f,-1.f,0.f}, DirectX::XMFLOAT2{0,1}},
		VertexPosTex{DirectX::XMFLOAT3{1.f,-1.f,0.f }, DirectX::XMFLOAT2{1,1}}
	};

	D3D11_BUFFER_DESC desc{};
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(VertexPosTex) * m_NumVertices;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = buffer;

	HRESULT res = gameContext.pDevice->CreateBuffer(&desc, &initData, &m_pVertexBuffer);
	Logger::LogHResult(res, L"PostProcessingMaterial::CreateVertexBuffer()");
}

void PostProcessingMaterial::CreateIndexBuffer(const GameContext& gameContext)
{
	if (m_pIndexBuffer) return;

	m_NumIndices = 6;

	unsigned int indices[6]{ 0, 1, 2, 2, 1, 3 };

	// Create index buffer
	D3D11_BUFFER_DESC desc{};
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(unsigned int) * m_NumIndices;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &indices;
	
	HRESULT res = gameContext.pDevice->CreateBuffer(&desc, &initData, &m_pIndexBuffer);
	Logger::LogHResult(res, L"PostProcessingMaterial::CreateIndexBuffer()");
}
