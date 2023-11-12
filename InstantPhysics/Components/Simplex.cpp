#include "Simplex.h"
#include "Components\Collider.h"
#include "Components\Manifold.h"
#include "Common\Parameters.h"

using namespace ip;

Simplex::State Simplex::EvolveSimplex()
{
    // Vector 순서
    // [0] [1] [2]
    //  C   B   A

    size_t vertexCount = m_vertices.size();
    switch (vertexCount)
    {
    case size_t(0):
        // Simplex 최초 단계
        // 아무 검색 방향이나 선정한다.
        // But 수학적으로 AB 벡터를 잡는 것이 유리하다고 하므로... 다음과 같이 설정.
        m_direction = m_pColliderB->m_position - m_pColliderA->m_position;
        break;
    case size_t(1):
        // Simplex 두 번째 단계
        // 충돌 여부를 최대한 빠르게 알아내려면 Simplex가 클수록 좋으므로
        // 최대한 큰 Simplex를 만들기 위해 검색 방향을 완전히 반대 방향으로 설정한다. (Convex Polygon이라고 가정하므로 가능)
        // (이렇게 하면 Minkowski Difference에서 첫 번째 정점과 반대 방향으로 가장 멀리 떨어진 점을 구할 수 있음)
        m_direction = -m_direction;
        break;
    case size_t(2):
    {
        // Simplex의 정점이 2개인 경우 -> Simplex를 삼각형으로 확장하기 위해 다음 검색 방향을 설정.
        // 다음 검색 방향은 원점을 향하는 방향이어야 유리하다.
        // 벡터 cb는 Simplex에서 가장 처음 만들어진 선분
        math::Vector2 cb = m_vertices[1] - m_vertices[0];
        // co 벡터는 정점 C에서 원점을 향하는 벡터.
        math::Vector2 co = -m_vertices[0];
        // cb x co x cb 삼중 외적을 통해 cb 벡터와 수직인 두 벡터 중 원점을 향하는 벡터를 검색 방향으로 설정.
        m_direction = math::Vector2::Cross(math::Vector2::Cross(cb, co), cb);
    }
        break;
    case size_t(3):
    {
        // 정점이 3개이므로 현재 Simplex가 원점을 포함하고 있는지 계산한다.
        // [0] [1] [2]
        //  C   B   A
        math::Vector2 ao = -m_vertices[2];                  // A to the origin
        math::Vector2 ab = m_vertices[1] - m_vertices[2];   // A to B
        math::Vector2 ac = m_vertices[0] - m_vertices[2];   // A to C

        math::Vector2 abPerp = math::Vector2::Cross(math::Vector2::Cross(ac, ab), ab);  // 반드시 외적으로 구해야 함(단순히 R, L 노말로 안됨)
        math::Vector2 acPerp = math::Vector2::Cross(math::Vector2::Cross(ab, ac), ac);  // 반드시 외적으로 구해야 함(단순히 R, L 노말로 안됨)

        if (math::Vector2::Dot(abPerp, ao) > real(0.0))
        {
            // 원점이 선분 ab 바깥 방향에 있음
            // 따라서 c(0번 인덱스)를 제거하고 ab의 바깥 수직 벡터 방향으로 새로운 Support Point를 찾도록 방향을 설정.
            m_vertices.erase(m_vertices.begin() /* + 0 */);
            m_direction = abPerp;                           // C 제거 => [0] = B, [1] = A (다음 Evolve에서는 B => C, A => B)
        }
        else if (math::Vector2::Dot(acPerp, ao) > real(0.0))
        {
            // 원점이 선분 ac 바깥 방향에 있음
            // 따라서 b(1번 인덱스)를 제거하고 ac의 바깥 수직 벡터 방향으로 새로운 Support Point를 찾도록 방향을 설정.
            m_vertices.erase(m_vertices.begin() + 1);       // B 제거 => [0] = C, [1] = A  (다음 Evolve에서는 C => C, A => B)
            m_direction = acPerp;
        }
        else
        {
            // 원점이 선분 ab와 ac 내부에 있으므로 삼각형 내부에 있음이 확정됨.
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

    if (math::Vector2::Dot(m_direction, newVertex) > real(0.0))     // >= 아닌 이유 (내적이 0이면 원점 방향으로 더 가지 못했기 때문에 확장 종료)
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
    if (cross < real(0.0))                              // CW 였던 경우
        std::swap(m_vertices[1], m_vertices[2]);        // CCW로 바꿔준다.
}

void Simplex::FindClosestEdge(math::Vector2* pClosestEdgeNormal, real* pClosestEdgeDistance, uint16_t* pClosestEdgeIndex)
{
    math::Vector2 edgeNormal;
    math::Vector2 closestEdgeNormal;
    real closestEdgeDistance;
    uint16_t closestEdgeIndex;

    // 0번째 노말 벡터
    // Simplex vertex가 CCW 순서이므로
    // 바깥 방향으로 향하는 Right normal 사용 (x', y') = (y, -x)
    closestEdgeNormal = edgeNormal = 
        math::Vector2::Normalized(math::Vector2(m_vertices[1].y - m_vertices[0].y, m_vertices[0].x - m_vertices[1].x));
    closestEdgeDistance = math::Vector2::Dot(m_vertices[0], edgeNormal);      // 원점으로부터 Edge 양끝 점 아무거나, edgeNormal 내적 => 거리
    closestEdgeIndex = 0;

    // 1번째 ~ 노말 벡터
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
