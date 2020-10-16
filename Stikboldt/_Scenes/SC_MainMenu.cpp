#include "stdafx.h"
#include "SC_MainMenu.h"

#include "../OverlordProject/_MyPrefabs/MyPrefabs.h"
#include "../OverlordProject/_MyComponents/MyComponents.h"

#include "SpriteComponent.h"
#include "TransformComponent.h"
#include "ContentManager.h"
#include "SpriteFont.h"
#include "SceneManager.h"

SC_MainMenu::SC_MainMenu()
	: GameScene(L"Main Menu")
	, m_pMainCamera{nullptr}
	, m_pPawns{nullptr, nullptr, nullptr, nullptr}
	, m_pBackground{nullptr}
	, m_EnterGameHitbox{}
	, m_ExitGameHitbox{}
{
	m_Spawns =
	{
		{0.f, 0.f},
		{0.f, 500.f},
		{1000.f, 0.f},
		{1000.f, 500.f}
	};

	m_EnterGameHitbox.m_Center = { 1280.f / 2.f, 720.f / 2.f };
	m_EnterGameHitbox.m_Radius = 200.f;
	m_ExitGameHitbox.m_Center = { 0.f, 720.f }; // Leftbottom corner
	m_ExitGameHitbox.m_Radius = 200.f;
}

void SC_MainMenu::Reset()
{
	for (size_t i{}; i < m_pPawns.size(); ++i)
	{
		if (!m_pPawns[i]) continue;

		SpriteComponent* pSprite = m_pPawns[i]->GetSprite();
		if (!pSprite) continue;

		m_pPawns[i]->ResetState();

		pSprite->SetPivot(m_Spawns[i]);
	}
}

void SC_MainMenu::Initialize()
{
	InitCamera();
	InitPawns();
	InitBackground();
	m_pMainFont = ContentManager::Load<SpriteFont>(L"./Resources/Fonts/Undertale_32.fnt");
}

void SC_MainMenu::Update()
{
	PostInit();

	GetGameContext().pCamera->SetActive();
	if (m_pMainCamera && !m_pMainCamera->IsWatched()) m_pMainCamera->WatchMe(true);

	CheckForStart();
	CheckForQuit();
}

void SC_MainMenu::CheckForStart()
{
	bool allReady = true;
	int pawnsReady = 0;
	for (PF_PlayerPawn* pPawn : m_pPawns)
	{
		if (pPawn && !pPawn->PlayRequest())
		{
			++pawnsReady;
			allReady = false;
		}
	}

	if (allReady && pawnsReady > 1)
	{
		Reset();
		SceneManager::GetInstance()->SetActiveGameScene(L"Gym Scene");
	}
}

void SC_MainMenu::CheckForQuit()
{
	bool allReady = true;
	for (PF_PlayerPawn* pPawn : m_pPawns)
	{
		if (pPawn && !pPawn->ExitRequest()) allReady = false;
	}

	if (allReady) PostQuitMessage(WM_QUIT);
}

void SC_MainMenu::Draw()
{

}

void SC_MainMenu::InitCamera()
{
	m_pMainCamera = new PF_OrthographicCamera{};

	m_pMainCamera->GetTransform()->Translate(0.f, 0.f, 0.f);

	AddChild(m_pMainCamera);
}

void SC_MainMenu::InitPawns()
{
	InputManager* pInput = GetGameContext().pInput;
	for (size_t i{}; i < 4; ++i)
	{
		if (pInput->IsGamepadConnected(GamepadIndex(i)))
		{
			m_pPawns[i] = new PF_PlayerPawn{ i };
			m_pPawns[i]->SetEnterGameHitbox(m_EnterGameHitbox);
			m_pPawns[i]->SetExitGameHitbox(m_ExitGameHitbox);
			AddChild(m_pPawns[i]);
		}
	}
}

void SC_MainMenu::InitBackground()
{
	GameObject* pGO = new GameObject{};
	m_pBackground = new SpriteComponent{ L"Resources/Textures/UI/MainMenu.png" };
	pGO->AddComponent(m_pBackground);
	AddChild(pGO);
}

void SC_MainMenu::PostInit()
{
	if (m_HasPostInit) return;
	for (size_t i{}; i < m_pPawns.size(); ++i)
	{
		if (!m_pPawns[i]) continue;

		SpriteComponent* pSprite = m_pPawns[i]->GetSprite();
		if (!pSprite) continue;

		pSprite->SetPivot(m_Spawns[i]);
	}

	m_HasPostInit = true;
}
