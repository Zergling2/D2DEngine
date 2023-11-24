#pragma once
#include "Common\Type.h"
#include <cmath>
#include <algorithm>
#include <immintrin.h>

namespace ip
{
	namespace math
	{
		constexpr real PI = real(3.14159265358979323846);
		constexpr real PIx2 = real(3.14159265358979323846) * real(2.0);

		struct Point2
		{
		public:
			real x;
			real y;
		};

		struct Size2
		{
		public:
			real width;
			real height;
		};

		inline real RadianToDegree(real radian)
		{
			return radian * (real(180.0) / PI);
		}

		inline real DegreeToRadian(real degree)
		{
			return degree * (PI / real(180.0));
		}

		// Forward declaration for operator overloading functions
		struct Vector2;
		inline bool operator==(const Vector2& v1, const Vector2& v2);
		inline bool operator!=(const Vector2& v1, const Vector2& v2);
		inline const Vector2 operator+(const Vector2& v1, const Vector2& v2);
		inline const Vector2 operator-(const Vector2& v1, const Vector2& v2);
		inline const Vector2 operator*(const Vector2& v1, const Vector2& v2);
		inline const Vector2 operator*(const Vector2& v, real scalar);
		inline const Vector2 operator*(real scalar, const Vector2& v);
		inline const Vector2 operator/(const Vector2& v1, const Vector2& v2);
		inline const Vector2 operator/(const Vector2& v, real scalar);

		struct Vector2
		{
		public:
			static const Vector2 zero;
			static const Vector2 up;
			static const Vector2 down;
			static const Vector2 right;
			static const Vector2 left;
		public:
			static inline const Vector2 Normalized(const Vector2& v)
			{
				real length = std::sqrt(v.x * v.x + v.y * v.y);
				return Vector2(v.x / length, v.y / length);
			}
			static inline real Dot(const Vector2& v1, const Vector2& v2)
			{
				return v1.x * v2.x + v1.y * v2.y;
			}
			static inline real Cross(const Vector2& v1, const Vector2& v2)
			{
				return v1.x * v2.y - v1.y * v2.x;
			}
			static inline const Vector2 Cross(const Vector2& v, real s)
			{
				return Vector2(v.y * s, v.x * -s);
			}
			static inline const Vector2 Cross(real s, const Vector2& v)
			{
				return Vector2(v.y * -s, v.x * s);
			}
			static inline const Vector2 Rotated(const Vector2& v, real angle)		// Radian
			{
				real cosine = std::cos(angle);
				real sine = std::sin(angle);
				return Vector2(v.x * cosine - v.y * sine, v.x * sine + v.y * cosine);
			}
		public:
			inline Vector2()
			{
			}
			inline Vector2(real x, real y) :
				x(x), y(y)
			{
			}
			inline Vector2(const Vector2& v) :
				x(v.x), y(v.y)
			{
			}
			inline Vector2& operator=(const Vector2& v)
			{
				x = v.x;
				y = v.y;
				return *this;
			}
			inline Vector2& operator+=(const Vector2& v)
			{
				x += v.x;
				y += v.y;
				return *this;
			}
			inline Vector2& operator/=(real s)
			{
				x /= s;
				y /= s;
				return *this;
			}
			inline Vector2& operator-=(const Vector2& v)
			{
				x -= v.x;
				y -= v.y;
				return *this;
			}
			inline const Vector2 operator-() const
			{
				return Vector2(-x, -y);
			}
			inline Vector2& Normalize()
			{
				real length = std::sqrt(x * x + y * y);
				x /= length;
				y /= length;
				return *this;
			}
			inline real Length() const
			{
				return std::sqrt(x * x + y * y);
			}
			inline real SquaredLength() const
			{
				return x * x + y * y;
			}
			inline Vector2& Rotate(real radian)
			{
				real cosine = std::cos(radian);
				real sine = std::sin(radian);
				Vector2 old = *this;
				this->x = old.x * cosine + old.y * sine;
				this->y = old.x * -sine + old.y * cosine;
				return *this;
			}
		public:
			real x;
			real y;
		};


		// ==============================================================
		// Non-member functions
		inline bool operator==(const Vector2& v1, const Vector2& v2)
		{
			return v1.x == v2.x && v1.y == v2.y ? true : false;
		}

		inline bool operator!=(const Vector2& v1, const Vector2& v2)
		{
			return v1.x != v2.x || v1.y != v2.y ? true : false;
		}

		inline const Vector2 operator+(const Vector2& v1, const Vector2& v2)
		{
			return Vector2(v1.x + v2.x, v1.y + v2.y);
		}

		inline const Vector2 operator-(const Vector2& v1, const Vector2& v2)
		{
			return Vector2(v1.x - v2.x, v1.y - v2.y);
		}

		inline const Vector2 operator*(const Vector2& v1, const Vector2& v2)
		{
			return Vector2(v1.x * v2.x, v1.y * v2.y);
		}

		inline const Vector2 operator*(const Vector2& v, real scalar)
		{
			return Vector2(v.x * scalar, v.y * scalar);
		}

		inline const Vector2 operator*(real scalar, const Vector2& v)
		{
			return Vector2(scalar * v.x, scalar * v.y);
		}

		inline const Vector2 operator/(const Vector2& v1, const Vector2& v2)
		{
			return Vector2(v1.x / v2.x, v1.y / v2.y);
		}

		inline const Vector2 operator/(const Vector2& v, real scalar)
		{
			return Vector2(v.x / scalar, v.y / scalar);
		}
		// ==============================================================


		// Forward declaration for operator overloading functions
		struct Matrix3x2R;
		inline const Matrix3x2R operator*(const Matrix3x2R& mat1, const Matrix3x2R& mat2);

		struct Matrix3x2R	// Matrix3x2[Real]
		{
		public:
			inline Matrix3x2R()
			{
			}
			inline Matrix3x2R(real m11, real m12, real m21, real m22, real m31, real m32)
				: _11(m11)
				, _12(m12)
				, _21(m21)
				, _22(m22)
				, _31(m31)
				, _32(m32)
			{
			}
			inline Matrix3x2R(const Matrix3x2R& ref)
				: _11(ref._11)
				, _12(ref._12)
				, _21(ref._21)
				, _22(ref._22)
				, _31(ref._31)
				, _32(ref._32)
			{
			}
			static inline const Matrix3x2R Identity()
			{
				return Matrix3x2R(
					real(1.0), real(0.0),
					real(0.0), real(1.0),
					real(0.0), real(0.0)
				);
			}
			static inline const Matrix3x2R Rotation(real degree)
			{
				real radian = DegreeToRadian(degree);
				real cosine = std::cos(radian);
				real sine = std::sin(radian);
				return Matrix3x2R(
					cosine, sine,
					-sine, cosine,
					real(0.0), real(0.0)
				);
			}
			static inline const Matrix3x2R Rotation(real degree, const Vector2& center)
			{
				return Matrix3x2R::Translation(-center.x, -center.y)
					* Matrix3x2R::Rotation(degree)
					* Matrix3x2R::Translation(center.x, center.y);
			}
			static inline const Matrix3x2R Translation(real x, real y)
			{
				return Matrix3x2R(
					real(1.0), real(0.0),
					real(0.0), real(1.0),
					x, y
				);
			}
			static inline const Matrix3x2R Translation(const Vector2& v)
			{
				return Matrix3x2R(
					real(1.0), real(0.0),
					real(0.0), real(1.0),
					v.x, v.y
				);
			}
		public:
			real _11, _12;
			real _21, _22;
			real _31, _32;
		};

		// ==============================================================
		// Non-member functions
		inline const Matrix3x2R operator*(const Matrix3x2R& mat1, const Matrix3x2R& mat2)
		{
			return Matrix3x2R(
				mat1._11 * mat2._11 + mat1._12 * mat2._21,
				mat1._11 * mat2._12 + mat1._12 * mat2._22,
				mat1._21 * mat2._11 + mat1._22 * mat2._21,
				mat1._21 * mat2._12 + mat1._22 * mat2._22,
				mat1._31 * mat2._11 + mat1._32 * mat2._21 + mat2._31,
				mat1._31 * mat2._12 + mat1._32 * mat2._22 + mat2._32
			);
		}
		// ==============================================================

		inline const Vector2 operator*(const Vector2& v, const Matrix3x2R& mat)
		{
			return Vector2(v.x * mat._11 + v.y * mat._21 + mat._31, v.x * mat._12 + v.y * mat._22 + mat._32);
		}
	}
}
