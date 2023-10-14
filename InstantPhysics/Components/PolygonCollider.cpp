#include "PolygonCollider.h"

using namespace ip;

PolygonCollider::PolygonCollider(const math::Vector2 convexShapeVerticesCCW[], uint32_t vertexCount)
    : Collider(ColliderType::Polygon)
    , m_vertexCount(vertexCount)
{
    m_pVertices = new math::Vector2[m_vertexCount * 2];
    m_pWorldVertices = m_pVertices + m_vertexCount;

    math::Vector2 center(real(0.0), real(0.0));

    // Compute local coordinate average
    for (uint32_t index = 0; index < m_vertexCount; index++)
    {
        m_pVertices[index] = convexShapeVerticesCCW[index];
        center += m_pVertices[index];
    }

    center /= static_cast<real>(m_vertexCount);

    for (uint32_t index = 0; index < m_vertexCount; index++)
    {
        m_pVertices[index] -= center;
    }

    // 이제 Collider m_position이 질량 중심을 나타낸다.
}

const math::Vector2 PolygonCollider::SupportPoint(const math::Vector2& direction) const
{
    uint32_t bestVertexIndex = 0;
    real bestProjection = math::Vector2::Dot(m_pWorldVertices[0], direction);

    for (uint32_t index = 1; index < m_vertexCount; index++)
    {
        float projection = math::Vector2::Dot(m_pWorldVertices[index], direction);

        if (projection > bestProjection)
        {
            bestProjection = projection;
            bestVertexIndex = index;
        }
    }

    return m_pWorldVertices[bestVertexIndex];
}

void ip::PolygonCollider::ComputeAABB()
{
}
