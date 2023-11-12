#include "Processor.h"
#include "Common\Parameters.h"
#include "Components\Manifold.h"
#include "Components\Simplex.h"
#include "Components\Query.h"
#include "Core\Acc\AABB.h"
#include "Components\RigidBody.h"
#include "Components\CircleCollider.h"
#include "Components\HullCollider.h"

using namespace ip;

#ifdef _DEBUG
std::list<Processor::ContactPoint> Processor::m_cpList;
#endif // _DEBUG

Simplex Processor::s_simplex[1];
Processor::CDF Processor::cdft[ColliderType::COLLIDER_TYPE_COUNT][ColliderType::COLLIDER_TYPE_COUNT];

//              Circle      Capsule     Hull        Line
// 
// Circle       Cir_Cir     Cir_Cap     Cir_Hul     Cir_Lin
// 
// Capsule      Cap_Cir     Cap_Cap     Cap_Hul     Cap_Lin
// 
// Hull         Hul_Cir     Hul_Cap     Hul_Hul     Hul_Lin
// 
// Line         Lin_Cir     Lin_Cap     Lin_Hul     Lin_Lin

#define _cdft Processor::cdft

#define Cir_Cir Processor::CheckCirclevsCircleCollision
#define Cir_Cap /*Processor::CheckCirclevsCapsuleCollision*/ nullptr
#define Cir_Hul Processor::CheckCirclevsHullCollision
#define Cir_Lin /*Processor::CheckCirclevsLineCollision*/ nullptr
#define Cap_Cir /*Processor::CheckCapsulevsCircleCollision*/ nullptr
#define Cap_Cap /*Processor::CheckCapsulevsCapsuleCollision*/ nullptr
#define Cap_Hul /*Processor::CheckCapsulevsHullCollision*/ nullptr
#define Cap_Lin /*Processor::CheckCapsulevsLineCollision*/ nullptr
#define Hul_Cir Processor::CheckHullvsCircleCollision
#define Hul_Cap /*Processor::CheckHullvsCapsuleCollision*/ nullptr
#define Hul_Hul Processor::CheckHullvsHullCollision
#define Hul_Lin /*Processor::CheckHullvsLineCollision*/ nullptr
#define Lin_Cir /*Processor::CheckLinevsCircleCollision*/ nullptr
#define Lin_Cap /*Processor::CheckLinevsCapsuleCollision*/ nullptr
#define Lin_Hul /*Processor::CheckLinevsHullCollision*/ nullptr
#define Lin_Lin /*Processor::CheckLinevsLineCollision*/ nullptr

Processor::Processor()
{
    _cdft[0][0] = Cir_Cir;  _cdft[0][1] = Cir_Cap;  _cdft[0][2] = Cir_Hul;  _cdft[0][3] = Cir_Lin;
    _cdft[1][0] = Cap_Cir;  _cdft[1][1] = Cap_Cap;  _cdft[1][2] = Cap_Hul;  _cdft[1][3] = Cap_Lin;
    _cdft[2][0] = Hul_Cir;  _cdft[2][1] = Hul_Cap;  _cdft[2][2] = Hul_Hul;  _cdft[2][3] = Hul_Lin;
    _cdft[3][0] = Lin_Cir;  _cdft[3][1] = Lin_Cap;  _cdft[3][2] = Lin_Hul;  _cdft[3][3] = Lin_Lin;
}

Processor::~Processor()
{
}

void Processor::Update()
{
    UpdateRigidBodyTransform();
    CollisionDetection();
}

CircleCollider* Processor::CreateCircleCollider(real radius)
{
    CircleCollider* pNewCollider = new CircleCollider(radius);
    m_colliders.push_back(pNewCollider);

    return pNewCollider;
}

HullCollider* Processor::CreateBoxCollider(math::Size2 size)
{
    math::Vector2 buffer[4];        // LB, RB, RT, LT 순서

    real halfWidth = size.width * real(0.5);
    real halfHeight = size.height * real(0.5);
    buffer[0].x = -halfWidth;
    buffer[0].y = -halfHeight;
    buffer[1].x = +halfWidth;
    buffer[1].y = -halfHeight;
    buffer[2].x = +halfWidth;
    buffer[2].y = +halfHeight;
    buffer[3].x = -halfWidth;
    buffer[3].y = +halfHeight;

    HullCollider* pNewCollider = new HullCollider(buffer, 4);
    m_colliders.push_back(pNewCollider);

    return pNewCollider;
}

HullCollider* Processor::CreateHullCollider(const math::Vector2 convexShapeVerticesCCW[], uint16_t vertexCount)
{
    HullCollider* pNewCollider = new HullCollider(convexShapeVerticesCCW, vertexCount);
    m_colliders.push_back(pNewCollider);
    
    return pNewCollider;
}

bool Processor::RemoveCollider(Collider* pCollider)
{
    bool result = false;

    std::list<Collider*>::iterator end = m_colliders.end();
    std::list<Collider*>::iterator iter = m_colliders.begin();
    while (iter != end)
    {
        if ((*iter) == pCollider)
        {
            Collider* pDel = *iter;
            delete pDel;
            m_colliders.erase(iter);
            result = true;
            break;
        }
    }

    return result;
}

void Processor::UpdateRigidBodyTransform()
{
    std::list<Collider*>::iterator end = m_colliders.end();
    std::list<Collider*>::iterator iter = m_colliders.begin();
    while (iter != end)
    {
        Collider& collider = **iter;
        // Symplectic Euler
        Integrate(*collider.m_pRigidBody);

        switch (collider.m_type)
        {
        case ColliderType::CIRCLE:
        case ColliderType::CAPSULE:
            // 정점 월드변환 필요 X
            break;
        case ColliderType::HULL:
            ApplyWorldTransform(reinterpret_cast<HullCollider&>(collider));
            break;
        default:
#ifdef _DEBUG
            *reinterpret_cast<int*>(0x0) = 0;
#endif // _DEBUG
            break;
        }
        ++iter;
    }
}

void Processor::CollisionDetection()
{
    // BroadPhase();
    // NarrowPhase();
    Manifold m;
    bool result;

    std::list<Collider*>::iterator end = m_colliders.end();
    std::list<Collider*>::iterator i = m_colliders.begin();
    while (i != end)
    {
        std::list<Collider*>::iterator j = i;
        ++j;
        while (j != end)
        {
            do
            {
                const Collider& colliderA = **i;
                const Collider& colliderB = **j;

                if (colliderA.m_pRigidBody->m_isStatic && colliderB.m_pRigidBody->m_isStatic)
                    break;

                result = cdft[colliderA.m_type][colliderB.m_type](m, colliderA, colliderB);

                if (result)
                {
#ifdef _DEBUG
                    for (uint32_t i = 0; i < m.contactCount; i++)
                    {
                        m_cpList.push_back(ContactPoint(m.contact[i].x, m.contact[i].y));
                    }
#endif // DEBUG
                    ResolveCollision(m);
                }
            } while (false);
            ++j;
        }
        ++i;
    }
}

void Processor::ResolveCollision(const Manifold& manifold)
{
    Collider& refCollider = *manifold.pReference;
    Collider& incCollider = *manifold.pIncident;
    RigidBody& refRigidBody = *refCollider.m_pRigidBody;
    RigidBody& incRigidBody = *incCollider.m_pRigidBody;

    // // ===================================================================================
    // math::Vector2 correctionVector = manifold.collisionNormal * manifold.penetrationDepth;
    // if (refRigidBody.m_isStatic)
    // {
    //     manifold.pIncident->Position() += correctionVector;
    // }
    // else if (incRigidBody.m_isStatic)
    // {
    //     manifold.pReference->Position() -= correctionVector;
    // }
    // else
    // {
    //     manifold.pReference->Position() -= correctionVector * 0.5;
    //     manifold.pIncident->Position() += correctionVector * 0.5;
    // }
    // // ===================================================================================

    // math::Vector2 refLinVelSum(real(0.0), real(0.0));
    // real refAngVelSum = real(0.0);
    // math::Vector2 incLinVelSum(real(0.0), real(0.0));
    // real incAngVelSum = real(0.0);
    // 
    // for (uint32_t i = 0; i < manifold.contactCount; i++)
    // {
    //     math::Vector2 vRP = refRigidBody.m_linearVelocity + math::Vector2::Cross(refRigidBody.m_angularVelocity, manifold.contact[i] - refCollider.m_position);
    //     math::Vector2 vIP = incRigidBody.m_linearVelocity + math::Vector2::Cross(incRigidBody.m_angularVelocity, manifold.contact[i] - incCollider.m_position);
    // 
    //     real rvAlongCollisionNormal = math::Vector2::Dot(vIP - vRP, manifold.collisionNormal);
    //     if (rvAlongCollisionNormal > real(0.0))
    //         continue;
    // 
    //     real e = std::min(refRigidBody.m_bounce, incRigidBody.m_bounce);
    //     real j = (-(real(1.0) + e) * rvAlongCollisionNormal) / 
    //         ((refRigidBody.m_invMass + incRigidBody.m_invMass) + )
    // }
    math::Vector2 vRel = incRigidBody.m_linearVelocity - refRigidBody.m_linearVelocity;
    
    real velAlongNormal = math::Vector2::Dot(vRel, manifold.collisionNormal);
    if (velAlongNormal <= real(0.0))
    {
        real e = std::min(refRigidBody.m_bounce, incRigidBody.m_bounce);
        real j = (-(real(1.0) + e) * velAlongNormal) / (refRigidBody.m_invMass + incRigidBody.m_invMass);
    
        math::Vector2 impulse = j * manifold.collisionNormal;
        refRigidBody.m_linearVelocity -= refRigidBody.m_invMass * impulse;
        incRigidBody.m_linearVelocity += incRigidBody.m_invMass * impulse;
    
        // Corrects Floating Point Error
        const real percent = real(0.8);     // usually 20% to 80%
        const real slop = real(0.01);       // usually 0.01 to 0.1
        // math::Vector2 correctionDir = manifold.collisionNormal *
        //     (manifold.penetrationDepth * percent / (refRigidBody.m_invMass + incRigidBody.m_invMass));
        math::Vector2 correctionDir = manifold.collisionNormal *
            (std::max(manifold.penetrationDepth - slop, real(0.0)) * percent / (refRigidBody.m_invMass + incRigidBody.m_invMass));
        manifold.pReference->m_position -= refRigidBody.m_invMass * correctionDir;
        manifold.pIncident->m_position += incRigidBody.m_invMass * correctionDir;
    
        // ======================
        // Friction
        vRel = incRigidBody.m_linearVelocity - refRigidBody.m_linearVelocity;       // 임펄스 적용 후의 상대속도 재계산
    
        // tangent = rVel - (rVel dot normal) dot normal
        math::Vector2 tangent = vRel - (math::Vector2::Dot(vRel, manifold.collisionNormal) * manifold.collisionNormal);
        real tangentLength = tangent.Length();
        if (tangentLength > real(0.0))
            tangent /= tangentLength;
        real jt = -math::Vector2::Dot(vRel, tangent) / (refRigidBody.m_invMass + incRigidBody.m_invMass);
    
        math::Vector2 frictionImpulse;
        if (std::abs(jt) < j * FrictionSolver(refRigidBody.m_staticFriction, incRigidBody.m_staticFriction))
        {
            frictionImpulse = jt * tangent;
        }
        else
        {
            frictionImpulse = -j * tangent * FrictionSolver(refRigidBody.m_dynamicFriction, incRigidBody.m_dynamicFriction);
        }
    
        refRigidBody.m_linearVelocity -= refRigidBody.m_invMass * frictionImpulse;
        incRigidBody.m_linearVelocity += incRigidBody.m_invMass * frictionImpulse;
    }
}

bool Processor::CheckCollisionAABBToAABB(const AABB& a, const AABB& b)
{
    if (a.rightTop.x < b.leftBottom.x || a.rightTop.y < b.leftBottom.y ||
        a.leftBottom.x > b.rightTop.x || a.leftBottom.y > b.rightTop.y)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void Processor::ApplyWorldTransform(HullCollider& hull)
{
    real cosine = std::cos(hull.m_radian);
    real sine = std::sin(hull.m_radian);
    math::Matrix3x2R tMat(
        cosine, sine,
        -sine, cosine,
        hull.m_position.x, hull.m_position.y
    );

    for (uint16_t index = 0; index < hull.m_vertexCount; index++)
    {
        hull.m_pWorldVertices[index] = hull.m_pLocalVertices[index] * tMat;
    }
}

void Processor::Integrate(RigidBody& rigidBody)
{
    if (rigidBody.m_isStatic)
        return;

    const math::Vector2 gravity(0, real(-9.81 * 10));
    // const math::Vector2 gravity(0, real(0));

    rigidBody.m_force += gravity * rigidBody.m_mass;
    rigidBody.m_linearVelocity += rigidBody.m_force * rigidBody.m_invMass * parameter::PHYSICS_UPDATE_INTERVAL;
    rigidBody.m_angularVelocity += rigidBody.m_invInertia * rigidBody.m_torque * parameter::PHYSICS_UPDATE_INTERVAL;
    rigidBody.m_collider.m_position += rigidBody.m_linearVelocity * parameter::PHYSICS_UPDATE_INTERVAL;
    rigidBody.m_collider.Rotate(rigidBody.m_angularVelocity * parameter::PHYSICS_UPDATE_INTERVAL);

    rigidBody.m_force = math::Vector2::zero;
    rigidBody.m_torque = real(0.0);
}

bool Processor::CheckCirclevsCircleCollision(Manifold& manifold, const Collider& circleColliderA, const Collider& circleColliderB)
{
    const CircleCollider& circleA = reinterpret_cast<const CircleCollider&>(circleColliderA);
    const CircleCollider& circleB = reinterpret_cast<const CircleCollider&>(circleColliderB);

    math::Vector2 v = circleB.m_position - circleA.m_position;
    real distancePower2 = v.SquaredLength();
    real radiusSum = circleA.m_radius + circleB.m_radius;
    
    if (distancePower2 > radiusSum * radiusSum)
        return false;

    real distance = std::sqrt(distancePower2);
    v /= distance;      // normalize vector v

    manifold.pReference = const_cast<Collider*>(&circleColliderA);
    manifold.pIncident = const_cast<Collider*>(&circleColliderB);

    if (distance != real(0.0))
    {
        manifold.collisionNormal = v;
        manifold.penetrationDepth = radiusSum - distance;
    }
    else
    {
        manifold.collisionNormal = math::Vector2(real(0.0), real(1.0));       // collision resolution constant vector
        manifold.penetrationDepth = circleA.m_radius;
    }

    manifold.contact[0] = circleA.m_position + (v * (circleA.m_radius - (manifold.penetrationDepth * real(0.5))));
    manifold.contactCount = 1;
    
    return true;
}

void Processor::FaceQueryHullvsCircle(Query& query, const HullCollider& ref, const CircleCollider& inc)
{
    math::Vector2 edge;
    math::Vector2 faceNormal;
    real bestDistance = std::numeric_limits<real>::lowest();
    math::Vector2 bestFaceNormal;

    for (uint16_t faceIndex = 0; faceIndex < ref.m_vertexCount; faceIndex++)
    {
        uint16_t faceIndex2 = faceIndex + 1;
        if (faceIndex2 >= ref.m_vertexCount)
            faceIndex2 = 0;

        edge = ref.m_pWorldVertices[faceIndex2] - ref.m_pWorldVertices[faceIndex];
        faceNormal = math::Vector2::Normalized(math::Vector2(edge.y, -edge.x));        // right normal

        // 원의 중심 - 엣지 한 점까지 내적 + 원 반지름 (SP 거리(음수)) (원의 중심이 무조건 Hull 내부 or 경계에 있으므로 d가 최대 0.0이다.)
        real d = math::Vector2::Dot(inc.m_position - ref.m_pWorldVertices[faceIndex], faceNormal);

        if (d > bestDistance)
        {
            bestDistance = d;
            bestFaceNormal = faceNormal;
        }
    }
    bestDistance -= inc.m_radius;   // d 구할 때마다 안뺐으므로

    // =======================================================================================
    // query.axisExist = false;        // FaceQueryHullvsCircle 호출되는 경우는 무조건 false이다.
    // query.refFaceIndex = ;           // 클리핑 안하므로 불필요
    // query.incSupportIndex = ;        // 클리핑 안하므로 불필요
    query.bestDistance = bestDistance;
    query.refFaceNormal = bestFaceNormal;
}

void Processor::ComputeHullvsHullContactPoint(Manifold& manifold, uint16_t refFaceIndex, uint16_t incVertexIndex)
{
    const HullCollider& ref = reinterpret_cast<const HullCollider&>(*manifold.pReference);
    const HullCollider& inc = reinterpret_cast<const HullCollider&>(*manifold.pIncident);

    // n1, n2 (incVertex를 포함하는 두 면 각각의 법선 벡터)
    
    uint16_t nPrevIndex = inc.GetPrevVertexIndex(incVertexIndex);
    const math::Vector2 nPrev = inc.GetFaceNormal(nPrevIndex);       // vertex index는 face index로도 쓰일 수 있음
    const math::Vector2 nNext = inc.GetFaceNormal(incVertexIndex);

    uint16_t incVertexIndex2;
    // ref의 충돌 법선이랑 내적하기 때문에더 작다는건 그만큼 평행하다는 것
    if (math::Vector2::Dot(nPrev, manifold.collisionNormal) < math::Vector2::Dot(nNext, manifold.collisionNormal))
        incVertexIndex2 = nPrevIndex;
    else
        incVertexIndex2 = inc.GetNextVertexIndex(incVertexIndex);

    // 최초 contact point가정
    math::Vector2 contactPoint[2] = { inc.m_pWorldVertices[incVertexIndex2], inc.m_pWorldVertices[incVertexIndex] };
    uint32_t contactCount = 2;

    // ===============
    // ======
    // 영역 너머 체크 파트
    // CP 제거 시 반대쪽 버텍스 살아있는 1, 2단계(ref면 왼쪽, 오른쪽면의 법선 테스트)의 경우는 클리핑 해야함. (지터 방지 투포인트 컨택 매니폴드 생성)
    // 단, 3단계(ref면 법선 테스트) 클리핑 시에는 반대쪽 버텍스 살아있어도 아예 제거. (=> 원포인트 컨택 매니폴드 완성)
    math::Vector2 invalidAreaDir;         // 클리핑 면 기준으로 이 방향에 있으면 클리핑 대상)

    // ========================
    // 클리핑 계산용 변수들 버퍼
    real ratio;
    math::Vector2 dir;      // 길이 성분도 유효 (ratio 곱해서 사용)
    // ========================

    // 1. ref 충돌 면의 왼쪽(반시계방향) 면부터 검사
    // 면 기준으로 invalidAreaDir 방향 너머에 있으면 클리핑
    uint16_t faceQueryIndex = ref.GetNextFaceIndex(refFaceIndex);
    invalidAreaDir = ref.GetFaceNormal(faceQueryIndex);

    // reference vertex 중 현재 클리핑 면을 이루는 면의 버텍스 둘 중 하나 선택
    // ref face가 바라보는 기준 왼쪽 영역 클리핑이니깐 refFaceIndex + 1 또는 +2번 버텍스 까지도 사용은 가능
    // 여기서는 refFaceIndex + 1번 버텍스 사용

    real d;
    d = math::Vector2::Dot(contactPoint[0] - ref.m_pWorldVertices[faceQueryIndex], invalidAreaDir);
    if (d > real(0.0))
    {
        // cp0이 클리핑 영역 너머에 있음
        // cp0 clipping code
        dir = contactPoint[1] - contactPoint[0];        // cp0 ------> cp1
        // cp1은 살아있다는 소리니깐 (invalidAreaDir 반대 방향에 있음)
        ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
        contactPoint[0] = contactPoint[0] + dir * ratio;            // NEW CONTACT POINT
    }
    else
    {
        d = math::Vector2::Dot(contactPoint[1] - ref.m_pWorldVertices[faceQueryIndex], invalidAreaDir);
        if (d > real(0.0))
        {
            // cp1이 클리핑 영역 너머에 있음
            // cp1 clipping code
            dir = contactPoint[0] - contactPoint[1];        // cp1 ------> cp0
            ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
            contactPoint[1] = contactPoint[1] + dir * ratio;        // NEW CONTACT POINT
        }
    }

    // 2. Clip right area
    faceQueryIndex = ref.GetPrevFaceIndex(refFaceIndex);
    invalidAreaDir = ref.GetFaceNormal(faceQueryIndex);

    d = math::Vector2::Dot(contactPoint[0] - ref.m_pWorldVertices[faceQueryIndex], invalidAreaDir);
    if (d > real(0.0))
    {
        // cp0이 클리핑 영역 너머에 있음
        // cp0 clipping code
        dir = contactPoint[1] - contactPoint[0];        // cp0 ------> cp1
        ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
        contactPoint[0] = contactPoint[0] + dir * ratio;            // NEW CONTACT POINT
    }
    else
    {
        d = math::Vector2::Dot(contactPoint[1] - ref.m_pWorldVertices[faceQueryIndex], invalidAreaDir);
        if (d > real(0.0))
        {
            // cp1이 클리핑 영역 너머에 있음
            // cp1 clipping code
            dir = contactPoint[0] - contactPoint[1];        // cp1 ------> cp0
            ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
            contactPoint[1] = contactPoint[1] + dir * ratio;        // NEW CONTACT POINT
        }
    }

    // 3. Clip collision normal face area
    // 충돌 법선 그대로 사용
    invalidAreaDir = manifold.collisionNormal;

    d = math::Vector2::Dot(contactPoint[0] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
    if (d > real(0.0))
    {
        // cp0이 클리핑 영역 너머에 있음
        // cp0 제거
        contactPoint[0] = contactPoint[1];
        contactCount = 1;
    }
    else
    {
        d = math::Vector2::Dot(contactPoint[1] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
        if (d > real(0.0))
        {
            // cp1이 클리핑 영역 너머에 있음
            // cp1 제거
            // contact[0] = contact[0];
            contactCount = 1;
        }
    }

    manifold.contact[0] = contactPoint[0];
    manifold.contact[1] = contactPoint[1];
    manifold.contactCount = contactCount;
}

bool Processor::CheckHullvsHullCollision(Manifold& manifold, const Collider& hullColliderA, const Collider& hullColliderB)
{
    Query queryA_B;     // HullA->HullB Query
    Query queryB_A;     // HullB->HullA Query

    // -------------------------------------------
    // Test Box->Hull                           //
    FaceQueryHullvsHull(queryA_B, reinterpret_cast<const HullCollider&>(hullColliderA), reinterpret_cast<const HullCollider&>(hullColliderB));
    if (queryA_B.axisExist)                     //
        return false;                           //
    // -------------------------------------------

    // -------------------------------------------
    // Test Hull->Box                           //
    FaceQueryHullvsHull(queryB_A, reinterpret_cast<const HullCollider&>(hullColliderB), reinterpret_cast<const HullCollider&>(hullColliderA));
    if (queryB_A.axisExist)                     //
        return false;                           //
    // -------------------------------------------

    if (queryA_B.bestDistance > queryB_A.bestDistance)
    {
        // ref = boxA
        manifold.pReference = const_cast<Collider*>(&hullColliderA);
        manifold.pIncident = const_cast<Collider*>(&hullColliderB);
        manifold.collisionNormal = queryA_B.refFaceNormal;
        manifold.penetrationDepth = -queryA_B.bestDistance;
        ComputeHullvsHullContactPoint(manifold, queryA_B.refFaceIndex, queryA_B.incSupportIndex);
    }
    else
    {
        // ref = boxB
        manifold.pReference = const_cast<Collider*>(&hullColliderB);
        manifold.pIncident = const_cast<Collider*>(&hullColliderA);
        manifold.collisionNormal = queryB_A.refFaceNormal;
        manifold.penetrationDepth = -queryB_A.bestDistance;
        ComputeHullvsHullContactPoint(manifold, queryB_A.refFaceIndex, queryB_A.incSupportIndex);
    }

    return true;
}

bool Processor::CheckCirclevsHullCollision(Manifold& manifold, const Collider& circleCollider, const Collider& hullCollider)
{
    return CheckHullvsCircleCollision(manifold, hullCollider, circleCollider);
}

const math::Vector2 Processor::GetClosestPointOnLineSegementFromOrigin(const math::Vector2& a, const math::Vector2& b)
{
    real l = (b - a).Length();
    math::Vector2 n = (b - a) / l;
    real v = math::Vector2::Dot(-a, n) / l;
    real u = math::Vector2::Dot(b, n) / l;

    if (u <= real(0.0))
    {
        return b;
    }
    else if (v <= real(0.0))
    {
        return a;
    }
    else
    {
        return math::Vector2(u * a + v * b);
    }
}

void Processor::GetClosestVectorBetweenCircleCenterAndHull(const math::Vector2 circleCenter, const HullCollider& hull, math::Vector2& v, real& dist)
{
    const math::Vector2 dir = circleCenter - hull.m_position;
    const uint16_t hullSupportIndex = hull.SupportIndex(dir);
    const uint16_t prevIndex = hull.GetPrevVertexIndex(hullSupportIndex);
    const uint16_t nextIndex = hull.GetNextVertexIndex(hullSupportIndex);

    math::Vector2 v1 = GetClosestPointOnLineSegementFromOrigin(hull.m_pWorldVertices[hullSupportIndex] - circleCenter, hull.m_pWorldVertices[prevIndex] - circleCenter);
    math::Vector2 v2 = GetClosestPointOnLineSegementFromOrigin(hull.m_pWorldVertices[nextIndex] - circleCenter, hull.m_pWorldVertices[hullSupportIndex] - circleCenter);

    real v1LengthSquared = v1.SquaredLength();
    real v2LengthSquared = v2.SquaredLength();
    if (v1LengthSquared < v2LengthSquared)
    {
        v = v1;
        dist = std::sqrt(v1LengthSquared);
    }
    else
    {
        v = v2;
        dist = std::sqrt(v2LengthSquared);
    }
}

bool Processor::CheckHullvsCircleCollision(Manifold& manifold, const Collider& hullCollider, const Collider& circleCollider)
{
    s_simplex[0].Initialize(&hullCollider, &circleCollider);
    Simplex::State state;
    
    while ((state = s_simplex[0].EvolveSimplex()) == Simplex::State::Evolving);
    
    if (state == Simplex::State::NotIntersecting)
    {
        math::Vector2 cpv;      // closest point vector in the simplex
        real distance;
        GetClosestVectorBetweenCircleCenterAndHull(circleCollider.m_position, reinterpret_cast<const HullCollider&>(hullCollider), cpv, distance);
    
        real r = reinterpret_cast<const CircleCollider&>(circleCollider).m_radius;
    
        if (distance <= r)
        {
            manifold.pReference = const_cast<Collider*>(&hullCollider);
            manifold.pIncident = const_cast<Collider*>(&circleCollider);
            manifold.collisionNormal = -cpv / distance;
            manifold.penetrationDepth = r - distance;
    
            // CirclevsHull 충돌은 클리핑 필요 없으므로 Contact point 바로 계산
            manifold.contact[0] = circleCollider.m_position + cpv;
            manifold.contactCount = 1;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        // state == Simplex::State::Intersecting
    
        Query query;
        FaceQueryHullvsCircle(query, reinterpret_cast<const HullCollider&>(hullCollider), reinterpret_cast<const CircleCollider&>(circleCollider));
    
        manifold.pReference = const_cast<Collider*>(&hullCollider);
        manifold.pIncident = const_cast<Collider*>(&circleCollider);
        manifold.collisionNormal = query.refFaceNormal;
        manifold.penetrationDepth = -query.bestDistance;
    
        // CirclevsHull 충돌은 클리핑 필요 없으므로 Contact point 바로 계산
        manifold.contact[0] = circleCollider.m_position - (query.refFaceNormal * (query.bestDistance + reinterpret_cast<const CircleCollider&>(circleCollider).m_radius));   // 반대 방향으로 가야하므로 빼야함
        manifold.contactCount = 1;
    
        return true;
    }

    return false;
}

void Processor::FaceQueryHullvsHull(Query& query, const HullCollider& ref, const HullCollider& inc)
{
    math::Vector2 bestFaceNormal;
    uint16_t bestFaceIndex;                    // face [i] = vertex[(i + 1) % 4] - vertex[i]
    uint16_t incBestSupportIndex;

    real bestDistance = std::numeric_limits<real>::lowest();

    uint16_t hullVertexCount = ref.m_vertexCount;
    for (uint16_t i = 0; i < hullVertexCount; i++)
    {
        math::Vector2 refNormal = ref.GetFaceNormal(i);
        uint16_t incSupportIndex = inc.SupportIndex(-refNormal);
        math::Vector2 incSupport = inc.m_pWorldVertices[incSupportIndex];

        real d = math::Vector2::Dot(incSupport - ref.m_pWorldVertices[i], refNormal);
        if (d > real(0.0))
        {
            query.axisExist = true;
            return;
        }

        if (d > bestDistance)
        {
            bestFaceNormal = refNormal;
            bestDistance = d;
            bestFaceIndex = i;
            incBestSupportIndex = incSupportIndex;
        }
    }

    query.axisExist = false;
    query.refFaceIndex = bestFaceIndex;
    query.incSupportIndex = incBestSupportIndex;
    query.bestDistance = bestDistance;
    query.refFaceNormal = bestFaceNormal;
}

/*
bool Processor::CheckCirclevsBoxCollision(Manifold& manifold, const Collider& circleCollider, const Collider& boxCollider)
{
    return CheckBoxvsCircleCollision(manifold, boxCollider, circleCollider);
}

bool Processor::CheckBoxvsCircleCollision(Manifold& manifold, const Collider& boxCollider, const Collider& circleCollider)
{
    const BoxCollider& box = reinterpret_cast<const BoxCollider&>(boxCollider);
    const CircleCollider& circle = reinterpret_cast<const CircleCollider&>(circleCollider);

    // 원의 중심이 박스 내에 있는지 외부에 있는지 검사

    math::Vector2 boxNormal[4];
    boxNormal[0].x = std::sin(box.m_radian);
    boxNormal[0].y = -std::cos(box.m_radian);
    boxNormal[1].x = -boxNormal[0].y;
    boxNormal[1].y = boxNormal[0].x;
    boxNormal[2].x = -boxNormal[1].y;
    boxNormal[2].y = boxNormal[1].x;
    boxNormal[3].x = -boxNormal[2].y;
    boxNormal[3].y = boxNormal[2].x;

    real bestDistance = std::numeric_limits<real>::lowest();
    uint16_t faceIndex;
    for (uint16_t i = 0; i < 4; i++)
    {
        math::Vector2 circleSP = circle.m_position + boxNormal[(i + uint16_t(2)) & uint16_t(0x0003)] * circle.m_radius;
        real d = math::Vector2::Dot(circleSP - box.m_pWorldVertices[i], boxNormal[i]);

        if (d > real(0.0))
            return false;

        if (d > bestDistance)
        {
            bestDistance = d;
            faceIndex = i;
        }
    }

    manifold.pReference = const_cast<Collider*>(&boxCollider);
    manifold.pIncident = const_cast<Collider*>(&circleCollider);

    // 보로노이 영역 테스트
    // 직사각형이라서 Normal 재활용 가능
    math::Vector2 vertexToCenter[2];
    unsigned int closestVertexIndex;        // 원의 중심과 가장 가까운 정점

    vertexToCenter[0] = circle.m_position - box.m_pWorldVertices[(faceIndex + uint16_t(1)) & uint16_t(0x0003)];
    real test = math::Vector2::Dot(vertexToCenter[0], boxNormal[(faceIndex - uint16_t(1)) & uint16_t(0x0003)]);
    if (test >= real(0.0))
    {
        vertexToCenter[1] = circle.m_position - box.m_pWorldVertices[faceIndex];
        test = math::Vector2::Dot(vertexToCenter[1], boxNormal[(faceIndex + uint16_t(1)) & uint16_t(0x0003)]);
        if (test >= real(0.0))
        {
            // face region
            manifold.collisionNormal = boxNormal[faceIndex];
            manifold.penetrationDepth = -bestDistance;


            manifold.contact[0] = circle.m_position - manifold.collisionNormal * (circle.m_radius + bestDistance);
            manifold.contactCount = 1;
            return true;
        }
        else
        {
            closestVertexIndex = 1;
        }
    }
    else
    {
        closestVertexIndex = 0;
    }

    real distanceSquared = vertexToCenter[closestVertexIndex].SquaredLength();
    if (distanceSquared > circle.m_radius * circle.m_radius)
        return false;

    real distance = std::sqrt(distanceSquared);
    manifold.collisionNormal = vertexToCenter[closestVertexIndex] / distance;
    manifold.penetrationDepth = circle.m_radius - distance;
    manifold.contact[0] = circle.m_position - manifold.collisionNormal * circle.m_radius;
    manifold.contactCount = 1;

    return true;
}

void Processor::FaceQueryBoxvsBox(Query& query, const BoxCollider& ref, const BoxCollider& inc)
{
    math::Vector2 refNormal[4];
    refNormal[0].x = std::sin(ref.m_radian);        // face[0](vertex[1]-vertex[0]) normal.x
    refNormal[0].y = -std::cos(ref.m_radian);       // face[0](vertex[1]-vertex[0]) normal.y
    refNormal[1].x = -refNormal[0].y;
    refNormal[1].y = refNormal[0].x;
    refNormal[2].x = -refNormal[1].y;
    refNormal[2].y = refNormal[1].x;
    refNormal[3].x = -refNormal[2].y;
    refNormal[3].y = refNormal[2].x;

    uint16_t incSupportIdx[4]; // [i] = ref normal i의 반대방향으로 검색한 other의 Support point
    // left normal (x, y) => (-y, x)

    incSupportIdx[0] = inc.SupportIndex(refNormal[2]);                  // normal[0]의 반대 방향 벡터인 normal[2] 사용
    incSupportIdx[2] = (incSupportIdx[0] + uint16_t(2)) & uint16_t(0x0003);       // Box To Box + SP 검색 비교 순서 CCW로 동일하므로 이렇게 계산 가능
    incSupportIdx[1] = (incSupportIdx[0] + uint16_t(1)) & uint16_t(0x0003);       // 위와 동일
    incSupportIdx[3] = (incSupportIdx[0] + uint16_t(3)) & uint16_t(0x0003);       // 위와 동일

    real bestDistanceFromRefToInc = std::numeric_limits<real>::lowest();
    uint16_t faceIndex;                    // face [i] = vertex[(i + 1) % 4] - vertex[i]
    uint16_t bestSupportIndexOfInc;
    for (uint16_t i = 0; i < 4; i++)
    {
        real d = math::Vector2::Dot(inc.m_pWorldVertices[incSupportIdx[i]] - ref.m_pWorldVertices[i], refNormal[i]);
        if (d > real(0.0))
        {
            query.axisExist = true;
            return;
        }

        if (d > bestDistanceFromRefToInc)
        {
            bestDistanceFromRefToInc = d;
            faceIndex = i;
            bestSupportIndexOfInc = incSupportIdx[i];
        }
    }

    query.axisExist = false;
    query.refFaceIndex = faceIndex;
    query.incSupportIndex = bestSupportIndexOfInc;
    query.bestDistance = bestDistanceFromRefToInc;
    query.refFaceNormal = refNormal[faceIndex];
}

void Processor::ComputeBoxvsBoxContactPoint(Manifold& manifold, uint16_t refFaceIndex, uint16_t incVertexIndex)
{
    const BoxCollider& ref = reinterpret_cast<const BoxCollider&>(*manifold.pReference);
    const BoxCollider& inc = reinterpret_cast<const BoxCollider&>(*manifold.pIncident);

    // n1, n2 (incVertex를 포함하는 두 면 각각의 법선 벡터)
    const math::Vector2 nPrev = inc.GetFaceNormal((incVertexIndex - uint16_t(1)) & uint16_t(0x0003));       // vertex index는 face index로도 쓰일 수 있음
    const math::Vector2 nNext = math::Vector2(-nPrev.y, nPrev.x);

    uint16_t incVertexIndex2;
    // ref의 충돌 법선이랑 내적하기 때문에더 작다는건 그만큼 평행하다는 것
    if (math::Vector2::Dot(nPrev, manifold.collisionNormal) < math::Vector2::Dot(nNext, manifold.collisionNormal))
        incVertexIndex2 = (incVertexIndex - uint16_t(1)) & uint16_t(0x0003);
    else
        incVertexIndex2 = (incVertexIndex + uint16_t(1)) & uint16_t(0x0003);

    // 최초 contact point가정
    math::Vector2 contactPoint[2] = { inc.m_pWorldVertices[incVertexIndex2], inc.m_pWorldVertices[incVertexIndex] };
    uint32_t contactCount = 2;

    // ===============
    // ======
    // 영역 너머 체크 파트
    // CP 제거 시 반대쪽 버텍스 살아있는 1, 2단계(ref면 왼쪽, 오른쪽면의 법선 테스트)의 경우는 클리핑 해야함. (지터 방지 투포인트 컨택 매니폴드 생성)
    // 단, 3단계(ref면 법선 테스트) 클리핑 시에는 반대쪽 버텍스 살아있어도 아예 제거. (=> 원포인트 컨택 매니폴드 완성)
    math::Vector2 invalidAreaDir;         // 클리핑 면 기준으로 이 방향에 있으면 클리핑 대상)

    // ========================
    // 클리핑 계산용 변수들 버퍼
    real ratio;
    math::Vector2 dir;      // 길이 성분도 유효 (ratio 곱해서 사용)
    // ========================

    // 1. ref 충돌 면의 왼쪽(반시계방향) 면부터 검사
    // 직사각형이므로 좌우측 클리핑에 필요한 노말들을 Ref 노말의 left, right normal을 써서 빠르게 계산할 수 있음
    // 면 기준으로 invalidAreaDir 방향 너머에 있으면 클리핑
    invalidAreaDir.x = -manifold.collisionNormal.y;
    invalidAreaDir.y = manifold.collisionNormal.x;

    // reference vertex 중 현재 클리핑 면을 이루는 면의 버텍스 둘 중 하나 선택
    // ref face가 바라보는 기준 왼쪽 영역 클리핑이니깐 refFaceIndex + 1 또는 +2번 버텍스 까지도 사용은 가능
    // 여기서는 refFaceIndex + 1번 버텍스 사용

    real d;
    d = math::Vector2::Dot(contactPoint[0] - ref.m_pWorldVertices[(refFaceIndex + uint16_t(1)) & uint16_t(0x0003)], invalidAreaDir);
    if (d > real(0.0))
    {
        // cp0이 클리핑 영역 너머에 있음
        // cp0 clipping code
        dir = contactPoint[1] - contactPoint[0];        // cp0 ------> cp1
        // cp1은 살아있다는 소리니깐 (invalidAreaDir 반대 방향에 있음)
        ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
        contactPoint[0] = contactPoint[0] + dir * ratio;            // NEW CONTACT POINT
    }
    else
    {
        d = math::Vector2::Dot(contactPoint[1] - ref.m_pWorldVertices[(refFaceIndex + uint16_t(1)) & uint16_t(0x0003)], invalidAreaDir);
        if (d > real(0.0))
        {
            // cp1이 클리핑 영역 너머에 있음
            // cp1 clipping code
            dir = contactPoint[0] - contactPoint[1];        // cp1 ------> cp0
            ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
            contactPoint[1] = contactPoint[1] + dir * ratio;        // NEW CONTACT POINT
        }
    }

    // 2. Clip right area
    // 충돌 법선의 right normal (y, -x) 사용
    invalidAreaDir.x = manifold.collisionNormal.y;
    invalidAreaDir.y = -manifold.collisionNormal.x;

    d = math::Vector2::Dot(contactPoint[0] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
    if (d > real(0.0))
    {
        // cp0이 클리핑 영역 너머에 있음
        // cp0 clipping code
        dir = contactPoint[1] - contactPoint[0];        // cp0 ------> cp1
        ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
        contactPoint[0] = contactPoint[0] + dir * ratio;            // NEW CONTACT POINT
    }
    else
    {
        d = math::Vector2::Dot(contactPoint[1] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
        if (d > real(0.0))
        {
            // cp1이 클리핑 영역 너머에 있음
            // cp1 clipping code
            dir = contactPoint[0] - contactPoint[1];        // cp1 ------> cp0
            ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
            contactPoint[1] = contactPoint[1] + dir * ratio;        // NEW CONTACT POINT
        }
    }

    // 3. Clip collision normal face area
    // 충돌 법선 그대로 사용
    invalidAreaDir = manifold.collisionNormal;

    d = math::Vector2::Dot(contactPoint[0] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
    if (d > real(0.0))
    {
        // cp0이 클리핑 영역 너머에 있음
        // cp0 제거
        contactPoint[0] = contactPoint[1];
        contactCount = 1;
    }
    else
    {
        d = math::Vector2::Dot(contactPoint[1] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
        if (d > real(0.0))
        {
            // cp1이 클리핑 영역 너머에 있음
            // cp1 제거
            // contact[0] = contact[0];
            contactCount = 1;
        }
    }

    manifold.contact[0] = contactPoint[0];
    manifold.contact[1] = contactPoint[1];
    manifold.contactCount = contactCount;
}

void Processor::ComputeBoxvsHullContactPoint(Manifold& manifold, uint16_t refFaceIndex, uint16_t incVertexIndex)
{
    const BoxCollider& ref = reinterpret_cast<const BoxCollider&>(*manifold.pReference);
    const HullCollider& inc = reinterpret_cast<const HullCollider&>(*manifold.pIncident);

    // n1, n2 (incVertex를 포함하는 두 면 각각의 법선 벡터)
    uint16_t nPrevIndex = inc.GetPrevVertexIndex(incVertexIndex);
    const math::Vector2 nPrev = inc.GetFaceNormal(nPrevIndex);       // vertex index는 face index로도 쓰일 수 있음
    const math::Vector2 nNext = inc.GetFaceNormal(incVertexIndex);

    uint16_t incVertexIndex2;
    // ref의 충돌 법선이랑 내적하기 때문에더 작다는건 그만큼 평행하다는 것
    if (math::Vector2::Dot(nPrev, manifold.collisionNormal) < math::Vector2::Dot(nNext, manifold.collisionNormal))
        incVertexIndex2 = nPrevIndex;
    else
    {
        incVertexIndex2 = inc.GetNextVertexIndex(incVertexIndex);
    }

    // 최초 contact point가정
    math::Vector2 contactPoint[2] = { inc.m_pWorldVertices[incVertexIndex2], inc.m_pWorldVertices[incVertexIndex] };
    uint32_t contactCount = 2;

    // ===============
    // ======
    // 영역 너머 체크 파트
    // CP 제거 시 반대쪽 버텍스 살아있는 1, 2단계(ref면 왼쪽, 오른쪽면의 법선 테스트)의 경우는 클리핑 해야함. (지터 방지 투포인트 컨택 매니폴드 생성)
    // 단, 3단계(ref면 법선 테스트) 클리핑 시에는 반대쪽 버텍스 살아있어도 아예 제거. (=> 원포인트 컨택 매니폴드 완성)
    math::Vector2 invalidAreaDir;         // 클리핑 면 기준으로 이 방향에 있으면 클리핑 대상)

    // ========================
    // 클리핑 계산용 변수들 버퍼
    real ratio;
    math::Vector2 dir;      // 길이 성분도 유효 (ratio 곱해서 사용)
    // ========================

    // 1. ref 충돌 면의 왼쪽(반시계방향) 면부터 검사
    // ref가 직사각형이므로 충돌 벡터 회전만으로 왼쪽, 오른쪽 면 노말 구할 수 있음
    // 면 기준으로 invalidAreaDir 방향 너머에 있으면 클리핑
    invalidAreaDir.x = -manifold.collisionNormal.y;     // 충돌 면 왼쪽 면의 노말(left normal로 구해짐)
    invalidAreaDir.y = manifold.collisionNormal.x;

    // reference vertex 중 현재 클리핑 면을 이루는 면의 버텍스 둘 중 하나 선택
    // ref face가 바라보는 기준 왼쪽 영역 클리핑이니깐 refFaceIndex + 1 또는 +2번 버텍스 까지도 사용은 가능
    // 여기서는 refFaceIndex + 1번 버텍스 사용

    real d;
    d = math::Vector2::Dot(contactPoint[0] - ref.m_pWorldVertices[(refFaceIndex + uint16_t(1)) & uint16_t(0x0003)], invalidAreaDir);
    if (d > real(0.0))
    {
        // cp0이 클리핑 영역 너머에 있음
        // cp0 clipping code
        dir = contactPoint[1] - contactPoint[0];        // cp0 ------> cp1
        // cp1은 살아있다는 소리니깐 (invalidAreaDir 반대 방향에 있음)
        ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
        contactPoint[0] = contactPoint[0] + dir * ratio;            // NEW CONTACT POINT
    }
    else
    {
        d = math::Vector2::Dot(contactPoint[1] - ref.m_pWorldVertices[(refFaceIndex + uint16_t(1)) & uint16_t(0x0003)], invalidAreaDir);
        if (d > real(0.0))
        {
            // cp1이 클리핑 영역 너머에 있음
            // cp1 clipping code
            dir = contactPoint[0] - contactPoint[1];        // cp1 ------> cp0
            ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
            contactPoint[1] = contactPoint[1] + dir * ratio;        // NEW CONTACT POINT
        }
    }

    // 2. Clip right area
    // 충돌 법선의 right normal (y, -x) 사용
    invalidAreaDir.x = manifold.collisionNormal.y;
    invalidAreaDir.y = -manifold.collisionNormal.x;

    d = math::Vector2::Dot(contactPoint[0] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
    if (d > real(0.0))
    {
        // cp0이 클리핑 영역 너머에 있음
        // cp0 clipping code
        dir = contactPoint[1] - contactPoint[0];        // cp0 ------> cp1
        ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
        contactPoint[0] = contactPoint[0] + dir * ratio;            // NEW CONTACT POINT
    }
    else
    {
        d = math::Vector2::Dot(contactPoint[1] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
        if (d > real(0.0))
        {
            // cp1이 클리핑 영역 너머에 있음
            // cp1 clipping code
            dir = contactPoint[0] - contactPoint[1];        // cp1 ------> cp0
            ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
            contactPoint[1] = contactPoint[1] + dir * ratio;        // NEW CONTACT POINT
        }
    }

    // 3. Clip collision normal face area
    // 충돌 법선 그대로 사용
    invalidAreaDir.x = manifold.collisionNormal.x;
    invalidAreaDir.y = manifold.collisionNormal.y;

    d = math::Vector2::Dot(contactPoint[0] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
    if (d > real(0.0))
    {
        // cp0이 클리핑 영역 너머에 있음
        // cp0 제거
        contactPoint[0] = contactPoint[1];
        contactCount = 1;
    }
    else
    {
        d = math::Vector2::Dot(contactPoint[1] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
        if (d > real(0.0))
        {
            // cp1이 클리핑 영역 너머에 있음
            // cp1 제거
            // contact[0] = contact[0];
            contactCount = 1;
        }
    }

    manifold.contact[0] = contactPoint[0];
    manifold.contact[1] = contactPoint[1];
    manifold.contactCount = contactCount;
}

void Processor::ComputeHullvsBoxContactPoint(Manifold& manifold, uint16_t refFaceIndex, uint16_t incVertexIndex)
{
    const HullCollider& ref = reinterpret_cast<const HullCollider&>(*manifold.pReference);
    const BoxCollider& inc = reinterpret_cast<const BoxCollider&>(*manifold.pIncident);

    // n1, n2 (incVertex를 포함하는 두 면 각각의 법선 벡터)
    const math::Vector2 nPrev = inc.GetFaceNormal((incVertexIndex - uint16_t(1)) & uint16_t(0x0003));       // vertex index는 face index로도 쓰일 수 있음
    const math::Vector2 nNext = inc.GetFaceNormal(incVertexIndex);

    uint16_t incVertexIndex2;
    // ref의 충돌 법선이랑 내적하기 때문에더 작다는건 그만큼 평행하다는 것
    if (math::Vector2::Dot(nPrev, manifold.collisionNormal) < math::Vector2::Dot(nNext, manifold.collisionNormal))
        incVertexIndex2 = (incVertexIndex - uint16_t(1)) & uint16_t(0x0003);
    else
        incVertexIndex2 = (incVertexIndex + uint16_t(1)) & uint16_t(0x0003);

    // 최초 contact point가정
    math::Vector2 contactPoint[2] = { inc.m_pWorldVertices[incVertexIndex2], inc.m_pWorldVertices[incVertexIndex] };
    uint32_t contactCount = 2;

    // ===============
    // ======
    // 영역 너머 체크 파트
    // CP 제거 시 반대쪽 버텍스 살아있는 1, 2단계(ref면 왼쪽, 오른쪽면의 법선 테스트)의 경우는 클리핑 해야함. (지터 방지 투포인트 컨택 매니폴드 생성)
    // 단, 3단계(ref면 법선 테스트) 클리핑 시에는 반대쪽 버텍스 살아있어도 아예 제거. (=> 원포인트 컨택 매니폴드 완성)
    math::Vector2 invalidAreaDir;         // 클리핑 면 기준으로 이 방향에 있으면 클리핑 대상)

    // ========================
    // 클리핑 계산용 변수들 버퍼
    real ratio;
    math::Vector2 dir;      // 길이 성분도 유효 (ratio 곱해서 사용)
    // ========================

    // 1. ref 충돌 면의 왼쪽(반시계방향) 면부터 검사
    // 면 기준으로 invalidAreaDir 방향 너머에 있으면 클리핑
    uint16_t faceQueryIndex = ref.GetNextFaceIndex(refFaceIndex);
    invalidAreaDir = ref.GetFaceNormal(faceQueryIndex);

    // reference vertex 중 현재 클리핑 면을 이루는 면의 버텍스 둘 중 하나 선택
    // ref face가 바라보는 기준 왼쪽 영역 클리핑이니깐 refFaceIndex + 1 또는 +2번 버텍스 까지도 사용은 가능
    // 여기서는 refFaceIndex + 1번 버텍스 사용

    real d;
    d = math::Vector2::Dot(contactPoint[0] - ref.m_pWorldVertices[faceQueryIndex], invalidAreaDir);
    if (d > real(0.0))
    {
        // cp0이 클리핑 영역 너머에 있음
        // cp0 clipping code
        dir = contactPoint[1] - contactPoint[0];        // cp0 ------> cp1
        // cp1은 살아있다는 소리니깐 (invalidAreaDir 반대 방향에 있음)
        ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
        contactPoint[0] = contactPoint[0] + dir * ratio;            // NEW CONTACT POINT
    }
    else
    {
        d = math::Vector2::Dot(contactPoint[1] - ref.m_pWorldVertices[faceQueryIndex], invalidAreaDir);
        if (d > real(0.0))
        {
            // cp1이 클리핑 영역 너머에 있음
            // cp1 clipping code
            dir = contactPoint[0] - contactPoint[1];        // cp1 ------> cp0
            ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
            contactPoint[1] = contactPoint[1] + dir * ratio;        // NEW CONTACT POINT
        }
    }

    // 2. Clip right area
    faceQueryIndex = ref.GetPrevFaceIndex(refFaceIndex);
    invalidAreaDir = ref.GetFaceNormal(faceQueryIndex);

    d = math::Vector2::Dot(contactPoint[0] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
    if (d > real(0.0))
    {
        // cp0이 클리핑 영역 너머에 있음
        // cp0 clipping code
        dir = contactPoint[1] - contactPoint[0];        // cp0 ------> cp1
        ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
        contactPoint[0] = contactPoint[0] + dir * ratio;            // NEW CONTACT POINT
    }
    else
    {
        d = math::Vector2::Dot(contactPoint[1] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
        if (d > real(0.0))
        {
            // cp1이 클리핑 영역 너머에 있음
            // cp1 clipping code
            dir = contactPoint[0] - contactPoint[1];        // cp1 ------> cp0
            ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
            contactPoint[1] = contactPoint[1] + dir * ratio;        // NEW CONTACT POINT
        }
    }

    // 3. Clip collision normal face area
    // 충돌 법선 그대로 사용
    invalidAreaDir.x = manifold.collisionNormal.x;
    invalidAreaDir.y = manifold.collisionNormal.y;

    d = math::Vector2::Dot(contactPoint[0] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
    if (d > real(0.0))
    {
        // cp0이 클리핑 영역 너머에 있음
        // cp0 제거
        contactPoint[0] = contactPoint[1];
        contactCount = 1;
    }
    else
    {
        d = math::Vector2::Dot(contactPoint[1] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
        if (d > real(0.0))
        {
            // cp1이 클리핑 영역 너머에 있음
            // cp1 제거
            // contact[0] = contact[0];
            contactCount = 1;
        }
    }

    manifold.contact[0] = contactPoint[0];
    manifold.contact[1] = contactPoint[1];
    manifold.contactCount = contactCount;
}

void Processor::FaceQueryBoxvsHull(Query& query, const BoxCollider& ref, const HullCollider& inc)
{
    math::Vector2 refNormal[4];
    refNormal[0].x = std::sin(ref.m_radian);        // face[0](vertex[1]-vertex[0]) normal.x
    refNormal[0].y = -std::cos(ref.m_radian);       // face[0](vertex[1]-vertex[0]) normal.y
    refNormal[1].x = -refNormal[0].y;
    refNormal[1].y = refNormal[0].x;
    refNormal[2].x = -refNormal[1].y;
    refNormal[2].y = refNormal[1].x;
    refNormal[3].x = -refNormal[2].y;
    refNormal[3].y = refNormal[2].x;

    uint16_t incSupportIndex[4]; // [i] = ref normal i의 반대방향으로 검색한 other의 Support point index
    // left normal (x, y) => (-y, x)

    incSupportIndex[0] = inc.SupportIndex(refNormal[2]);       // ref 0번 면에 대한 inc의 support point검색은 ref 2번 노말 사용하면 됨
    incSupportIndex[1] = inc.SupportIndex(refNormal[3]);
    incSupportIndex[2] = inc.SupportIndex(refNormal[0]);
    incSupportIndex[3] = inc.SupportIndex(refNormal[1]);

    real bestDistance = std::numeric_limits<real>::lowest();
    uint16_t bestFaceIndex;                    // face [i] = vertex[(i + 1) % 4] - vertex[i]
    uint16_t incBestSupportIndex;
    for (uint16_t i = 0; i < 4; i++)
    {
        real d = math::Vector2::Dot(inc.m_pWorldVertices[incSupportIndex[i]] - ref.m_pWorldVertices[i], refNormal[i]);
        if (d > real(0.0))
        {
            query.axisExist = true;
            return;
        }

        if (d > bestDistance)
        {
            bestDistance = d;
            bestFaceIndex = i;
            incBestSupportIndex = incSupportIndex[i];
        }
    }

    query.axisExist = false;
    query.refFaceIndex = bestFaceIndex;
    query.incSupportIndex = incBestSupportIndex;
    query.bestDistance = bestDistance;
    query.refFaceNormal = refNormal[bestFaceIndex];
}

void Processor::FaceQueryHullvsBox(Query& query, const HullCollider& ref, const BoxCollider& inc)
{
    real bestDistance = std::numeric_limits<real>::lowest();
    math::Vector2 bestFaceNormal;
    uint16_t bestFaceIndex;                    // face [i] = vertex[(i + 1) % 4] - vertex[i]
    uint16_t incBestSupportIndex;

    uint16_t hullVertexCount = ref.m_vertexCount;
    for (uint16_t i = 0; i < hullVertexCount; i++)
    {
        math::Vector2 refNormal = ref.GetFaceNormal(i);
        uint16_t incSupportIndex = inc.SupportIndex(-refNormal);
        math::Vector2 incSupport = inc.m_pWorldVertices[incSupportIndex];

        real d = math::Vector2::Dot(incSupport - ref.m_pWorldVertices[i], refNormal);
        if (d > real(0.0))
        {
            query.axisExist = true;
            return;
        }

        if (d > bestDistance)
        {
            bestFaceNormal = refNormal;
            bestDistance = d;
            bestFaceIndex = i;
            incBestSupportIndex = incSupportIndex;
        }
    }

    query.axisExist = false;
    query.refFaceIndex = bestFaceIndex;
    query.incSupportIndex = incBestSupportIndex;
    query.bestDistance = bestDistance;
    query.refFaceNormal = bestFaceNormal;
}
 
bool Processor::CheckBoxvsBoxCollision(Manifold& manifold, const Collider& boxColliderA, const Collider& boxColliderB)
{
    Query queryA_B;     // BoxA->BoxB Query
    Query queryB_A;     // BoxB->BoxA Query

    // -------------------------------------------
    // Test A->B                                //
    FaceQueryBoxvsBox(queryA_B, reinterpret_cast<const BoxCollider&>(boxColliderA), reinterpret_cast<const BoxCollider&>(boxColliderB));    //
    if (queryA_B.axisExist)                     //
        return false;                           //
    // -------------------------------------------

    // -------------------------------------------
    // Test B->A                                //
    FaceQueryBoxvsBox(queryB_A, reinterpret_cast<const BoxCollider&>(boxColliderB), reinterpret_cast<const BoxCollider&>(boxColliderA));    //
    if (queryB_A.axisExist)                     //
        return false;                           //
    // -------------------------------------------

    if (queryA_B.bestDistance > queryB_A.bestDistance)
    {
        // ref = boxA
        manifold.pReference = const_cast<Collider*>(&boxColliderA);
        manifold.pIncident = const_cast<Collider*>(&boxColliderB);
        manifold.collisionNormal = queryA_B.refFaceNormal;
        manifold.penetrationDepth = -queryA_B.bestDistance;
        ComputeBoxvsBoxContactPoint(manifold, queryA_B.refFaceIndex, queryA_B.incSupportIndex);
    }
    else
    {
        // ref = boxB
        manifold.pReference = const_cast<Collider*>(&boxColliderB);
        manifold.pIncident = const_cast<Collider*>(&boxColliderA);
        manifold.collisionNormal = queryB_A.refFaceNormal;
        manifold.penetrationDepth = -queryB_A.bestDistance;
        ComputeBoxvsBoxContactPoint(manifold, queryB_A.refFaceIndex, queryB_A.incSupportIndex);
    }

    return true;
}

bool Processor::CheckBoxvsHullCollision(Manifold& manifold, const Collider& boxCollider, const Collider& hullCollider)
{
    Query queryB_H;     // Box->Hull Query
    Query queryH_B;     // Hull->Box Query

    // -------------------------------------------
    // Test Box->Hull                           //
    FaceQueryBoxvsHull(queryB_H, reinterpret_cast<const BoxCollider&>(boxCollider), reinterpret_cast<const HullCollider&>(hullCollider));    //
    if (queryB_H.axisExist)                     //
        return false;                           //
    // -------------------------------------------

    // -------------------------------------------
    // Test Hull->Box                           //
    FaceQueryHullvsBox(queryH_B, reinterpret_cast<const HullCollider&>(hullCollider), reinterpret_cast<const BoxCollider&>(boxCollider));    //
    if (queryH_B.axisExist)                     //
        return false;                           //
    // -------------------------------------------

    if (queryB_H.bestDistance > queryH_B.bestDistance)
    {
        // ref = boxA
        manifold.pReference = const_cast<Collider*>(&boxCollider);
        manifold.pIncident = const_cast<Collider*>(&hullCollider);
        manifold.collisionNormal = queryB_H.refFaceNormal;
        manifold.penetrationDepth = -queryB_H.bestDistance;
        ComputeBoxvsHullContactPoint(manifold, queryB_H.refFaceIndex, queryB_H.incSupportIndex);
    }
    else
    {
        // ref = boxB
        manifold.pReference = const_cast<Collider*>(&hullCollider);
        manifold.pIncident = const_cast<Collider*>(&boxCollider);
        manifold.collisionNormal = queryH_B.refFaceNormal;
        manifold.penetrationDepth = -queryH_B.bestDistance;
        ComputeHullvsBoxContactPoint(manifold, queryH_B.refFaceIndex, queryH_B.incSupportIndex);
    }

    return true;
}

bool Processor::CheckHullvsBoxCollision(Manifold& manifold, const Collider& hullCollider, const Collider& boxCollider)
{
    return CheckBoxvsHullCollision(manifold, boxCollider, hullCollider);
}
*/