#pragma once
#include "../_MyHelpers/ILocator.h"

struct GameContext;

class TimeManip
{
	struct Manipulation
	{
		float m_ManipTime = 0.f;
		float m_ManipTimer = 0.f;
		float m_TimeScale = 1.f;
		bool m_IsActive = false;
		bool m_IsToggle = true;
	};

public:
	TimeManip();
	void Update(const GameContext&);

	float GetSimulatedElapsedSeconds() const;

	void SetTimeScale(const float scale);
	void SetTimeScale(const float scale, float duration);
	void ResetTimeScale();

private:
	float m_SimulatedElapsedSec;

	Manipulation m_Manipulation;
};

class TimeManipLocator : public ILocator<TimeManip>{};