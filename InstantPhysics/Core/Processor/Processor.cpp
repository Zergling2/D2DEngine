#include "Processor.h"
#include "InstantPhysics.h"
#include "Components\Simplex.h"
#include "Core\Acc\AABB.h"
#include "Components\RigidBody.h"
#include "Components\CircleCollider.h"
#include "Components\PolygonCollider.h"
#include <vector>

using namespace ip;

Processor::Processor()
{
}

Processor::~Processor()
{
}

void Processor::Update()
{
    UpdateRigidBodyTransform();
    CollisionDetection();
    ResolveCollision();
}

CircleCollider* Processor::CreateCircleCollider(real radius)
{
    CircleCollider* pNewCollider = new CircleCollider(radius);
    m_colliders.push_back(pNewCollider);

    return pNewCollider;
}

PolygonCollider* Processor::CreatePolygonCollider(const math::Vector2 convexShapeVerticesCCW[], uint32_t vertexCount)
{
    PolygonCollider* pNewCollider = new PolygonCollider(convexShapeVerticesCCW, vertexCount);
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
        // Symplectic Euler
        Step(*(*iter)->m_pRigidBody);

        ColliderType type = (*iter)->GetType();
        switch (type)
        {
        case ColliderType::Circle:
            // 정점 월드변환 할거 없음
            break;
        case ColliderType::Polygon:
            WorldTransformPolygonVertices(reinterpret_cast<PolygonCollider&>(**iter));
            break;
        default:
            break;
        }
        ++iter;
    }
}

bool Processor::CheckAABBCollision(const AABB* const pAABB1, const AABB* const pAABB2) const
{
    if (pAABB1->m_max.x < pAABB2->m_min.x || pAABB1->m_max.y < pAABB2->m_min.y ||
        pAABB1->m_min.x > pAABB2->m_max.x || pAABB1->m_min.y > pAABB2->m_max.y)
    {
        return false;
    }
    return true;
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

                if (colliderA.GetRigidBody()->m_isStatic && colliderB.GetRigidBody()->m_isStatic)
                    break;

                switch (colliderA.GetType())
                {
                case ColliderType::Circle:
                    switch (colliderB.GetType())
                    {
                    case ColliderType::Circle:
                        result = CheckCollisionCircleToCircle(&m, reinterpret_cast<const CircleCollider&>(colliderA), reinterpret_cast<const CircleCollider&>(colliderB));
                        break;
                    case ColliderType::Polygon:
                        result = CheckCollisionCircleToPolygon(&m, reinterpret_cast<const CircleCollider&>(colliderA), reinterpret_cast<const PolygonCollider&>(colliderB));
                        break;
                    default:
                        *reinterpret_cast<int*>(0x00000000) = 0;
                        break;
                    }
                    break;
                case ColliderType::Polygon:
                    switch (colliderB.GetType())
                    {
                    case ColliderType::Circle:
                        result = CheckCollisionCircleToPolygon(&m, reinterpret_cast<const CircleCollider&>(colliderB), reinterpret_cast<const PolygonCollider&>(colliderA));
                        break;
                    case ColliderType::Polygon:
                        result = CheckCollisionPolygonToPolygon(&m, reinterpret_cast<const PolygonCollider&>(colliderA), reinterpret_cast<const PolygonCollider&>(colliderB));
                        break;
                    default:
                        *reinterpret_cast<int*>(0x00000000) = 0;
                        break;
                    }
                    break;
                default:
                    break;
                }

                if (result)
                {
                    m_resolveList.push_back(m);
                }
            } while (false);
            ++j;
        }
        ++i;
    }
}

void Processor::ResolveCollision()
{
    std::list<Manifold>::iterator end = m_resolveList.end();
    std::list<Manifold>::iterator iter = m_resolveList.begin();
    while (iter != end)
    {
        do
        {
            const Manifold& manifold = *iter;
            RigidBody& rigidBodyA = *manifold.pColliderA->m_pRigidBody;
            RigidBody& rigidBodyB = *manifold.pColliderB->m_pRigidBody;

            math::Vector2 rVel = rigidBodyB.m_linearVelocity - rigidBodyA.m_linearVelocity;
        
            real velAlongNormal = math::Vector2::Dot(rVel, manifold.collisionNormal);
            if (velAlongNormal > real(0.0))
                break;
        
            real e = std::min(rigidBodyA.m_restitution, rigidBodyB.m_restitution);
            real j = (-(real(1.0) + e) * velAlongNormal) / (rigidBodyA.m_invMass + rigidBodyB.m_invMass);
        
            math::Vector2 impulse = j * manifold.collisionNormal;
            rigidBodyA.m_linearVelocity -= rigidBodyA.m_invMass * impulse;
            rigidBodyB.m_linearVelocity += rigidBodyB.m_invMass * impulse;

            // Corrects Floating Point Error
            const real percent = real(0.8);     // usually 20% to 80%
            const real slop = real(0.01);       // usually 0.01 to 0.1
            // math::Vector2 correctionDir = manifold.collisionNormal *
            //     (manifold.penetrationDepth * percent / (rigidBodyA.m_invMass + rigidBodyB.m_invMass));
            math::Vector2 correctionDir = manifold.collisionNormal * 
                (std::max(manifold.penetrationDepth - slop, real(0.0)) * percent / (rigidBodyA.m_invMass + rigidBodyB.m_invMass));
            manifold.pColliderA->m_position -= rigidBodyA.m_invMass * correctionDir;
            manifold.pColliderB->m_position += rigidBodyB.m_invMass * correctionDir;
        } while (false);
        iter = m_resolveList.erase(iter);
    }
}

void Processor::WorldTransformPolygonVertices(PolygonCollider& polygonCollider)
{
    math::Matrix3x2R tMat(math::Matrix3x2R::Rotation(polygonCollider.m_orientation) * math::Matrix3x2R::Translation(polygonCollider.m_position));

    for (uint32_t index = 0; index < polygonCollider.m_vertexCount; index++)
    {
        polygonCollider.m_pWorldVertices[index] = polygonCollider.m_pVertices[index] * tMat;
    }
}

void Processor::Step(RigidBody& rigidBody)
{
    if (rigidBody.m_isStatic)
        return;

    rigidBody.m_force += math::Vector2(real(0.0), real(-98.1)) * rigidBody.m_mass;

    rigidBody.m_linearVelocity += (rigidBody.m_force * rigidBody.m_invMass) * FIXED_DELTA_TIME;
    rigidBody.m_pCollider->m_position += rigidBody.m_linearVelocity * FIXED_DELTA_TIME;

    // 각속도 적용
    //

    rigidBody.m_force = math::Vector2::zero;         // initialize force

    // rigidBody.m_linearVelocity += (rigidBody.m_invMass * rigidBody.m_force) * FIXED_DELTA_TIME;
    // rigidBody.m_pos += rigidBody.m_linearVelocity * FIXED_DELTA_TIME;
}

bool Processor::CheckCollisionCircleToCircle(Manifold* pManifold, const CircleCollider& circleColliderA, const CircleCollider& circleColliderB)
{
    math::Vector2 v = circleColliderB.m_position - circleColliderA.m_position;
    real distancePower2 = v.SquaredLength();
    real radiusSum = circleColliderA.m_radius + circleColliderB.m_radius;
    
    if (distancePower2 > radiusSum * radiusSum)
        return false;
    
    pManifold->pColliderA = const_cast<CircleCollider*>(&circleColliderA);
    pManifold->pColliderB = const_cast<CircleCollider*>(&circleColliderB);
    
    if (distancePower2 != real(0.0))
    {
        real distance = std::sqrt(distancePower2);
        pManifold->penetrationDepth = radiusSum - distance;
        pManifold->collisionNormal = v / distance;
    }
    else
    {
        pManifold->penetrationDepth = circleColliderA.m_radius;
        pManifold->collisionNormal = math::Vector2(real(1.0), real(0.0));       // collision resolution constant vector
    }
    return true;
}

bool Processor::CheckCollisionCircleToPolygon(Manifold* pManifold, const CircleCollider& circleCollider, const PolygonCollider& polygonCollider)
{
    Simplex simplex(&circleCollider, &polygonCollider);
    Simplex::State state;

    while ((state = simplex.EvolveSimplex()) == Simplex::State::Evolving);

    if (state == Simplex::State::Intersecting)
        simplex.ComputeManifold(pManifold);

    return static_cast<bool>(state);        // Intersecting = 1(true), Not Intersecting = 0(false)
}

bool Processor::CheckCollisionPolygonToPolygon(Manifold* pManifold, const PolygonCollider& polygonColliderA, const PolygonCollider& polygonColliderB)
{
    Simplex simplex(&polygonColliderA, &polygonColliderB);
    Simplex::State state;

    while ((state = simplex.EvolveSimplex()) == Simplex::State::Evolving);

    if (state == Simplex::State::Intersecting)
        simplex.ComputeManifold(pManifold);

    return static_cast<bool>(state);        // Intersecting = 1(true), Not Intersecting = 0(false)
}

/*
real Processor::FindALPBoxToBox(uint32_t* faceIndex, const BoxCollider* const pBoxA, const BoxCollider* const pBoxB)
{
    real bestDistance;
    uint32_t bestIndex;
    real d;
    math::Vector2 n;

    // 1.
    // Retrieve a face normal from A
    n.x = pBoxA->m_points[1].y - pBoxA->m_points[0].y;
    n.y = -(pBoxA->m_points[1].x - pBoxA->m_points[0].x);

    // Retrieve support point from B along -n
    math::Vector2 s = pBoxB->GetSupport(n * real(-1.0));

    // Retrieve vertex on face from A, transform into
    // B's model space
    math::Vector2 v = pBoxA->m_points[0];
    bestDistance = math::Vector2::Dot(n, s - v);
    bestIndex = 0;

    // 2.
    // Retrieve a face normal from A
    n.x = pBoxA->m_points[2].y - pBoxA->m_points[1].y;
    n.y = -(pBoxA->m_points[2].x - pBoxA->m_points[1].x);

    // Retrieve support point from B along -n
    math::Vector2 s = pBoxB->GetSupport(n * real(-1.0));

    // Retrieve vertex on face from A, transform into
    // B's model space
    math::Vector2 v = pBoxA->m_points[1];
    d = math::Vector2::Dot(n, s - v);
    if (d > bestDistance)
    {
        bestDistance = d;
        bestIndex = 1;
    }

    // 3.
    // Retrieve a face normal from A
    n.x = pBoxA->m_points[3].y - pBoxA->m_points[2].y;
    n.y = -(pBoxA->m_points[3].x - pBoxA->m_points[2].x);

    // Retrieve support point from B along -n
    math::Vector2 s = pBoxB->GetSupport(n * real(-1.0));

    // Retrieve vertex on face from A, transform into
    // B's model space
    math::Vector2 v = pBoxA->m_points[2];
    d = math::Vector2::Dot(n, s - v);
    if (d > bestDistance)
    {
        bestDistance = d;
        bestIndex = 2;
    }

    // 4.
    // Retrieve a face normal from A
    n.x = pBoxA->m_points[0].y - pBoxA->m_points[3].y;
    n.y = -(pBoxA->m_points[0].x - pBoxA->m_points[3].x);

    // Retrieve support point from B along -n
    math::Vector2 s = pBoxB->GetSupport(n * real(-1.0));

    // Retrieve vertex on face from A, transform into
    // B's model space
    math::Vector2 v = pBoxA->m_points[3];
    d = math::Vector2::Dot(n, s - v);
    if (d > bestDistance)
    {
        bestDistance = d;
        bestIndex = 3;
    }

    *faceIndex = bestIndex;
    return bestDistance;
}

bool Processor::CheckBoxToCircleCollision(Manifold* pManifold, const BoxCollider& pBox, const RigidBody* const pCircle)
{
    const BoxCollider* const pBoxCollider = reinterpret_cast<const BoxCollider* const>(pBox->m_pCollider);
    const CircleCollider* const pCircleCollider = reinterpret_cast<const CircleCollider* const>(pCircle->m_pCollider);
    
    math::Vector2 projAxis;
    math::Vector2 boxToCircle = pCircle->m_pos - pBox->m_pos;
    real l;
    
    real radian = math::DegreeToRadian(pBox->m_orientation);
    // 테스트 축 1 - OBB x axis
    projAxis.x = std::cos(radian);
    projAxis.y = std::sin(radian);
    l = std::abs(math::Vector2::Dot(boxToCircle, projAxis));
    if (pBoxCollider->m_extentABS.x + pCircleCollider->m_radius < l)
        return false;

    // 테스트 축 2 - OBB y axis
    real temp = projAxis.x;
    projAxis.x = -projAxis.y;
    projAxis.y = temp;
    l = std::abs(math::Vector2::Dot(boxToCircle, projAxis));
    if (pBoxCollider->m_extentABS.y + pCircleCollider->m_radius < l)
        return false;

    // 테스트 축 3 - Box to circle vector
    real boxToCircleLength = boxToCircle.Length();
    projAxis = boxToCircle / boxToCircleLength;        // Projection Axis = Normalized boxToCircle vector
    real boxMax = math::Vector2::Dot(pBoxCollider->m_points[0] - pBox->m_pos, projAxis);
    real dotValue[3];
    dotValue[0] = math::Vector2::Dot(pBoxCollider->m_points[1] - pBox->m_pos, projAxis);
    dotValue[1] = math::Vector2::Dot(pBoxCollider->m_points[2] - pBox->m_pos, projAxis);
    dotValue[2] = math::Vector2::Dot(pBoxCollider->m_points[3] - pBox->m_pos, projAxis);
    if (boxMax < dotValue[0]) boxMax = dotValue[0];
    if (boxMax < dotValue[1]) boxMax = dotValue[1];
    if (boxMax < dotValue[2]) boxMax = dotValue[2];

    if (boxToCircleLength > pCircleCollider->m_radius + boxMax)
        return false;

    return true;
}

bool Processor::CheckBoxToBoxCollision(Manifold* pManifold, const RigidBody* const pBox1, const RigidBody* const pBox2)
{
    const BoxCollider* const pBoxCollider1 = reinterpret_cast<const BoxCollider* const>(pBox1->m_pCollider);
    const BoxCollider* const pBoxCollider2 = reinterpret_cast<const BoxCollider* const>(pBox2->m_pCollider);

    // math::Vector2 v = pBox2->m_pos - pBox1->m_pos;   // OBB 충돌 감지 전까지 보류
    math::Vector2 projAxis;
    real box1Min;
    real box1Max;
    real box2Min;
    real box2Max;
    real radian;

    // Right normal 사용 (x, y) => (y, -x)
    // 1. Box1 축1
    radian = math::DegreeToRadian(pBox1->m_orientation);
    projAxis.x = std::cos(radian);
    projAxis.y = std::sin(radian);
    pBoxCollider1->GetWorldMinMax(projAxis, &box1Min, &box1Max);
    pBoxCollider2->GetWorldMinMax(projAxis, &box2Min, &box2Max);
    if (box1Max < box2Min || box2Max < box1Min)
        return false;

    // 2. Box1 축2
    real temp = projAxis.x;
    projAxis.x = -projAxis.y;
    projAxis.y = temp;
    pBoxCollider1->GetWorldMinMax(projAxis, &box1Min, &box1Max);
    pBoxCollider2->GetWorldMinMax(projAxis, &box2Min, &box2Max);
    if (box1Max < box2Min || box2Max < box1Min)
        return false;

    // 3. Box2 축1
    radian = math::DegreeToRadian(pBox2->m_orientation);
    projAxis.x = std::cos(radian);
    projAxis.y = std::sin(radian);
    pBoxCollider1->GetWorldMinMax(projAxis, &box1Min, &box1Max);
    pBoxCollider2->GetWorldMinMax(projAxis, &box2Min, &box2Max);
    if (box1Max < box2Min || box2Max < box1Min)
        return false;

    // 4. Box2 축2
    temp = projAxis.x;
    projAxis.x = -projAxis.y;
    projAxis.y = temp;
    pBoxCollider1->GetWorldMinMax(projAxis, &box1Min, &box1Max);
    pBoxCollider2->GetWorldMinMax(projAxis, &box2Min, &box2Max);
    if (box1Max < box2Min || box2Max < box1Min)
        return false;
    
    return true;
}
*/