//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ShadowMapMaterial.h"
#include "ContentManager.h"

ShadowMapMaterial::~ShadowMapMaterial()
{
	// Make sure you don't have memory leaks and/or resource leaks :) -> Figure out if you need to do something here
	for (int i{}; i < NUM_TYPES; ++i)
	{
		SafeRelease(m_pInputLayouts[i]);
	}
}

void ShadowMapMaterial::Initialize(const GameContext& gameContext)
{
	if (!m_IsInitialized)
	{
		// Initialize the effect, techniques, shader variables, input layouts (hint use EffectHelper::BuildInputLayout), etc.
		std::wstring filepath = L"Resources/Effects/ShadowMapGenerator.fx";
		m_pShadowEffect = ContentManager::Load<ID3DX11Effect>(filepath);
		
		if (!m_pShadowEffect)
		{
			Logger::LogWarning(L"ShadowMapMaterial::Initialize >> Problem loading ShadowMapGenerator effect: " + filepath);
			return;
		}

		m_pShadowTechs[0] = m_pShadowEffect->GetTechniqueByIndex(0);
		EffectHelper::BuildInputLayout(
			gameContext.pDevice,
			m_pShadowTechs[0],
			&m_pInputLayouts[0],
			m_InputLayoutDescriptions[0],
			m_InputLayoutSizes[0],
			m_InputLayoutIds[0]);

		m_pShadowTechs[1] = m_pShadowEffect->GetTechniqueByIndex(1);
		EffectHelper::BuildInputLayout(
			gameContext.pDevice,
			m_pShadowTechs[1],
			&m_pInputLayouts[1],
			m_InputLayoutDescriptions[1],
			m_InputLayoutSizes[1],
			m_InputLayoutIds[1]);

		m_pLightVPMatrixVariable = m_pShadowEffect->GetVariableByName("gLightViewProj")->AsMatrix();
		m_pWorldMatrixVariable = m_pShadowEffect->GetVariableByName("gWorld")->AsMatrix();
		m_pBoneTransforms = m_pShadowEffect->GetVariableByName("gBones")->AsMatrix();

		m_IsInitialized = true;
	}
}

void ShadowMapMaterial::SetLightVP(DirectX::XMFLOAT4X4 lightVP) const
{
	m_pLightVPMatrixVariable->SetMatrix(&lightVP.m[0][0]);
}

void ShadowMapMaterial::SetWorld(DirectX::XMFLOAT4X4 world) const
{
	m_pWorldMatrixVariable->SetMatrix(&world.m[0][0]);
}

void ShadowMapMaterial::SetBones(const float* pData, int count) const
{
	m_pBoneTransforms->SetMatrixArray(pData, 0, count);
}
