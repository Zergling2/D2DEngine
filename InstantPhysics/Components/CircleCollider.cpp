#include "CircleCollider.h"
#include "Core\Acc\AABB.h"
#include "Components\RigidBody.h"

using namespace ip;

const math::Vector2 CircleCollider::SupportPoint(const math::Vector2& direction) const
{
	return m_position;
}

void CircleCollider::ComputeAABB()
{
}