#include "stdafx.h"
#include "ShadowMapRenderer.h"
#include "ContentManager.h"
#include "ShadowMapMaterial.h"
#include "RenderTarget.h"
#include "MeshFilter.h"
#include "SceneManager.h"
#include "OverlordGame.h"

ShadowMapRenderer::~ShadowMapRenderer()
{
	// Make sure you don't have memory leaks and/or resource leaks :) -> Figure out if you need to do something here
	SafeDelete(m_pShadowMat);
	SafeDelete(m_pShadowRT);
}

void ShadowMapRenderer::Initialize(const GameContext& gameContext)
{
	if (m_IsInitialized)
		return;
	
	m_pShadowMat = new ShadowMapMaterial{};
	m_pShadowMat->Initialize(gameContext);

	RENDERTARGET_DESC RT_Desc;
	RT_Desc.EnableDepthBuffer = true;
	RT_Desc.EnableDepthSRV = true;
	RT_Desc.EnableColorBuffer = false;
	RT_Desc.EnableColorSRV = false;
	RT_Desc.Width = OverlordGame::GetGameSettings().Window.Width;
	RT_Desc.Height = OverlordGame::GetGameSettings().Window.Height;

	m_pShadowRT = new RenderTarget{ gameContext.pDevice };
	m_pShadowRT->Create(RT_Desc);

	m_IsInitialized = true;
}

void ShadowMapRenderer::SetLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction)
{
	m_LightDirection = direction;
	m_LightPosition = position;

	m_LightVP = CalculateVP(position, direction, 0.1f, 500.f, false);
}

DirectX::XMFLOAT4X4 ShadowMapRenderer::CalculateVP(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& dir, float nearZ, float farZ, bool perspective)
{
	using namespace DirectX;
	XMFLOAT4X4 result{};
	XMFLOAT3 globalUp{ 0.f, 1.f, 0.f };

	FXMVECTOR positionVec = XMLoadFloat3(&pos);
	FXMVECTOR directionVec = XMLoadFloat3(&dir);
	FXMVECTOR globalUpVec = XMLoadFloat3(&globalUp);

	// Inverse of transform
	XMMATRIX viewMat = XMMatrixLookToLH(positionVec, directionVec, globalUpVec);
	XMMATRIX projMat;

	if (!perspective)
	{
		// Orthographic:
		float x = m_Size * OverlordGame::GetGameSettings().Window.AspectRatio;
		float y = m_Size;

		projMat = XMMatrixOrthographicLH(x, y, nearZ, farZ); // TOF
	}
	else
	{
		// Perspective:
		float fovAngleY = 90.f;
		projMat = XMMatrixPerspectiveFovLH(fovAngleY, OverlordGame::GetGameSettings().Window.AspectRatio, nearZ, farZ);
	}

	XMMATRIX viewProjMat = viewMat * projMat;
	XMStoreFloat4x4(&result, viewProjMat);

	return result;
}

void ShadowMapRenderer::Begin(const GameContext& gameContext) const
{
	//Reset Texture Register 5 (Unbind)
	ID3D11ShaderResourceView* const pSRV[] = { nullptr };
	gameContext.pDeviceContext->PSSetShaderResources(1, 1, pSRV);

	// Set the appropriate render target that our shadow generator will write to (hint: use the OverlordGame::SetRenderTarget function through SceneManager)
	SceneManager::GetInstance()->GetGame()->SetRenderTarget(m_pShadowRT);

	// Clear this render target
	FLOAT color[4]{ 0.f, 0.f, 0.f, 0.f };
	m_pShadowRT->Clear(gameContext, DirectX::Colors::White);

	// TODO: Set the shader variables of this shadow generator material
	m_pShadowMat->SetLightVP(m_LightVP);
}

void ShadowMapRenderer::End(const GameContext& gameContext) const
{
	UNREFERENCED_PARAMETER(gameContext);

	// Restore default render target (hint: passing nullptr to OverlordGame::SetRenderTarget will do the trick)
	SceneManager::GetInstance()->GetGame()->SetRenderTarget(nullptr); // TOF
}

void ShadowMapRenderer::Draw(const GameContext& gameContext, MeshFilter* pMeshFilter, DirectX::XMFLOAT4X4 world, const std::vector<DirectX::XMFLOAT4X4>& bones) const
{
	m_pShadowMat->SetLightVP(m_LightVP);
	m_pShadowMat->SetWorld(world);
	
	// Set the correct inputlayout, buffers, topology (some variables are set based on the generation type Skinned or Static)
	ShadowMapMaterial::ShadowGenType type{};
	if (pMeshFilter->m_HasAnimations)
	{
		type = ShadowMapMaterial::ShadowGenType::Skinned;
		m_pShadowMat->SetBones(&bones[0].m[0][0], bones.size());
	}
	else type = ShadowMapMaterial::ShadowGenType::Static;

	const VertexBufferData& vertexBufferData = pMeshFilter->GetVertexBufferData(gameContext, m_pShadowMat->m_InputLayoutIds[type]);
	UINT offset[1]{0};

	ID3D11DeviceContext* pDevCon = gameContext.pDeviceContext;
	pDevCon->IASetVertexBuffers(0, 1, &vertexBufferData.pVertexBuffer, &vertexBufferData.VertexStride, offset);
	pDevCon->IASetIndexBuffer(pMeshFilter->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDevCon->IASetInputLayout(m_pShadowMat->m_pInputLayouts[type]);
	pDevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Invoke draw call
	D3DX11_TECHNIQUE_DESC desc{};
	m_pShadowMat->m_pShadowTechs[type]->GetDesc(&desc);
	for (size_t i{}; i < desc.Passes; ++i)
	{
		m_pShadowMat->m_pShadowTechs[type]->GetPassByIndex(i)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->DrawIndexed(pMeshFilter->m_IndexCount, 0, 0);
	}
}

void ShadowMapRenderer::UpdateMeshFilter(const GameContext& gameContext, MeshFilter* pMeshFilter)
{
	// Based on the type (Skinned or Static) build the correct vertex buffers for the MeshFilter (Hint use MeshFilter::BuildVertexBuffer)
	
	ShadowMapMaterial::ShadowGenType type{};
	if (pMeshFilter->m_HasAnimations) type = ShadowMapMaterial::ShadowGenType::Skinned;
	else type = ShadowMapMaterial::ShadowGenType::Static;

	pMeshFilter->BuildVertexBuffer(
		gameContext, 
		m_pShadowMat->m_InputLayoutIds[type], 
		m_pShadowMat->m_InputLayoutSizes[type], 
		m_pShadowMat->m_InputLayoutDescriptions[type]);
}

ID3D11ShaderResourceView* ShadowMapRenderer::GetShadowMap() const
{
	// Return the depth shader resource view of the shadow generator render target
	return m_pShadowRT->GetDepthShaderResourceView();
}
