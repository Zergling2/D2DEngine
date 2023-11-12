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

    // 이제 Collider m_position이 질량 중심을 나타낸다.
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

    // 4개 묶음에서 남는 공간들은 유효한 값 중 하나 골라서 채워놔야 함

    // 4개 한번에 처리 및 최대 최소 구하기
}
