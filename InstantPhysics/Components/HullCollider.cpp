#include "HullCollider.h"

using namespace ip;

HullCollider::HullCollider(const math::Vector2 convexShapeVerticesCCW[], uint16_t vertexCount)
    : Collider(ColliderType::HULL)
    , m_vertexCount(vertexCount)
{
    m_pLocalVertices = new math::Vector2[m_vertexCount * 2];
    m_pWorldVertices = m_pLocalVertices + m_vertexCount;

    math::Vector2 center(real(0.0), real(0.0));

    // Compute local coordinate average
    for (uint16_t index = 0; index < m_vertexCount; index++)
    {
        m_pLocalVertices[index] = convexShapeVerticesCCW[index];
        center += m_pLocalVertices[index];
    }

    center /= static_cast<real>(m_vertexCount);

    for (uint16_t index = 0; index < m_vertexCount; index++)
    {
        m_pLocalVertices[index] -= center;
    }

    // ���� Collider m_position�� ���� �߽��� ��Ÿ����.
}

const math::Vector2 HullCollider::GetFaceNormal(uint16_t faceIndex) const
{
    math::Vector2 face = m_pWorldVertices[GetNextFaceIndex(faceIndex)] - m_pWorldVertices[faceIndex];
    return math::Vector2::Normalized(math::Vector2(face.y, -face.x));       // Right normal
}

uint16_t HullCollider::SupportIndex(const math::Vector2 direction) const
{
    uint16_t bestVertexIndex = 0;
    real bestDistance = math::Vector2::Dot(m_pWorldVertices[0], direction);

    for (uint16_t index = 1; index < m_vertexCount; index++)
    {
        real d = math::Vector2::Dot(m_pWorldVertices[index], direction);

        if (d > bestDistance)
        {
            bestDistance = d;
            bestVertexIndex = index;
        }
    }

    return bestVertexIndex;
}

const math::Vector2 HullCollider::SupportPoint(const math::Vector2& direction) const
{
    return m_pWorldVertices[SupportIndex(direction)];
}

void ip::HullCollider::ComputeAABB()
{
    // SIMD code

    // 4�� �������� ���� �������� ��ȿ�� �� �� �ϳ� ��� ä������ ��

    // 4�� �ѹ��� ó�� �� �ִ� �ּ� ���ϱ�
}
