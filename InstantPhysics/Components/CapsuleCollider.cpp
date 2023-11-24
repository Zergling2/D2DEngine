#include "CapsuleCollider.h"
#include "Components\PhysicsObject.h"

using namespace ip;

CapsuleCollider::CapsuleCollider(PhysicsObject* pPhysObj, CollisionLayer layer)
    : Collider(pPhysObj, CAPSULE, layer)
{
}

bool CapsuleCollider::Initialize(real radius, real length)
{
    if (!Collider::Initialize())
        return false;

    m_radius = radius;
    m_halfLength = length * real(0.5);

    return true;
}

uint16_t CapsuleCollider::SupportIndex(const math::Vector2 direction) const
{
    real d1 = math::Vector2::Dot(m_worldVertices[0], direction);
    real d2 = math::Vector2::Dot(m_worldVertices[1], direction);

    if (d1 > d2)
        return uint16_t(0);
    else
        return uint16_t(1);
}

const math::Vector2 CapsuleCollider::SupportPoint(const math::Vector2& direction) const
{
    return m_worldVertices[SupportIndex(direction)];
}

void CapsuleCollider::UpdateCollider()
{
    // Update collider
    m_worldVertices[0].x = m_halfLength;
    m_worldVertices[0].y = real(0.0);
    m_worldVertices[1].x = -m_halfLength;
    m_worldVertices[1].y = real(0.0);

    real cosine = std::cos(m_pPhysObj->m_rotation);
    real sine = std::sin(m_pPhysObj->m_rotation);
    math::Matrix3x2R tMat(
        cosine, sine,
        -sine, cosine,
        m_pPhysObj->m_position.x, m_pPhysObj->m_position.y
    );

    m_worldVertices[0] = m_worldVertices[0] * tMat;
    m_worldVertices[1] = m_worldVertices[1] * tMat;

    // Update AABB
    real tempMinX;
    real tempMaxX;
    real tempMinY;
    real tempMaxY;

    if (m_worldVertices[0].x < m_worldVertices[1].x)
    {
        tempMinX = m_worldVertices[0].x;
        tempMaxX = m_worldVertices[1].x;
    }
    else
    {
        tempMinX = m_worldVertices[1].x;
        tempMaxX = m_worldVertices[0].x;
    }

    if (m_worldVertices[0].y < m_worldVertices[1].y)
    {
        tempMinY = m_worldVertices[0].y;
        tempMaxY = m_worldVertices[1].y;
    }
    else
    {
        tempMinY = m_worldVertices[1].y;
        tempMaxY = m_worldVertices[0].y;
    }

    m_AABB.pMinXNode->value = tempMinX - m_radius;
    m_AABB.pMaxXNode->value = tempMaxX + m_radius;
    m_AABB.minY = tempMinY - m_radius;
    m_AABB.maxY = tempMaxY + m_radius;
}
