#pragma once
#include "Components\Collider.h"
#include "Common\Math2D.h"

namespace ip
{
	class PolygonCollider : public Collider
	{
	public:
		// x0, y0, x1, y1, x2, y2, x3, y3
		PolygonCollider(const math::Vector2 convexShapeVerticesCCW[], uint32_t vertexCount);
		virtual ~PolygonCollider() override
		{
			if (m_pVertices != nullptr)
			{
				delete[] m_pVertices;		// m_pWorldVertices는 delete X
				m_pVertices = nullptr;
			}
		}
		// ==========================================================================
		// [Parameter]
		// 1. const math::Vector2& direction: world vector
		virtual const math::Vector2 SupportPoint(const math::Vector2& direction) const override;	// 월드 좌표, 월드 벡터에 대해 수행
		virtual void ComputeAABB() override;
	public:
		uint32_t m_vertexCount;
		math::Vector2* m_pVertices;		// CCW
		math::Vector2* m_pWorldVertices;
	};
}
