#pragma once
// ****
// Simple Functor struct that prints FPS every given interval
// ****

#include "MyUtilities.h"

struct FPSLogger
{
	FPSLogger(const float interval = 1.f);
	void operator()(const GameContext& context);

private:
	// Data:
	const float m_Interval;
	float m_Timer;
};

