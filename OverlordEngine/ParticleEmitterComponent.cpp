#include "stdafx.h"
#include "ParticleEmitterComponent.h"
 #include <utility>
#include "EffectHelper.h"
#include "ContentManager.h"
#include "TextureDataLoader.h"
#include "Particle.h"
#include "TransformComponent.h"

ParticleEmitterComponent::ParticleEmitterComponent(std::wstring  assetFile, int particleCount):
	m_pVertexBuffer(nullptr),
	m_pEffect(nullptr),
	m_pParticleTexture(nullptr),
	m_pInputLayout(nullptr),
	m_pInputLayoutSize(0),
	m_Settings(ParticleEmitterSettings()),
	m_ParticleCount(particleCount),
	m_ActiveParticles(0),
	m_LastParticleInit(0.0f),
	m_AssetFile(std::move(assetFile))
{
	m_Particles.reserve(m_ParticleCount);
	for (int i{}; i < m_ParticleCount; ++i)
	{
		m_Particles.push_back(new Particle(m_Settings));
	}
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	for (Particle* pParticle : m_Particles)
	{
		delete pParticle;
		pParticle = nullptr;
	}

	m_Particles.clear();

	SafeRelease(m_pInputLayout);
	SafeRelease(m_pVertexBuffer);
}

void ParticleEmitterComponent::Initialize(const GameContext& gameContext)
{
	LoadEffect(gameContext);
	CreateVertexBuffer(gameContext);
	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);
	if (!m_pParticleTexture) Logger::LogWarning(L"ParticleEmitterComponent::Initialize( ) >>  Problem loading Particle texture with filepath: \n" + m_AssetFile);
}

void ParticleEmitterComponent::LoadEffect(const GameContext& gameContext)
{
	std::wstring filepath = L"./Resources/Effects/ParticleRenderer.fx";
	m_pEffect = ContentManager::Load<ID3DX11Effect>(filepath);
	if (!m_pEffect) Logger::LogWarning(L"ParticleEmitterComponent::Initialize( ) >> Problem loading effect from filepath: \n" + filepath);

	m_pWvpVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	m_pTextureVariable = m_pEffect->GetVariableByName("gParticleTexture")->AsShaderResource();
	m_pViewInverseVariable = m_pEffect->GetVariableByName("gViewInverse")->AsMatrix();

	m_pDefaultTechnique = m_pEffect->GetTechniqueByIndex(0);

	if (!m_pWvpVariable->IsValid()) Logger::LogWarning(L"ParticleEmitterComponent::LoadEffect( ) >> Problem loading Effect Variable 'm_pWvPVariable'!");
	if (!m_pTextureVariable->IsValid()) Logger::LogWarning(L"ParticleEmitterComponent::LoadEffect( ) >> Problem loading Effect Variable 'm_pTextureVariable'!");
	if (!m_pViewInverseVariable->IsValid()) Logger::LogWarning(L"ParticleEmitterComponent::LoadEffect( ) >> Problem loading Effect Variable 'm_pViewInverseVariable'!");

	bool res = EffectHelper::BuildInputLayout(gameContext.pDevice, m_pDefaultTechnique, &m_pInputLayout, m_pInputLayoutSize);

	if (!res) Logger::LogWarning(L"ParticleEmitterComponent::LoadEffect( ) >> Problem Building InputLayout!");
}

void ParticleEmitterComponent::CreateVertexBuffer(const GameContext& gameContext)
{
	if (m_pVertexBuffer) SafeRelease(m_pVertexBuffer);

	D3D11_BUFFER_DESC desc{};
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = m_ParticleCount * sizeof(ParticleVertex);
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	
	gameContext.pDevice->CreateBuffer(&desc, 0, &m_pVertexBuffer);
	if (!m_pVertexBuffer) Logger::LogWarning(L"ParticleEmitterComponent::CreateVertexBuffer( ) >> Problem Creating the vertexbuffer! ");
}

void ParticleEmitterComponent::Update(const GameContext& gameContext)
{
	float particleSpawnInterval = ((m_Settings.MinEnergy + m_Settings.MaxEnergy) / 2.f) / m_ParticleCount;

	m_LastParticleInit += gameContext.pGameTime->GetElapsed();

	m_ActiveParticles = 0;

	//BUFFER MAPPING CODE [PARTIAL :)]
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	gameContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	ParticleVertex* pBuffer = (ParticleVertex*) mappedResource.pData;

	for (Particle* pParticle : m_Particles)
	{
		// Update every particle:
		pParticle->Update(gameContext);

		if (pParticle->IsActive())
		{
			pBuffer[m_ActiveParticles] = pParticle->GetVertexInfo();
			++m_ActiveParticles;
		}

		if (m_IsActive)
		{
			if (m_LastParticleInit >= particleSpawnInterval && !pParticle->IsActive())
			{
				pParticle->Init(GetTransform()->GetPosition(), m_Settings.Color);
				pBuffer[m_ActiveParticles] = pParticle->GetVertexInfo();
				++m_ActiveParticles;
				m_LastParticleInit = 0.f;
			}
		}
	}

	gameContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void ParticleEmitterComponent::Draw(const GameContext& )
{
	// Do Nothing
}

void ParticleEmitterComponent::PostDraw(const GameContext& gameContext)
{
	using namespace DirectX;

	// Particles update their position independently so we don't need to transform them again inside the shader --> Only the ViewProj is needed
	m_pWvpVariable->SetMatrix(&gameContext.pCamera->GetViewProjection().m[0][0]);
	m_pViewInverseVariable->SetMatrix(&gameContext.pCamera->GetViewInverse().m[0][0]);
	m_pTextureVariable->SetResource(m_pParticleTexture->GetShaderResourceView());

	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);

	size_t offset = 0;
	size_t stride = sizeof(ParticleVertex);
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	D3DX11_TECHNIQUE_DESC techDesc{};
	m_pDefaultTechnique->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pDefaultTechnique->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->Draw(m_ActiveParticles, 0);
	}
}
