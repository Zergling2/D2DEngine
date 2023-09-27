#pragma once
#include "Headers\Essential.h"

namespace D2DEngine
{
	// Be sure to call the Initialize function before using it.
	class Time
	{
		friend class Engine;
	private:
		Time() = delete;
	private:
		static long long s_prevTime;
		static long long s_currTime;
		static float s_secondsPerCount;
		static float s_deltaTime;
		static float s_timeScale;
		static bool s_stopped;
		static long long s_baseTime;
		static long long s_pausedTime;
		static long long s_stopTime;
		static void Tick();
	public:
		bool static Initialize();
		static inline float GetFixedDeltaTime() { return FIXED_DELTA_TIME; }
		static inline float GetRealDeltaTime() { return Time::s_deltaTime; }
		static inline float GetDeltaTime() { return Time::s_deltaTime * Time::s_timeScale; }
		static inline void SetTimeScale(float scale) { if (scale >= 0.0f) Time::s_timeScale = scale; }
		static inline float GetTimeScale() { return Time::s_timeScale; }
		static void Stop();
		static void Start();

		// Returns the time elapsed since Time::Initialize
		// was called not counting paused time.
		static float GetTotalTime();
	};

}
