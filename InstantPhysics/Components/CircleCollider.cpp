#include "CircleCollider.h"
#include "Components\PhysicsObject.h"

using namespace ip;

CircleCollider::CircleCollider(PhysicsObject* pPhysObj, CollisionLayer layer)
	: Collider(pPhysObj, CIRCLE, layer)
{
}

bool CircleCollider::Initialize(real radius)
{
	if (!Collider::Initialize())
		return false;

	m_radius = radius;
	return true;
}

const math::Vector2 CircleCollider::SupportPoint(const math::Vector2& direction) const
{
	return m_pPhysObj->m_position;
}

void CircleCollider::UpdateCollider()
{
	// Update AABB
	m_AABB.pMinXNode->value = m_pPhysObj->m_position.x - m_radius;
	m_AABB.pMaxXNode->value = m_pPhysObj->m_position.x + m_radius;
	m_AABB.minY = m_pPhysObj->m_position.y - m_radius;
	m_AABB.maxY = m_pPhysObj->m_position.y + m_radius;
}
