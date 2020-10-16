#pragma once
#include <GameScene.h>

class PF_OrthographicCamera;
class PF_PlayerPawn;

class SC_StartScreen : public GameScene
{
	enum class StartscreenInput
	{
		Start
	};

public:
	SC_StartScreen();
	virtual ~SC_StartScreen() = default;

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Draw() override;

private:
	PF_OrthographicCamera* m_pMainCamera;
	GameObject* m_pTitle;

	void InitCamera();
	void InitStartScreen();
	void InitInput();

	void ProcessInput();
};

