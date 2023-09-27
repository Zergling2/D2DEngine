#pragma once
#include "Components\Collider.h"
#include "Common\Math2D.h"

namespace ip
{
	class CircleCollider : public Collider
	{
	public:
		inline CircleCollider(real centerX, real centerY, real radius)
			: Collider(ColliderType::Circle)
			, m_radius(radius)
		{
		}
		inline CircleCollider(const math::Vector2& m_center, real radius)
			: Collider(ColliderType::Circle)
			, m_radius(radius)
		{
		}
		virtual ~CircleCollider() override {}
		// ==========================================================================
		virtual void CalculateAABB(class AABB* const pAABB, const class RigidBody* const pRigidBody) override {}
		void GetMinMax(const math::Vector2& center, const math::Vector2& projVector, real* pMin, real* pMax) const;
	public:
		real m_radius;
	};
}
