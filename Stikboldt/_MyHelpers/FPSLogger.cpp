#include "stdafx.h"
#include "FPSLogger.h"

FPSLogger::FPSLogger(const float interval)
	: m_Timer{}
	, m_Interval{interval}	
{

}

void FPSLogger::operator()(const GameContext& context)
{
	m_Timer += context.pGameTime->GetElapsed();

	if (m_Timer > m_Interval)
	{
		Logger::LogFormat(LogLevel::Info, L"FPS: %i", context.pGameTime->GetFPS());
		
		MyUtils::ChangeConsoleTextColor();
		m_Timer = 0.f;
	}
}