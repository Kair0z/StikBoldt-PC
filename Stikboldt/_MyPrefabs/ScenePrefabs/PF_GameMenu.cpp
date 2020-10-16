#include "stdafx.h"
#include "PF_GameMenu.h"

#include "../OverlordProject/_MyPrefabs/MyPrefabs.h"

#include "SpriteComponent.h"
#include "SceneManager.h"

PF_GameMenu::PF_GameMenu()
	: m_pPawns{nullptr, nullptr, nullptr, nullptr}
{
	m_Spawns =
	{
		{0.f, 0.f},
		{0.f, 500.f},
		{1000.f, 0.f},
		{1000.f, 500.f}
	};

	m_pToMainMenuCollider = new MyUtils::CircleCollider{};
	m_pToMainMenuCollider->m_Shape.m_Center = { 1080.f, 188.f };
	m_pToMainMenuCollider->m_Shape.m_Radius = 130.f;

	m_pRestartCollider = new MyUtils::CircleCollider{};
	m_pRestartCollider->m_Shape.m_Center = {1080.f, 545.f };
	m_pRestartCollider->m_Shape.m_Radius = 130.f;
}

PF_GameMenu::~PF_GameMenu()
{
	delete m_pRestartCollider;
	m_pRestartCollider = nullptr;

	delete m_pToMainMenuCollider;
	m_pToMainMenuCollider = nullptr;
}

bool PF_GameMenu::IsActive() const
{
	return m_IsActive;
}

void PF_GameMenu::Activate(bool active)
{
	m_IsActive = active;

	if (active) Hide(false);
	else Hide(true);
}

void PF_GameMenu::Initialize(const GameContext& context)
{
	InitPawns(context);

	m_pBackground = new SpriteComponent{ L"Resources/Textures/UI/GameMenu.png" };
	m_pBackground->SetDepth(0.55f);
	AddComponent(m_pBackground);
}

void PF_GameMenu::InitPawns(const GameContext& context)
{
	InputManager* pInput = context.pInput;
	for (size_t i{}; i < 4; ++i)
	{
		if (pInput->IsGamepadConnected(GamepadIndex(i)))
		{
			m_pPawns[i] = new PF_PlayerPawn{ i };
			m_pPawns[i]->SetEnterGameHitbox(m_pRestartCollider->m_Shape);
			m_pPawns[i]->SetExitGameHitbox(m_pToMainMenuCollider->m_Shape);
			AddChild(m_pPawns[i]);
		}
	}
}

void PF_GameMenu::PostInit()
{
	if (m_HasPostInit) return;

	Hide(!m_IsActive);
	m_HasPostInit = true;
}

void PF_GameMenu::Update(const GameContext&)
{
	PostInit();

	if (!m_IsActive)
	{
		Hide(true);
		return;
	}

	CheckButtons();
}

void PF_GameMenu::CheckButtons()
{
	// Check for restart:
	bool allRestart = true;
	bool allMainMenu = true;
	for (PF_PlayerPawn* pPawn : m_pPawns)
	{
		if (!pPawn) continue;
		if (!pPawn->IsPinned()) { allRestart = false; allMainMenu = false; } 
		if (!m_pRestartCollider->PointOverlaps(pPawn->GetComponent<SpriteComponent>()->GetPivot())) allRestart = false; 
		if (!m_pToMainMenuCollider->PointOverlaps(pPawn->GetComponent<SpriteComponent>()->GetPivot())) allMainMenu = false;
	}

	if (allRestart)
	{
		Activate(false);
		SceneManager::GetInstance()->SetActiveGameScene(L"Gym Scene");
	}

	if (allMainMenu)
	{
		Activate(false);
		SceneManager::GetInstance()->SetActiveGameScene(L"Main Menu");
	}
}

void PF_GameMenu::Draw(const GameContext&)
{
}

void PF_GameMenu::Hide(bool hide)
{
	for (PF_PlayerPawn* pPawn : m_pPawns)
	{
		if (!pPawn) continue;
		pPawn->ResetState();
	}

	if (!hide)
	{
		ResetPositions();
		m_pBackground->SetPivot({});
	}
	else
	{
		for (PF_PlayerPawn* pPawn : m_pPawns)
		{
			if (!pPawn) continue;
			pPawn->GetSprite()->SetPivot({ -1000.f, -1000.f });
		}
		m_pBackground->SetPivot({-100000.f, -1000000.f});
	}
	
}

void PF_GameMenu::ResetPositions()
{
	for (size_t i{}; i < 4; ++i)
	{
		if (!m_pPawns[i]) continue;
		m_pPawns[i]->GetSprite()->SetPivot(m_Spawns[i]);
	}
}
