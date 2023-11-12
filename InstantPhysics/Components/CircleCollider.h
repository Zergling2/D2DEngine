#pragma once
#include "Components\Collider.h"
#include "Common\Math2D.h"

namespace ip
{
	class CircleCollider : public Collider
	{
	public:
		inline CircleCollider(real radius)
			: Collider(ColliderType::CIRCLE)
			, m_radius(radius)
		{
			
		}
		virtual ~CircleCollider() override {}
		// ==========================================================================
		// [Parameter]
		// 1. const math::Vector2& direction: world vector
		// 원의 Support Point는 항상 원의 중심(중요!) (GJK로는 Circle vs Hull => Shallow contact, Deep contact 구분)
		virtual const math::Vector2 SupportPoint(const math::Vector2& direction) const override;	// 월드 좌표, 월드 벡터에 대해 수행
		virtual void ComputeAABB();
	public:
		real m_radius;
	};
}
