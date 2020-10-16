#include "stdafx.h"
#include "SC_StartScreen.h"

#include "../OverlordProject/_MyPrefabs/MyPrefabs.h"
#include "../OverlordProject/_MyComponents/MyComponents.h"

#include "SpriteComponent.h"

#include "TransformComponent.h"
#include "SceneManager.h"
#include "SoundManager.h"

SC_StartScreen::SC_StartScreen()
	: GameScene(L"Start Screen")
	, m_pMainCamera{nullptr}
	, m_pTitle{nullptr}
{
}

void SC_StartScreen::Initialize()
{
	InitCamera();
	InitStartScreen();
	InitInput();

	FMOD::Sound* pSound{ nullptr };
	FMOD::ChannelGroup* pGroup{ nullptr };
	FMOD::Channel* pChannel{ nullptr };
	SoundManager::GetInstance()->GetSystem()->createChannelGroup("MainTrack", &pGroup);
	SoundManager::GetInstance()->GetSystem()->createSound("Resources/Audio/FunkGroove.mp3", 0, nullptr, &pSound);
	SoundManager::GetInstance()->GetSystem()->playSound(pSound, pGroup, false, &pChannel);
}

void SC_StartScreen::Update()
{
	GetGameContext().pCamera->SetActive();
	if (m_pMainCamera && !m_pMainCamera->IsWatched()) m_pMainCamera->WatchMe(true);

	ProcessInput();
}

void SC_StartScreen::Draw()
{

}

void SC_StartScreen::InitCamera()
{
	m_pMainCamera = new PF_OrthographicCamera{};

	m_pMainCamera->GetTransform()->Translate(0.f, 0.f, 0.f);

	AddChild(m_pMainCamera);
}

void SC_StartScreen::InitStartScreen()
{
	// TITLE:
	m_pTitle = new GameObject{};
	SpriteComponent* pSprite = new SpriteComponent{ L"Resources/Textures/UI/StartTitle.png", {}, {1.f, 1.f, 1.f, 1.f} };
	m_pTitle->AddComponent(pSprite);
	pSprite->SetDepth(0.6f);
	AddChild(m_pTitle);
}

void SC_StartScreen::InitInput()
{
	const GameContext& context = GetGameContext();

	context.pInput->AddInputAction(InputAction((int)StartscreenInput::Start, InputTriggerState::Pressed, -1, -1, XINPUT_GAMEPAD_A));
}

void SC_StartScreen::ProcessInput()
{
	if (GetGameContext().pInput->IsActionTriggered((int)StartscreenInput::Start))
	{
		SceneManager::GetInstance()->SetActiveGameScene(L"Main Menu");
	}
}
