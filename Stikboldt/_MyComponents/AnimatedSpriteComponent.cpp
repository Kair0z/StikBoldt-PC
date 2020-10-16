#include "stdafx.h"
#include "AnimatedSpriteComponent.h"

#include "ContentManager.h"
#include "SpriteRenderer.h"
#include "TextureData.h"

#include <fstream>

AnimatedSpriteComponent::AnimatedSpriteComponent(std::wstring spriteMap, DirectX::XMFLOAT2 pivot, DirectX::XMFLOAT4 color, float depth, float frameInterval)
	: m_SpriteMap{spriteMap}
	, m_Pivot{pivot}
	, m_Color{color}
	, m_Animation{}
	, m_Depth{depth}
{
	m_Animation.m_FrameInterval = frameInterval;
}

void AnimatedSpriteComponent::Initialize(const GameContext&)
{
	LoadSpriteMap(m_SpriteMap);
}

void AnimatedSpriteComponent::Update(const GameContext& gameContext)
{
	if (!m_Animation.m_IsRunning) return;

	m_Animation.m_AccumTime += gameContext.pGameTime->GetElapsed();
	if (m_Animation.m_AccumTime > m_Animation.m_FrameInterval)
	{
		++m_Animation.m_CurrentFrame;
		m_Animation.m_AccumTime = 0.f;

		if (m_Animation.m_CurrentFrame >= m_Animation.m_TotalFrames) m_Animation.m_CurrentFrame = 0;
	}
}

void AnimatedSpriteComponent::Draw(const GameContext&)
{
	using namespace DirectX;
	if (m_pFrames.empty())
		return;

	TextureData* pFrame = m_pFrames[m_Animation.m_CurrentFrame];

	SpriteRenderer::GetInstance()->Draw(pFrame, XMFLOAT2{ 0.f, 0.f }, m_Color, m_Pivot, XMFLOAT2{ 1.f, 1.f }, 0.f, m_Depth);
}

void AnimatedSpriteComponent::LoadSpriteMap(const std::wstring& filepath)
{
	bool fail{ false };
	size_t counter{ 0 };

	// All frames should be named 0-... .png
	// else, fail...

	while (!fail)
	{
		std::wstring finalpath = filepath + L"/" + std::to_wstring(counter) + L".png";
		std::ifstream ifs{};
		ifs.open(finalpath);
		fail = !ifs.is_open();

		if (!fail)
		{
			m_pFrames.push_back(ContentManager::Load<TextureData>(finalpath));
			++counter;
		}
	}

	m_MultipleFrames = (m_pFrames.size() != 1) && (!m_pFrames.empty());

	m_Animation.m_CurrentFrame = 0;
	m_Animation.m_TotalFrames = m_pFrames.size();
}

void AnimatedSpriteComponent::SetPause(bool pause)
{
	m_Animation.m_IsRunning = !pause;
}

bool AnimatedSpriteComponent::IsPaused() const
{
	return !m_Animation.m_IsRunning;
}

void AnimatedSpriteComponent::SetFrameInterval(float interval)
{
	m_Animation.m_FrameInterval = interval;
}

float AnimatedSpriteComponent::GetFrameInterval() const
{
	return m_Animation.m_FrameInterval;
}
