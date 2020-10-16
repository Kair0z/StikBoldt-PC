#include "stdafx.h"
#include "TimeManip.h"

#define TIMEMANIP_TIMESCALE_MIN 0.f
#define TIMEMANIP_TIMESCALE_MAX 100000.f

TimeManip::TimeManip()
	: m_Manipulation{}
	, m_SimulatedElapsedSec{}
{

}

void TimeManip::Update(const GameContext& context)
{
	// just updates it's own manipulated copy of Deltatime
	m_SimulatedElapsedSec = context.pGameTime->GetElapsed();
	if (m_Manipulation.m_IsActive) m_SimulatedElapsedSec *= m_Manipulation.m_TimeScale;

	if (m_Manipulation.m_IsActive && !m_Manipulation.m_IsToggle)
	{
		m_Manipulation.m_ManipTimer += context.pGameTime->GetElapsed();
		if (m_Manipulation.m_ManipTimer > m_Manipulation.m_ManipTime)
		{
			m_Manipulation.m_IsActive = false;
			m_Manipulation.m_TimeScale = 1.f;
		}
	}
}

float TimeManip::GetSimulatedElapsedSeconds() const
{
	return m_SimulatedElapsedSec;
}

void TimeManip::SetTimeScale(const float scale)
{
	m_Manipulation.m_TimeScale = scale;

	if (m_Manipulation.m_TimeScale < TIMEMANIP_TIMESCALE_MIN) m_Manipulation.m_TimeScale = TIMEMANIP_TIMESCALE_MIN;
	if (m_Manipulation.m_TimeScale > TIMEMANIP_TIMESCALE_MAX) m_Manipulation.m_TimeScale = TIMEMANIP_TIMESCALE_MAX;

	m_Manipulation.m_IsActive = true;
	m_Manipulation.m_IsToggle = true;
}

void TimeManip::SetTimeScale(const float scale, float duration)
{
	m_Manipulation.m_TimeScale = scale;

	if (m_Manipulation.m_TimeScale < TIMEMANIP_TIMESCALE_MIN) m_Manipulation.m_TimeScale = TIMEMANIP_TIMESCALE_MIN;
	if (m_Manipulation.m_TimeScale > TIMEMANIP_TIMESCALE_MAX) m_Manipulation.m_TimeScale = TIMEMANIP_TIMESCALE_MAX;

	m_Manipulation.m_IsActive = true;
	m_Manipulation.m_IsToggle = false;
	
	m_Manipulation.m_ManipTimer = 0.0f;
	m_Manipulation.m_ManipTime = duration;
}

void TimeManip::ResetTimeScale()
{
	m_Manipulation.m_TimeScale = 1.f;
}
