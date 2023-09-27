#pragma once

namespace D2DEngine
{
	typedef float Radian;
	typedef float Degree;

	class Mathf
	{
	public:
		static constexpr float PI = 3.1415927F;
	public:
		// C++ 17 �̻��� ������� ���� ��� (std::clamp ������)
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
		static __forceinline float RadianToDegree(Radian radian) { return radian * (180.0F / Mathf::PI); }
		static __forceinline float DegreeToRadian(Degree degree) { return degree * (Mathf::PI / 180.0F); }
	};
}
