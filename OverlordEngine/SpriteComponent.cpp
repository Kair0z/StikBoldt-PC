#include "stdafx.h"

#include "SpriteComponent.h"
 #include <utility>

#include "GameObject.h"
#include "TextureData.h"
#include "ContentManager.h"
#include "SpriteRenderer.h"
#include "TransformComponent.h"
#include "Singleton.h"

SpriteComponent::SpriteComponent(std::wstring spriteAsset, DirectX::XMFLOAT2 pivot, DirectX::XMFLOAT4 color):
	m_pTexture(nullptr),
	m_SpriteAsset(std::move(spriteAsset)),	
	m_Pivot(pivot),
	m_Color(color)
{
}

void SpriteComponent::Initialize(const GameContext& )
{
	m_pTexture = ContentManager::Load<TextureData>(m_SpriteAsset);
}

void SpriteComponent::SetTexture(const std::wstring& spriteAsset)
{
	m_SpriteAsset = spriteAsset;
	m_pTexture = ContentManager::Load<TextureData>(m_SpriteAsset);
}

void SpriteComponent::Update(const GameContext& )
{
}

using namespace DirectX;

void SpriteComponent::Draw(const GameContext& )
{
	if (!m_pTexture)
		return;

	SpriteRenderer::GetInstance()->Draw(m_pTexture, m_Pivot, XMFLOAT4{1.f, 1.f, 1.f, 1.f}, XMFLOAT2{0.f, 0.f}, XMFLOAT2{1.f, 1.f}, 0.f, m_Depth);

	//  Here you need to draw the SpriteComponent using the Draw of the sprite renderer
	// The sprite renderer is a singleton
	// you will need to position, the rotation and the scale
	// You can use the QuaternionToEuler function to help you with the z rotation 
}
