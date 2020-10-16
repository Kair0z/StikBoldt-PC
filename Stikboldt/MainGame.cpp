
#include "stdafx.h"

#include "MainGame.h"
#include "GeneralStructs.h"
#include "SceneManager.h"
#include "PhysxProxy.h"
#include "DebugRenderer.h"

//#define Testing

#ifdef Testing
	#include "../OverlordProject/_Scenes/SC_Workshop.h"
	#include "../OverlordProject/_Scenes/HardwareSkinningScene.h"
	#include "../OverlordProject/_Scenes/FontTestScene.h"
#else
	#include "../OverlordProject/_Scenes/SC_GymScene.h"
	#include "../OverlordProject/_Scenes/SC_MainMenu.h"
	#include "../OverlordProject/_Scenes/SC_StartScreen.h"
#endif



MainGame::MainGame(void)
{}

MainGame::~MainGame(void)
{}

//Game is preparing
void MainGame::OnGamePreparing(GameSettings& gameSettings)
{
	UNREFERENCED_PARAMETER(gameSettings);
	//Nothing to do atm.
}

void MainGame::Initialize()
{	
#ifdef Testing
	SceneManager::GetInstance()->AddGameScene(new FontTestScene{});
#else
	SceneManager::GetInstance()->AddGameScene(new SC_StartScreen{});
	SceneManager::GetInstance()->AddGameScene(new SC_MainMenu{});
	SceneManager::GetInstance()->AddGameScene(new SC_GymScene{});

	SceneManager::GetInstance()->SetActiveGameScene(L"Start Screen");
#endif

	
}

LRESULT MainGame::WindowProcedureHook(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(message);
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	switch (message)
	{
		case WM_KEYUP:
		{
			if ((lParam & 0x80000000) != 0x80000000)
				return -1;

			//NextScene
			if (wParam == VK_F3)
			{
				SceneManager::GetInstance()->NextScene();
				return 0;
			}
			//PreviousScene
			else if (wParam == VK_F2)
			{
				SceneManager::GetInstance()->PreviousScene();
				return 0;
			}
			else if (wParam == VK_F4)
			{
				DebugRenderer::ToggleDebugRenderer();
				return 0;
			}
			else if (wParam == VK_F6)
			{
				auto activeScene = SceneManager::GetInstance()->GetActiveScene();
				activeScene->GetPhysxProxy()->NextPhysXFrame();
			}
		}
	}

	return -1;
}
