#pragma once

namespace D2DEngine
{
	typedef float radian;
	typedef float degree;

	class Mathf
	{
	public:
		static constexpr float PI = 3.1415927F;
	public:
		// C++ 17 이상을 사용하지 않을 경우 (std::clamp 미지원)
		static inline double Clamp(double value, double min, double max)
		{
			const double t = value < min ? min : value;
			return t > max ? max : t;
		}
		static inline float Clamp(float value, float min, float max)
		{
			const float t = value < min ? min : value;
			return t > max ? max : t;
		}
		static int Clamp(int value, int min, int max)
		{
			const int t = value < min ? min : value;
			return t > max ? max : t;
		}
		static __forceinline float RadianToDegree(radian r) { return r * (180.0F / Mathf::PI); }
		static __forceinline float DegreeToRadian(degree d) { return d * (Mathf::PI / 180.0F); }
	};
}
