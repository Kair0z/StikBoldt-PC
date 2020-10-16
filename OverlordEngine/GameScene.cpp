#include "stdafx.h"
#include "GameScene.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "OverlordGame.h"
#include "Prefabs.h"
#include "Components.h"
#include "DebugRenderer.h"
#include "RenderTarget.h"
#include "SpriteRenderer.h"
#include "TextRenderer.h"
#include "PhysxProxy.h"
#include "SoundManager.h"
#include <algorithm>
#include "PostProcessingMaterial.h"

GameScene::GameScene(std::wstring sceneName):
	m_pChildren(std::vector<GameObject*>()),
	m_GameContext(GameContext()),
	m_IsInitialized(false),
	m_SceneName(std::move(sceneName)),
	m_pDefaultCamera(nullptr),
	m_pActiveCamera(nullptr),
	m_pPhysxProxy(nullptr)
{
}

GameScene::~GameScene()
{
	SafeDelete(m_GameContext.pGameTime);
	SafeDelete(m_GameContext.pInput);
	SafeDelete(m_GameContext.pMaterialManager);
	SafeDelete(m_GameContext.pShadowMapper);

	for (auto pChild : m_pChildren)
	{
		SafeDelete(pChild);
	}
	SafeDelete(m_pPhysxProxy);

	for (PostProcessingMaterial* pPPMat : m_pPostProcessingEffects)
	{
		SafeDelete(pPPMat);
	}
}

void GameScene::AddChild(GameObject* obj)
{
#if _DEBUG
	if (obj->m_pParentScene)
	{
		if (obj->m_pParentScene == this)
			Logger::LogWarning(L"GameScene::AddChild > GameObject is already attached to this GameScene");
		else
			Logger::LogWarning(
				L"GameScene::AddChild > GameObject is already attached to another GameScene. Detach it from it's current scene before attaching it to another one.");

		return;
	}

	if (obj->m_pParentObject)
	{
		Logger::LogWarning(
			L"GameScene::AddChild > GameObject is currently attached to a GameObject. Detach it from it's current parent before attaching it to another one.");
		return;
	}
#endif

	obj->m_pParentScene = this;
	obj->RootInitialize(m_GameContext);
	m_pChildren.push_back(obj);
}

void GameScene::RemoveChild(GameObject* obj, bool deleteObject)
{
	const auto it = find(m_pChildren.begin(), m_pChildren.end(), obj);

#if _DEBUG
	if (it == m_pChildren.end())
	{
		Logger::LogWarning(L"GameScene::RemoveChild > GameObject to remove is not attached to this GameScene!");
		return;
	}
#endif

	m_pChildren.erase(it);
	if (deleteObject)
	{
		delete obj;
		obj = nullptr;
	}
	else
		obj->m_pParentScene = nullptr;
}

void GameScene::RootInitialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	if (m_IsInitialized)
		return;

	//Create DefaultCamera
	auto freeCam = new FreeCamera();
	freeCam->SetRotation(30, 0);
	freeCam->GetTransform()->Translate(0, 50, -80);
	AddChild(freeCam);
	m_pDefaultCamera = freeCam->GetComponent<CameraComponent>();
	m_pActiveCamera = m_pDefaultCamera;
	m_GameContext.pCamera = m_pDefaultCamera;

	//Create GameContext
	m_GameContext.pGameTime = new GameTime();
	m_GameContext.pGameTime->Reset();
	m_GameContext.pGameTime->Stop();

	m_GameContext.pInput = new InputManager();
	InputManager::Initialize();

	m_GameContext.pMaterialManager = new MaterialManager();
	m_GameContext.pShadowMapper = new ShadowMapRenderer();

	m_GameContext.pDevice = pDevice;
	m_GameContext.pDeviceContext = pDeviceContext;

	//Initialize ShadowMapper
	m_GameContext.pShadowMapper->Initialize(m_GameContext);

	// Initialize Physx
	m_pPhysxProxy = new PhysxProxy();
	m_pPhysxProxy->Initialize(this);
	m_GameContext.pPhysxProxy = m_pPhysxProxy;

	//User-Scene Initialize
	Initialize();

	//Root-Scene Initialize
	for (auto pChild : m_pChildren)
	{
		pChild->RootInitialize(m_GameContext);
	}

	for (PostProcessingMaterial* pPPEffect : m_pPostProcessingEffects)
	{
		pPPEffect->Initialize(m_GameContext);
	}

	m_IsInitialized = true;
}

void GameScene::RootUpdate()
{
	m_GameContext.pGameTime->Update();
	m_GameContext.pInput->Update();
	m_GameContext.pCamera = m_pActiveCamera;

	SoundManager::GetInstance()->GetSystem()->update();

	//User-Scene Update
	Update();

	//Root-Scene Update
	for (auto pChild : m_pChildren)
	{
		pChild->RootUpdate(m_GameContext);
	}

	std::sort(m_pPostProcessingEffects.begin(), m_pPostProcessingEffects.end(),
		[](PostProcessingMaterial* p0, PostProcessingMaterial* p1) -> bool
		{
			return p0->GetRenderIndex() < p1->GetRenderIndex();
		});

	m_pPhysxProxy->Update(m_GameContext);
}

void GameScene::RootDraw()
{
	// Object-Scene SHADOW_PASS
	m_GameContext.pShadowMapper->Begin(m_GameContext);
	for (auto pChild : m_pChildren)
	{
		pChild->RootDrawShadowMap(m_GameContext);
	}
	m_GameContext.pShadowMapper->End(m_GameContext);

	//User-Scene Draw
	Draw();

	//Object-Scene Draw
	for (auto pChild : m_pChildren)
	{
		pChild->RootDraw(m_GameContext);
	}

	//Object-Scene Post-Draw
	for (auto pChild : m_pChildren)
	{
		pChild->RootPostDraw(m_GameContext);
	}

	//Draw PhysX
	m_pPhysxProxy->Draw(m_GameContext);

	//Draw Debug Stuff
	DebugRenderer::Draw(m_GameContext);

	// Sprites & text:
	SpriteRenderer::GetInstance()->Draw(m_GameContext);
	TextRenderer::GetInstance()->Draw(m_GameContext);
	
	// Post Processing:
	if (!m_pPostProcessingEffects.empty())
	{
		// GetRender target from OverlordGame
		OverlordGame* pGame = SceneManager::GetInstance()->GetGame();
		RenderTarget* pInitRT = pGame->GetRenderTarget();
		RenderTarget* pPrevRT = pInitRT;

		for (PostProcessingMaterial* pPP : m_pPostProcessingEffects)
		{
			// Get new renderTarget
			RenderTarget* pTempRT = pPP->GetRenderTarget();
			pGame->SetRenderTarget(pTempRT);
			pPP->Draw(m_GameContext, pPrevRT);

			// Set as new previous render target:
			pPrevRT = pTempRT;
		}

		pGame->SetRenderTarget(pInitRT);
		SpriteRenderer::GetInstance()->DrawImmediate(m_GameContext, pPrevRT->GetShaderResourceView(), {});
	}
}

void GameScene::RootSceneActivated()
{
	//Start Timer
	m_GameContext.pGameTime->Start();
	SceneActivated();
}

void GameScene::RootSceneDeactivated()
{
	//Stop Timer
	m_GameContext.pGameTime->Stop();
	SceneDeactivated();
}

void GameScene::RootWindowStateChanged(int state, bool active) const
{
	//TIMER
	if (state == 0)
	{
		if (active)m_GameContext.pGameTime->Start();
		else m_GameContext.pGameTime->Stop();
	}
}

void GameScene::AddPostProcessingEffect(PostProcessingMaterial* pEffect)
{
	// Add effect:
	if (std::find(m_pPostProcessingEffects.cbegin(), m_pPostProcessingEffects.cend(), pEffect) == m_pPostProcessingEffects.cend())
	{
		// if it doesn't exist yet:
		m_pPostProcessingEffects.emplace_back(pEffect);
		pEffect->Initialize(m_GameContext);
	}
}

void GameScene::RemovePostProcessingEffect(PostProcessingMaterial* pEffect)
{
	// Erase & delete Effect from vector:
	auto it = std::find(m_pPostProcessingEffects.cbegin(), m_pPostProcessingEffects.cend(), pEffect);

	if (it != m_pPostProcessingEffects.cend()) m_pPostProcessingEffects.erase(it);
}

void GameScene::SetActiveCamera(CameraComponent* pCameraComponent)
{
	if (m_pActiveCamera != nullptr)
		m_pActiveCamera->m_IsActive = false;

	m_pActiveCamera = (pCameraComponent) ? pCameraComponent : m_pDefaultCamera;
	m_pActiveCamera->m_IsActive = true;
}
