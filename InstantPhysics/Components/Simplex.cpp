#include "Simplex.h"
#include "Components\Collider.h"
#include "Components\Manifold.h"
#include "Common\Parameters.h"

using namespace ip;

Simplex::State Simplex::EvolveSimplex()
{
    // Vector ����
    // [0] [1] [2]
    //  C   B   A

    size_t vertexCount = m_vertices.size();
    switch (vertexCount)
    {
    case size_t(0):
        // Simplex ���� �ܰ�
        // �ƹ� �˻� �����̳� �����Ѵ�.
        // But ���������� AB ���͸� ��� ���� �����ϴٰ� �ϹǷ�... ������ ���� ����.
        m_direction = m_pColliderB->m_position - m_pColliderA->m_position;
        break;
    case size_t(1):
        // Simplex �� ��° �ܰ�
        // �浹 ���θ� �ִ��� ������ �˾Ƴ����� Simplex�� Ŭ���� �����Ƿ�
        // �ִ��� ū Simplex�� ����� ���� �˻� ������ ������ �ݴ� �������� �����Ѵ�. (Convex Polygon�̶�� �����ϹǷ� ����)
        // (�̷��� �ϸ� Minkowski Difference���� ù ��° ������ �ݴ� �������� ���� �ָ� ������ ���� ���� �� ����)
        m_direction = -m_direction;
        break;
    case size_t(2):
    {
        // Simplex�� ������ 2���� ��� -> Simplex�� �ﰢ������ Ȯ���ϱ� ���� ���� �˻� ������ ����.
        // ���� �˻� ������ ������ ���ϴ� �����̾�� �����ϴ�.
        // ���� cb�� Simplex���� ���� ó�� ������� ����
        math::Vector2 cb = m_vertices[1] - m_vertices[0];
        // co ���ʹ� ���� C���� ������ ���ϴ� ����.
        math::Vector2 co = -m_vertices[0];
        // cb x co x cb ���� ������ ���� cb ���Ϳ� ������ �� ���� �� ������ ���ϴ� ���͸� �˻� �������� ����.
        m_direction = math::Vector2::Cross(math::Vector2::Cross(cb, co), cb);
    }
        break;
    case size_t(3):
    {
        // ������ 3���̹Ƿ� ���� Simplex�� ������ �����ϰ� �ִ��� ����Ѵ�.
        // [0] [1] [2]
        //  C   B   A
        math::Vector2 ao = -m_vertices[2];                  // A to the origin
        math::Vector2 ab = m_vertices[1] - m_vertices[2];   // A to B
        math::Vector2 ac = m_vertices[0] - m_vertices[2];   // A to C

        math::Vector2 abPerp = math::Vector2::Cross(math::Vector2::Cross(ac, ab), ab);  // �ݵ�� �������� ���ؾ� ��(�ܼ��� R, L �븻�� �ȵ�)
        math::Vector2 acPerp = math::Vector2::Cross(math::Vector2::Cross(ab, ac), ac);  // �ݵ�� �������� ���ؾ� ��(�ܼ��� R, L �븻�� �ȵ�)

        if (math::Vector2::Dot(abPerp, ao) > real(0.0))
        {
            // ������ ���� ab �ٱ� ���⿡ ����
            // ���� c(0�� �ε���)�� �����ϰ� ab�� �ٱ� ���� ���� �������� ���ο� Support Point�� ã���� ������ ����.
            m_vertices.erase(m_vertices.begin() /* + 0 */);
            m_direction = abPerp;                           // C ���� => [0] = B, [1] = A (���� Evolve������ B => C, A => B)
        }
        else if (math::Vector2::Dot(acPerp, ao) > real(0.0))
        {
            // ������ ���� ac �ٱ� ���⿡ ����
            // ���� b(1�� �ε���)�� �����ϰ� ac�� �ٱ� ���� ���� �������� ���ο� Support Point�� ã���� ������ ����.
            m_vertices.erase(m_vertices.begin() + 1);       // B ���� => [0] = C, [1] = A  (���� Evolve������ C => C, A => B)
            m_direction = acPerp;
        }
        else
        {
            // ������ ���� ab�� ac ���ο� �����Ƿ� �ﰢ�� ���ο� ������ Ȯ����.
            return Simplex::State::Intersecting;
        }
    }
        break;
    default:
        *reinterpret_cast<int*>(0x00000000) = 0;
        break;
    }

    math::Vector2 newVertex = m_pColliderA->SupportPoint(m_direction) - m_pColliderB->SupportPoint(-m_direction);
    m_vertices.push_back(newVertex);

    if (math::Vector2::Dot(m_direction, newVertex) > real(0.0))     // >= �ƴ� ���� (������ 0�̸� ���� �������� �� ���� ���߱� ������ Ȯ�� ����)
        return Simplex::State::Evolving;
    else
        return Simplex::State::NotIntersecting;
}

/*
void Simplex::ComputeManifold(Manifold& manifold)
{
    math::Vector2 closestEdgeNormal;			// Normalized vector
    real closestEdgeDistance;
    uint16_t edgeIndex;

    ConvertVerticesToCCW();

    manifold.pReference = const_cast<Collider*>(m_pColliderA);
    manifold.pIncident = const_cast<Collider*>(m_pColliderB);

    for (unsigned int count = 0; count < parameter::EPA_ITERATION_MAX; count++)
    {
        FindClosestEdge(&closestEdgeNormal, &closestEdgeDistance, &edgeIndex);      // edgeIndex i <- vertex[i + 1] - vertex[i]
        math::Vector2 supportPoint = m_pColliderA->SupportPoint(closestEdgeNormal) - m_pColliderB->SupportPoint(-closestEdgeNormal);    // get new support point
        real distance = math::Vector2::Dot(supportPoint, closestEdgeNormal);        // new edge distance

        manifold.collisionNormal = closestEdgeNormal;
        manifold.penetrationDepth = closestEdgeDistance;
        // manifold.contact[0] = ;
        // manifold.contact[1] = ;
        // manifold.contactCount = ;

        if (std::abs(distance - closestEdgeDistance) <= parameter::EPA_EPSILON)
        {
            return;
        }
        else
        {
            size_t offset = static_cast<size_t>(edgeIndex) + 1;
            if (offset >= m_vertices.size())
                offset = 0;
            m_vertices.insert(m_vertices.begin() + offset, supportPoint);
        }
    }
}
*/

void Simplex::ConvertVerticesToCCW()
{
    //      [2]
    //     /   \
    //    /     \
    //   /       \
    // [0]-------[1]
    real cross = math::Vector2::Cross(m_vertices[1] - m_vertices[0], m_vertices[2] - m_vertices[0]);
    if (cross < real(0.0))                              // CW ���� ���
        std::swap(m_vertices[1], m_vertices[2]);        // CCW�� �ٲ��ش�.
}

void Simplex::FindClosestEdge(math::Vector2* pClosestEdgeNormal, real* pClosestEdgeDistance, uint16_t* pClosestEdgeIndex)
{
    math::Vector2 edgeNormal;
    math::Vector2 closestEdgeNormal;
    real closestEdgeDistance;
    uint16_t closestEdgeIndex;

    // 0��° �븻 ����
    // Simplex vertex�� CCW �����̹Ƿ�
    // �ٱ� �������� ���ϴ� Right normal ��� (x', y') = (y, -x)
    closestEdgeNormal = edgeNormal = 
        math::Vector2::Normalized(math::Vector2(m_vertices[1].y - m_vertices[0].y, m_vertices[0].x - m_vertices[1].x));
    closestEdgeDistance = math::Vector2::Dot(m_vertices[0], edgeNormal);      // �������κ��� Edge �糡 �� �ƹ��ų�, edgeNormal ���� => �Ÿ�
    closestEdgeIndex = 0;

    // 1��° ~ �븻 ����
    const size_t vertexCount = m_vertices.size();
    for (uint16_t index = 1; index < vertexCount; index++)
    {
        uint16_t index2 = index + 1;
        if (index2 >= vertexCount)
            index2 = uint16_t(0);

        edgeNormal = math::Vector2::Normalized(math::Vector2(m_vertices[index2].y - m_vertices[index].y, m_vertices[index].x - m_vertices[index2].x));  // right normal (y, -x)
        real distance = math::Vector2::Dot(m_vertices[index], edgeNormal);

        if (distance < closestEdgeDistance)
        {
            closestEdgeNormal = edgeNormal;
            closestEdgeDistance = distance;
            closestEdgeIndex = index;
        }
    }

    *pClosestEdgeNormal = closestEdgeNormal;
    *pClosestEdgeDistance = closestEdgeDistance;
    *pClosestEdgeIndex = closestEdgeIndex;
}
