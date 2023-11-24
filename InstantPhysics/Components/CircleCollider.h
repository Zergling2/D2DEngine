#pragma once
#include "Components\Collider.h"

namespace ip
{
	class CircleCollider : public Collider
	{
	public:
		CircleCollider(class PhysicsObject* pPhysObj, CollisionLayer layer);
		bool Initialize(real radius);
		// ==========================================================================
		// [Parameter]
		// 1. const math::Vector2& direction: world vector
		// ���� Support Point�� �׻� ���� �߽�(�߿�!) (GJK�δ� Circle vs Hull => Shallow contact, Deep contact ����)
		virtual const math::Vector2 SupportPoint(const math::Vector2& direction) const override;	// ���� ��ǥ, ���� ���Ϳ� ���� ����
		virtual void UpdateCollider() override;
	public:
		real m_radius;
	};
}
