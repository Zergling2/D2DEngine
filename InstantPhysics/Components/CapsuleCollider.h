#pragma once
#include "Components\Collider.h"

namespace ip
{
	class CapsuleCollider : public Collider
	{
	public:
		CapsuleCollider(class PhysicsObject* pPhysObj, CollisionLayer layer);
		bool Initialize(real radius, real length);
		uint16_t SupportIndex(const math::Vector2 direction) const;
		virtual const math::Vector2 SupportPoint(const math::Vector2& direction) const override;	// ���� ��ǥ, ���� ���Ϳ� ���� ����
		virtual void UpdateCollider() override;
	public:
		real m_radius;
		real m_halfLength;
		math::Vector2 m_worldVertices[2];
	};
}
