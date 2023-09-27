#pragma warning(disable:26495)
#include "Time.h"
#include <Windows.h>

using namespace D2DEngine;

long long Time::s_prevTime;
long long Time::s_currTime;
float Time::s_secondsPerCount;
float Time::s_deltaTime;
float Time::s_timeScale;
bool Time::s_stopped;
long long Time::s_baseTime;
long long Time::s_pausedTime;
long long Time::s_stopTime;

bool Time::Initialize()
{
	LARGE_INTEGER freq;

	// Windows XP 이상을 실행하는 시스템에서는 함수가 항상 성공하므로 0을 반환하지 않습니다.
	if (QueryPerformanceFrequency(&freq) == FALSE)
		return false;

	Time::s_secondsPerCount = 1.0f / static_cast<float>(freq.QuadPart);

	LARGE_INTEGER currTime;
	// Windows XP 이상을 실행하는 시스템에서는 함수가 항상 성공하므로 0을 반환하지 않습니다.
	if (QueryPerformanceCounter(&currTime) == FALSE)
		return false;

	Time::s_prevTime = currTime.QuadPart;
	Time::s_currTime = currTime.QuadPart;
	Time::s_deltaTime = 0.0f;
	Time::s_timeScale = 1.0f;
	Time::s_baseTime = currTime.QuadPart;
	Time::s_pausedTime = 0LL;
	Time::s_stopTime = 0LL;

	Time::s_stopped = false;

	return true;
}

void Time::Tick()
{
	if (Time::s_stopped)
	{
		Time::s_deltaTime = 0.0f;
		return;
	}

	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&Time::s_currTime));

	// Calculate delta time
	Time::s_deltaTime = (Time::s_currTime - Time::s_prevTime) 
		* Time::s_secondsPerCount;

	// Prepare for next frame
	Time::s_prevTime = Time::s_currTime;

	// Force nonnegative. The DXSDK's CDXUTTimer mentions that if the
	// processor goes into a power save mode or we get shuffled to another
	// processor, then delta time can be negative.
	if (Time::s_deltaTime < 0.0f)
		Time::s_deltaTime = 0.0f;
}

void Time::Stop()
{
	if (!Time::s_stopped)
	{
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&Time::s_currTime));

		Time::s_stopTime = Time::s_currTime;
		Time::s_stopped = true;
	}
}

void Time::Start()
{
	LARGE_INTEGER startTime;
	QueryPerformanceCounter(&startTime);

	if (Time::s_stopped)
	{
		Time::s_pausedTime += (startTime.QuadPart - Time::s_stopTime);

		Time::s_prevTime = startTime.QuadPart;
		Time::s_stopTime = 0LL;
		Time::s_stopped = false;
	}
}

float Time::GetTotalTime()
{
	if (Time::s_stopped)
		return static_cast<float>(Time::s_stopTime - Time::s_baseTime - Time::s_pausedTime)
			* Time::s_secondsPerCount;
	else
		return static_cast<float>(Time::s_currTime - Time::s_baseTime - Time::s_pausedTime)
			* Time::s_secondsPerCount;
}
