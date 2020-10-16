#pragma once
#include <GameObject.h>
#include "../../_MyHelpers/ILocator.h"

class SpriteFont;
using namespace DirectX;

class PF_GymUI final : public GameObject
{
	struct GameInfoData
	{
		float m_BallSpeed = 0.0f;
		float m_BallSpeedRunUp = 0.0f;
		float m_BallCounterTime = 6.f; // How long should it take to count up/down to the newValue?

		float m_ThresholdWarning = 80.f;
		float m_ThresholdExtreme = 200.f;
		XMFLOAT4 m_NeutralColor = (XMFLOAT4)Colors::White;
		XMFLOAT4 m_WarningColor = (XMFLOAT4)Colors::Orange;
		XMFLOAT4 m_ExtremeColor = (XMFLOAT4)Colors::Red;
	};

public:
	PF_GymUI();

	void UpdateBallSpeed(float ballSpeed);

private:
	virtual void Initialize(const GameContext&) override;
		void InitGameInfo(const GameContext&);

	virtual void Update(const GameContext&) override;
	virtual void PostDraw(const GameContext&) override;

	GameObject* m_pGameInfo; GameInfoData m_GameInfoData;
	SpriteFont* m_pMainFont;
};

class UILocator : public ILocator<PF_GymUI>{};