#pragma once
#include "Components\Collider.h"
#include "Common\Math2D.h"

namespace ip
{
	class CircleCollider : public Collider
	{
	public:
		inline CircleCollider(real radius)
			: Collider(ColliderType::Circle)
			, m_radius(radius)
		{
			
		}
		virtual ~CircleCollider() override {}
		// ==========================================================================
		// [Parameter]
		// 1. const math::Vector2& direction: world vector
		virtual const math::Vector2 SupportPoint(const math::Vector2& direction) const override;	// ���� ��ǥ, ���� ���Ϳ� ���� ����
		virtual void ComputeAABB();
	public:
		real m_radius;
	};
}
