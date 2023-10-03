#pragma once
#include "Components\Collider.h"
#include "Common\Math2D.h"

namespace ip
{
	class BoxCollider : public Collider
	{
	public:
		// x0, y0, x1, y1, x2, y2, x3, y3
		inline BoxCollider(const real pPoints[8])
			: Collider(ColliderType::Box)
			, m_points{{pPoints[0], pPoints[1]}, {pPoints[2], pPoints[3]}, {pPoints[4], pPoints[5]}, {pPoints[6], pPoints[7]}}
		{
		}
		inline BoxCollider(const math::Vector2 pPoints[4])
			: Collider(ColliderType::Box)
			, m_points{pPoints[0], pPoints[1], pPoints[2], pPoints[3]}
		{
		}
		virtual ~BoxCollider() override {}
		// ==========================================================================
		virtual void CalculateAABB(struct AABB* const pAABB, const class RigidBody* const pRigidBody) override {}
		void GetMinMax(const math::Vector2& projVector, real* pMin, real* pMax) const;
	public:
		math::Vector2 m_points[4];
	};
}
