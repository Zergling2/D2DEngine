#pragma once

#include "Common\Math2D.h"

namespace ip
{
	class Ray
	{
	public:
		inline Ray(const math::Vector2& origin) : m_isZRay(true), m_origin(origin), m_direction() {}
		inline Ray(const math::Vector2& origin, const math::Vector2 direction) : m_isZRay(false), m_origin(origin), m_direction(direction) {}
	private:
		bool m_isZRay;						// Z-direction ray
		const math::Vector2 m_origin;		// origin of ray
		const math::Vector2 m_direction;	// local vector
	};

	class RaycastHit2D
	{
	public:
		__forceinline class PhysicsObject* GetPhysicsObject() const { return m_pPhysObj; }
	private:
		class PhysicsObject* m_pPhysObj;
	};
}
