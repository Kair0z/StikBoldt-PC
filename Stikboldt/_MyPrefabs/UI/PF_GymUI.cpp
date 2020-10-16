#include "stdafx.h"
#include "PF_GymUI.h"

#include "GameObject.h"
#include "SpriteComponent.h"
#include "SpriteFont.h"

#include "ContentManager.h"
#include "TextRenderer.h"
#include "../../_MyHelpers/MyUtilities.h"

PF_GymUI::PF_GymUI()
	: m_pGameInfo{nullptr}
	, m_pMainFont{nullptr}
	, m_GameInfoData{}
{
}

void PF_GymUI::UpdateBallSpeed(float ballSpeed)
{
	m_GameInfoData.m_BallSpeedRunUp = m_GameInfoData.m_BallSpeed; // Set runup to previous value
	m_GameInfoData.m_BallSpeed = ballSpeed;
}

void PF_GymUI::Initialize(const GameContext& context)
{
	m_pMainFont = ContentManager::Load<SpriteFont>(L"./Resources/Fonts/Undertale_32.fnt");
	InitGameInfo(context);
}

void PF_GymUI::InitGameInfo(const GameContext&)
{
	m_pGameInfo = new GameObject{};

	SpriteComponent* pSprite = new SpriteComponent{ L"Resources/Textures/UI/GameInfo.png", {}};
	m_pGameInfo->AddComponent(pSprite);

	AddChild(m_pGameInfo);
}

void PF_GymUI::Update(const GameContext& context) 
{
	float difference = m_GameInfoData.m_BallSpeed - m_GameInfoData.m_BallSpeedRunUp;
	if (difference < 0.1f) m_GameInfoData.m_BallSpeedRunUp = m_GameInfoData.m_BallSpeed;

	float increment = (context.pGameTime->GetElapsed() * difference) / m_GameInfoData.m_BallCounterTime;

	m_GameInfoData.m_BallSpeedRunUp += increment;
}

void PF_GymUI::PostDraw(const GameContext&)
{
	if (m_pMainFont->GetFontName() != L"")
	{
		// Render ballspeed text:
		XMFLOAT4 color = m_GameInfoData.m_NeutralColor;
		if (m_GameInfoData.m_BallSpeedRunUp > m_GameInfoData.m_ThresholdWarning) color = m_GameInfoData.m_WarningColor;
		if (m_GameInfoData.m_BallSpeedRunUp > m_GameInfoData.m_ThresholdExtreme) color = m_GameInfoData.m_ExtremeColor;

		TextRenderer::GetInstance()->DrawText(m_pMainFont, std::to_wstring((int)m_GameInfoData.m_BallSpeedRunUp), DirectX::XMFLOAT2(575, 645), color);
	}
}
