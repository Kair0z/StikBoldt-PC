#pragma once
#include <GameScene.h>
#include <vector>
#include "../_MyHelpers/MyUtilities.h"

class PF_OrthographicCamera;
class PF_PlayerPawn;
class SpriteComponent;
class SpriteFont;
using namespace DirectX;

class SC_MainMenu : public GameScene
{
	using Spawn2D = XMFLOAT2;

public:
	SC_MainMenu();
	virtual ~SC_MainMenu() = default;

	void Reset();

private:
	virtual void Initialize() override;
	void PostInit();
	bool m_HasPostInit = false;

	virtual void Update() override;
		void CheckForStart();
		void CheckForQuit();

	virtual void Draw() override;

	PF_OrthographicCamera* m_pMainCamera;
	SpriteComponent* m_pBackground;
	std::vector<PF_PlayerPawn*> m_pPawns;
	std::vector<Spawn2D> m_Spawns;
	SpriteFont* m_pMainFont;

	MyUtils::Circle2D m_EnterGameHitbox;
	MyUtils::Circle2D m_ExitGameHitbox;

	void InitCamera();
	void InitPawns();
	void InitBackground();
};

