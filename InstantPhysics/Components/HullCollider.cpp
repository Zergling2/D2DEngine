#include "HullCollider.h"
#include "Components\PhysicsObject.h"

using namespace ip;

HullCollider::HullCollider(PhysicsObject* pPhysObj, CollisionLayer layer)
    : Collider(pPhysObj, HULL, layer)
    , m_pLocalX(nullptr)
    , m_pLocalY(nullptr)
    , m_pWorldX(nullptr)
    , m_pWorldY(nullptr)
{
}

HullCollider::~HullCollider()
{
    if (m_pLocalX != nullptr)
    {
        free(m_pLocalX);		// m_pWorldVertices는 delete X
        m_pLocalX = nullptr;
        m_pLocalY = nullptr;
        m_pWorldX = nullptr;
        m_pWorldY = nullptr;
    }
}

bool HullCollider::Initialize(const math::Vector2 convexShapeVerticesCCW[], uint16_t vertexCount)
{
    if (!Collider::Initialize())
        return false;

    m_vertexCount = vertexCount;

    uint16_t _Mul4Ceil = uint16_t(m_vertexCount + 4 - 1) & uint16_t(0xFFFC);
    size_t allocSize = sizeof(real) * _Mul4Ceil * 4;

    void* pVertexBuffer = _aligned_malloc(allocSize, sizeof(real) * 4);
    if (!pVertexBuffer)
        return false;

    m_pLocalX = reinterpret_cast<real*>(pVertexBuffer);
    m_pLocalY = m_pLocalX + _Mul4Ceil;
    m_pWorldX = m_pLocalY + _Mul4Ceil;
    m_pWorldY = m_pWorldX + _Mul4Ceil;

    // Padding 0으로 채우기
    for (uint16_t index = m_vertexCount; index < _Mul4Ceil; index++)
    {
        m_pLocalX[index] = real(0.0);
        m_pLocalY[index] = real(0.0);
    }

    for (uint16_t index = 0; index < m_vertexCount; index++)
    {
        m_pLocalX[index] = convexShapeVerticesCCW[index].x;
        m_pLocalY[index] = convexShapeVerticesCCW[index].y;
    }

    // math::Vector2 center(real(0.0), real(0.0));
    // 
    // // Compute local coordinate average
    // for (uint16_t index = 0; index < m_vertexCount; index++)
    // {
    //     m_pLocalX[index] = convexShapeVerticesCCW[index].x;
    //     m_pLocalY[index] = convexShapeVerticesCCW[index].y;
    //     center.x += m_pLocalX[index];
    //     center.y += m_pLocalY[index];
    // }
    // 
    // center /= static_cast<real>(m_vertexCount);
    // 
    // for (uint16_t index = 0; index < m_vertexCount; index++)
    // {
    //     m_pLocalX[index] -= center.x;
    //     m_pLocalY[index] -= center.y;
    // }

    return true;
}

const math::Vector2 HullCollider::GetFaceNormal(uint16_t faceIndex) const
{
    uint16_t nextFaceIndex = GetNextFaceIndex(faceIndex);
    math::Vector2 face(m_pWorldX[nextFaceIndex] - m_pWorldX[faceIndex], m_pWorldY[nextFaceIndex] - m_pWorldY[faceIndex]);
    return math::Vector2::Normalized(math::Vector2(face.y, -face.x));       // Right normal
}

uint16_t HullCollider::SupportIndex(const math::Vector2 direction) const
{
    uint16_t bestVertexIndex = 0;
    real bestDistance = math::Vector2::Dot(math::Vector2(m_pWorldX[0], m_pWorldY[0]), direction);

    for (uint16_t index = 1; index < m_vertexCount; index++)
    {
        real d = math::Vector2::Dot(math::Vector2(m_pWorldX[index], m_pWorldY[index]), direction);

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
    uint16_t supportIndex = SupportIndex(direction);
    return math::Vector2(m_pWorldX[supportIndex], m_pWorldY[supportIndex]);
}

void HullCollider::UpdateCollider()
{
    uint16_t i;

#ifdef DOUBLE_PRECISION
    __m256d m256dcos = _mm256_set1_pd(std::cos(m_pPhysObj->m_rotation));
    __m256d m256dsin = _mm256_set1_pd(std::sin(m_pPhysObj->m_rotation));
    __m256d m256dposX = _mm256_set1_pd(m_pPhysObj->m_position.x);
    __m256d m256dposY = _mm256_set1_pd(m_pPhysObj->m_position.y);
    for (i = 0; i < m_vertexCount; i += 4)
    {
        __m256d x = _mm256_load_pd(m_pLocalX + i);
        __m256d y = _mm256_load_pd(m_pLocalY + i);

        _mm256_store_pd(m_pWorldX + i, _mm256_add_pd(_mm256_sub_pd(_mm256_mul_pd(x, m256dcos), _mm256_mul_pd(y, m256dsin)), m256dposX));
        _mm256_store_pd(m_pWorldY + i, _mm256_add_pd(_mm256_add_pd(_mm256_mul_pd(x, m256dsin), _mm256_mul_pd(y, m256dcos)), m256dposY));
    }
#else
    __m128 m128cos = _mm_set_ps1(std::cos(m_pPhysObj->m_rotation));
    __m128 m128sin = _mm_set_ps1(std::sin(m_pPhysObj->m_rotation));
    __m128 m128posX = _mm_set_ps1(m_pPhysObj->m_position.x);
    __m128 m128posY = _mm_set_ps1(m_pPhysObj->m_position.y);
    for (i = 0; i < m_vertexCount; i += 4)
    {
        __m128 x = _mm_load_ps(m_pLocalX + i);
        __m128 y = _mm_load_ps(m_pLocalY + i);

        _mm_store_ps(m_pWorldX + i, _mm_add_ps(_mm_sub_ps(_mm_mul_ps(x, m128cos), _mm_mul_ps(y, m128sin)), m128posX));
        _mm_store_ps(m_pWorldY + i, _mm_add_ps(_mm_add_ps(_mm_mul_ps(x, m128sin), _mm_mul_ps(y, m128cos)), m128posY));
    }
#endif // DOUBLE_PRECISION

    // Update AABB
    // SIMD로 변경 시..
    // 4개 묶음에서 남는 공간들은 유효한 값 중 하나 골라서 채워놔야 함
    // 4개 한번에 처리 및 최대 최소 구하기

    real tempMinX;
    real tempMaxX;
    real tempMinY;
    real tempMaxY;
    tempMinX = tempMaxX = m_pWorldX[0];
    tempMinY = tempMaxY = m_pWorldY[0];

    for (i = 1; i < m_vertexCount; i++)
    {
        const math::Vector2 vertex(m_pWorldX[i], m_pWorldY[i]);
        if (vertex.x > tempMaxX) tempMaxX = vertex.x;
        if (vertex.x < tempMinX) tempMinX = vertex.x;
        if (vertex.y > tempMaxY) tempMaxY = vertex.y;
        if (vertex.y < tempMinY) tempMinY = vertex.y;
    }

    m_AABB.pMinXNode->value = tempMinX;
    m_AABB.pMaxXNode->value = tempMaxX;
    m_AABB.minY = tempMinY;
    m_AABB.maxY = tempMaxY;
}
