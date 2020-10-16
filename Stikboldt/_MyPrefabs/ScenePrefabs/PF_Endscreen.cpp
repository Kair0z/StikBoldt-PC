#include "stdafx.h"
#include "PF_Endscreen.h"
#include "SceneManager.h"
#include "../../_Scenes/SC_MainMenu.h"

#include "SpriteComponent.h"

void PF_Endscreen::Start(size_t playerID)
{
	m_pWinnerFrame->SetTexture(L"Resources/Textures/UI/tex_Playerwin_" + std::to_wstring(playerID) + L".png");
	m_pWinnerFrame->SetPivot({ 0.f,0.f });
	m_pWinnerFrame->SetDepth(0.2f);
	m_HasStarted = true;
	m_PlayerID = playerID;
}

bool PF_Endscreen::HasStarted() const
{
	return m_HasStarted;
}

void PF_Endscreen::Initialize(const GameContext&)
{
	m_pWinnerFrame = new SpriteComponent{ L"Resources/Textures/UI/tex_Playerwin_0.png" };
	m_pWinnerFrame->SetPivot({ 0.f, 0.f });
	AddComponent(m_pWinnerFrame);
}

void PF_Endscreen::Update(const GameContext& context)
{
	if (!m_HasStarted)
	{
		m_pWinnerFrame->SetPivot({ -10000.f, -100000.f });
		return;
	}

	m_ToMainMenuTimer -= context.pGameTime->GetElapsed();

	if (m_ToMainMenuTimer <= 0.0f)
	{
		m_pWinnerFrame->SetDepth(2.f);
		SceneManager::GetInstance()->SetActiveGameScene(L"Main Menu");
	}
}

void PF_Endscreen::Draw(const GameContext&)
{
}
