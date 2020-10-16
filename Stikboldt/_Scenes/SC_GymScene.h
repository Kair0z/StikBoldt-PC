#pragma once
#include <GameScene.h>
#include <vector>
#include "../_MyHelpers/TimeManip.h"

class PF_GymEnviroment;
class PF_GameMenu;
class PF_Endscreen;
class PF_RobotCharacter;
class PF_MultipleTargetCamera;
class PF_TheBall;
class SpriteComponent;
class PF_GymUI;
class SpriteFont;
class PostVignette;

using namespace DirectX;

class SC_GymScene final : public GameScene
{
	enum class Input_GymScene
	{
		Reset = 0,
		Pause = 1
	};

public:
	SC_GymScene();
	virtual ~SC_GymScene() = default;

private: // Typedefs
	using Spawn = XMFLOAT3;

	virtual void SceneDeactivated() override;

protected:
	virtual void Initialize() override;
	void PostInit();
	bool m_HasPostInit;

	virtual void Update() override;
	virtual void Draw() override;

	PF_Endscreen* m_pEndscreen;
	PF_GymEnviroment* m_pEnviroment;
	PF_GameMenu* m_pGameMenu;
	PF_GymUI* m_pUI;
	TimeManip m_TimeManipulator; // Just alters GameContext's elapsedseconds with a timescale

	std::vector<PF_RobotCharacter*> m_pPlayers;
	std::vector<Spawn> m_Spawns;

	PF_MultipleTargetCamera* m_pMainCamera;

	PostVignette* m_pVignette;

	PF_TheBall* m_pTheBall;
	Spawn m_BallSpawn;

	SpriteComponent* m_pBindings;

	void InitPlayers();
	void InitCamera();
	void InitEnviroment();
	void InitTheBall();
	void InitUI();
	void InitTimeManip();
	void InitInput();
	void InitPostProcess();
	void InitGameMenu();
	void InitEndscreen();

	void Reset();
	PF_RobotCharacter* GetSoleSurvivor() const;
	
public:
	SC_GymScene(const SC_GymScene&) = delete;
	SC_GymScene(SC_GymScene&&) = delete;
	SC_GymScene& operator=(const SC_GymScene&) = delete;
	SC_GymScene& operator=(SC_GymScene&&) = delete;
};

