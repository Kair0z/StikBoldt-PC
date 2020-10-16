#include "stdafx.h"
#include "SC_GymScene.h"

#include "../_MyPrefabs/MyPrefabs.h"

#include "PhysxProxy.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "RigidBodyComponent.h"
#include "../_Materials/Materials.h"

#include "SpriteFont.h"
#include "TextRenderer.h"
#include "ContentManager.h"

SC_GymScene::SC_GymScene()
	: GameScene(L"Gym Scene")
	, m_pEnviroment{nullptr}
	, m_pUI{nullptr}
	, m_pMainCamera{nullptr}
	, m_pTheBall{nullptr}
	, m_BallSpawn{0.f, 2.f, 0.f}
	, m_pBindings{nullptr}
	, m_TimeManipulator{}
	, m_HasPostInit{false}
	, m_pGameMenu{nullptr}
	, m_pEndscreen{nullptr}
{
	m_Spawns =
	{
		{20.f, 5.f, 0.f},
		{-20.f, 5.f, 0.f},
		{0.f, 5.f, 20.f},
		{0.f, 5.f, -20.f}
	};

	m_pPlayers = { nullptr, nullptr, nullptr, nullptr };
}



void SC_GymScene::SceneDeactivated()
{

}

void SC_GymScene::Initialize()
{
	//GetPhysxProxy()->EnablePhysxDebugRendering(true);
	ActivateMaterials(GetGameContext());

	InitEnviroment();
	InitPlayers();
	InitUI();
	InitTheBall();
	InitTimeManip();
	InitInput();
	InitPostProcess();
	InitGameMenu();
	InitEndscreen();

	// LAST!
	InitCamera();

	m_TimeManipulator.SetTimeScale(1.f);
}

#pragma region Init
void SC_GymScene::InitTheBall()
{
	m_pTheBall = new PF_TheBall{1.f};

	m_pTheBall->GetTransform()->Translate(m_BallSpawn);

	AddChild(m_pTheBall);
}

void SC_GymScene::InitPlayers()
{
	InputManager* pInput = GetGameContext().pInput;
	for (size_t i{}; i < 4; ++i)
	{
		if (pInput->IsGamepadConnected(GamepadIndex(i)))
		{
			m_pPlayers[i] = new PF_RobotCharacter{};
			m_pPlayers[i]->GetTransform()->Translate(m_Spawns[i]);
			m_pPlayers[i]->SetPlayerID((PF_RobotCharacter::PlayerID)i);
			AddChild(m_pPlayers[i]);
		}
	}
}

void SC_GymScene::InitCamera()
{
	m_pMainCamera = new PF_MultipleTargetCamera{};
	m_pMainCamera->SetMaxSpeed(50.f);
	m_pMainCamera->SetOffset({ 0.f, 100.f, -80.f });
	m_pMainCamera->SetSmoothTime(0.5f);

	for (PF_RobotCharacter* pRobot : m_pPlayers)
	{
		if (pRobot) m_pMainCamera->AddToWatch(pRobot);
	}

	//auto scoreboard = ScoreboardLocator::Locate();
	//if (scoreboard) m_pMainCamera->AddToWatch(scoreboard);

	m_pMainCamera->GetTransform()->Rotate(50.f, 0.f, 0.f);
	AddChild(m_pMainCamera);

	MainCameraLocator::Provide(m_pMainCamera);
}

void SC_GymScene::InitEnviroment()
{
	m_pEnviroment = new PF_GymEnviroment{};
	AddChild(m_pEnviroment);
}

void SC_GymScene::InitUI()
{
	m_pUI = new PF_GymUI{};
	AddChild(m_pUI);

	UILocator::Provide(m_pUI);
}

void SC_GymScene::InitTimeManip()
{
	TimeManipLocator::Provide(&m_TimeManipulator);
}

void SC_GymScene::InitInput()
{
	const GameContext& context = GetGameContext();

	context.pInput->AddInputAction(InputAction((int)Input_GymScene::Reset, InputTriggerState::Pressed, 'R'));
	context.pInput->AddInputAction(InputAction((int)Input_GymScene::Pause, InputTriggerState::Pressed, -1, -1, XINPUT_GAMEPAD_START, GamepadIndex::PlayerOne));
}

void SC_GymScene::InitPostProcess()
{
	m_pVignette = new PostVignette{};
	m_pVignette->SetIntensity(0.5f);

	AddPostProcessingEffect(m_pVignette);
}

void SC_GymScene::InitGameMenu()
{
	m_pGameMenu = new PF_GameMenu{};
	GameMenuLocator::Provide(m_pGameMenu);
	AddChild(m_pGameMenu);
}

void SC_GymScene::InitEndscreen()
{
	m_pEndscreen = new PF_Endscreen{};
	AddChild(m_pEndscreen);
}

void SC_GymScene::PostInit()
{
	if (m_HasPostInit) return;

	PF_Scoreboard* pScoreboard = ScoreboardLocator::Locate();
	InputManager* pInput = GetGameContext().pInput;

	for (size_t i{}; i < 4; ++i)
	{
		if (!pInput->IsGamepadConnected(GamepadIndex(i))) if (pScoreboard) pScoreboard->RegisterOut(i);
	}

	m_HasPostInit = true;
}
#pragma endregion

void SC_GymScene::Reset()
{
	// The ball:
	m_pTheBall->Reset();
	m_pTheBall->GetTransform()->Translate(m_BallSpawn);

	// Players:
	for (size_t i{}; i < 4; ++i)
	{
		if (!m_pPlayers[i]) continue;

		m_pPlayers[i]->GetTransform()->Translate(m_Spawns[i]);
		m_pPlayers[i]->Reset();
	}

	// Camera:
	m_pMainCamera->SetMaxSpeed(50.f);
	m_pMainCamera->SetSmoothTime(0.5f);
	m_pMainCamera->SetOffset({ 0.f, 100.f, -100.f });
	m_pMainCamera->GetTransform()->Rotate(50.f, 0.f, 0.f);

	// Enviroment:
	if (m_pEnviroment) m_pEnviroment->Reset();

	// Scoreboard:
	PF_Scoreboard* pScoreboard = ScoreboardLocator::Locate();
	InputManager* pInput = GetGameContext().pInput;
	for (size_t i{}; i < 4; ++i)
	{
		if (!pInput->IsGamepadConnected(GamepadIndex(i))) if (pScoreboard) pScoreboard->RegisterOut(i);
	}
}

PF_RobotCharacter* SC_GymScene::GetSoleSurvivor() const
{
	if (m_pPlayers.size() <= 1) return nullptr;

	size_t survCounter = 0;
	PF_RobotCharacter* pSurvivor = nullptr;

	for (PF_RobotCharacter* pRobot : m_pPlayers)
	{
		if (pRobot && !pRobot->IsOut())
		{
			++survCounter;
			pSurvivor = pRobot;
		}
	}

	if (survCounter == 1) return pSurvivor;
	else return nullptr;
}

void SC_GymScene::Update()
{
	PostInit();

	// Toggle Game menu:
	if (GetGameContext().pInput->IsActionTriggered((int)Input_GymScene::Pause))
	{
		m_pGameMenu->Activate(!m_pGameMenu->IsActive());
	}

	// Set camera:
	GetGameContext().pCamera->SetActive();
	if (m_pMainCamera &&  !m_pMainCamera->IsWatched()) m_pMainCamera->WatchMe(true);

	if (m_pGameMenu->IsActive())
	{
		m_pVignette->SetIntensity(0.f);
		return; // Pause if game menu is active:
	}
	else m_pVignette->SetIntensity(0.5f);

	// Hard Reset:
	if (GetGameContext().pInput->IsActionTriggered((int)Input_GymScene::Reset)) Reset();

	// Update time manipulator:
	m_TimeManipulator.Update(GetGameContext());

	// Check if no win
	//		(By Score full?)
	PF_Scoreboard* pScoreboard = ScoreboardLocator::Locate();
	if (pScoreboard && pScoreboard->GetWinner() != nullptr)
	{
		m_pEndscreen->Start(pScoreboard->GetWinner()->GetPlayerID());
	}
	//		(By Sole survivor?)
	if (GetSoleSurvivor() != nullptr) m_pEndscreen->Start(GetSoleSurvivor()->GetPlayerID());
}

void SC_GymScene::Draw()
{
}