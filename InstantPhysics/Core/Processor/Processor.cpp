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
    // ResolveCollision();
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
        // Step((*iter));

        ColliderType type = (*iter)->GetType();
        switch (type)
        {
        case ColliderType::Circle:
            // ���� ���庯ȯ �Ұ� ����
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
            ColliderType typeA = (*i)->GetType();
            ColliderType typeB = (*j)->GetType();

            switch (typeA)
            {
            case ColliderType::Circle:
                switch (typeB)
                {
                case ColliderType::Circle:
                    result = CheckCollisionCircleToCircle(&m, reinterpret_cast<CircleCollider&>(**i), reinterpret_cast<CircleCollider&>(**j));
                    break;
                case ColliderType::Polygon:
                    result = CheckCollisionCircleToPolygon(&m, reinterpret_cast<CircleCollider&>(**i), reinterpret_cast<PolygonCollider&>(**j));
                    break;
                default:
                    *reinterpret_cast<int*>(0x00000000) = 0;
                    break;
                }
                break;
            case ColliderType::Polygon:
                switch (typeB)
                {
                case ColliderType::Circle:
                    result = CheckCollisionCircleToPolygon(&m, reinterpret_cast<CircleCollider&>(**j), reinterpret_cast<PolygonCollider&>(**i));
                    break;
                case ColliderType::Polygon:
                    result = CheckCollisionPolygonToPolygon(&m, reinterpret_cast<PolygonCollider&>(**i), reinterpret_cast<PolygonCollider&>(**j));
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
                m_resolveList.push_back(m);
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
            // (*iter).pRigidBodyA->m_pos -= math::Vector2((*iter).collisionNormal * (*iter).penetrationDepth / real(2.0));
            // (*iter).pRigidBodyB->m_pos += math::Vector2((*iter).collisionNormal * (*iter).penetrationDepth / real(2.0));

            math::Vector2 rVel = (*iter).pRigidBodyB->m_linearVelocity - (*iter).pRigidBodyA->m_linearVelocity;
        
            real velAlongNormal = math::Vector2::Dot(rVel, (*iter).collisionNormal);
            if (velAlongNormal > real(0.0))
                break;
        
            real e = std::min((*iter).pRigidBodyA->m_restitution, (*iter).pRigidBodyB->m_restitution);
            real j = (-(real(1.0) + e) * velAlongNormal) / ((*iter).pRigidBodyA->m_invMass + (*iter).pRigidBodyB->m_invMass);
        
            math::Vector2 impulse = j * (*iter).collisionNormal;
            (*iter).pRigidBodyA->m_linearVelocity -= (*iter).pRigidBodyA->m_invMass * impulse;
            (*iter).pRigidBodyB->m_linearVelocity += (*iter).pRigidBodyB->m_invMass * impulse;
        } while (false);
        iter = m_resolveList.erase(iter);
    }
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


// this is performed after the impulse is applied.
void PositionalCorrection(Manifold* pManifold)
{
    const real percent = real(0.2);
    const real slop = real(0.01);
    real o = pManifold->penetrationDepth - slop;
    if (o > real(0.0))
    {
        math::Vector2 correction = o / (pManifold->pRigidBodyA->m_invMass + pManifold->pRigidBodyB->m_invMass)
            * percent * pManifold->collisionNormal;
        pManifold->pRigidBodyA->m_pos -= pManifold->pRigidBodyA->m_invMass * correction;
        pManifold->pRigidBodyB->m_pos += pManifold->pRigidBodyB->m_invMass * correction;
    }
}
*/

// bool Processor::CheckBoxToCircleCollision(Manifold* pManifold, const BoxCollider& pBox, const RigidBody* const pCircle)
// {
//     const BoxCollider* const pBoxCollider = reinterpret_cast<const BoxCollider* const>(pBox->m_pCollider);
//     const CircleCollider* const pCircleCollider = reinterpret_cast<const CircleCollider* const>(pCircle->m_pCollider);
//     
//     math::Vector2 projAxis;
//     math::Vector2 boxToCircle = pCircle->m_pos - pBox->m_pos;
//     real l;
//     
//     real radian = math::DegreeToRadian(pBox->m_orientation);
//     // �׽�Ʈ �� 1 - OBB x axis
//     projAxis.x = std::cos(radian);
//     projAxis.y = std::sin(radian);
//     l = std::abs(math::Vector2::Dot(boxToCircle, projAxis));
//     if (pBoxCollider->m_extentABS.x + pCircleCollider->m_radius < l)
//         return false;
// 
//     // �׽�Ʈ �� 2 - OBB y axis
//     real temp = projAxis.x;
//     projAxis.x = -projAxis.y;
//     projAxis.y = temp;
//     l = std::abs(math::Vector2::Dot(boxToCircle, projAxis));
//     if (pBoxCollider->m_extentABS.y + pCircleCollider->m_radius < l)
//         return false;
// 
//     // �׽�Ʈ �� 3 - Box to circle vector
//     real boxToCircleLength = boxToCircle.Length();
//     projAxis = boxToCircle / boxToCircleLength;        // Projection Axis = Normalized boxToCircle vector
//     real boxMax = math::Vector2::Dot(pBoxCollider->m_points[0] - pBox->m_pos, projAxis);
//     real dotValue[3];
//     dotValue[0] = math::Vector2::Dot(pBoxCollider->m_points[1] - pBox->m_pos, projAxis);
//     dotValue[1] = math::Vector2::Dot(pBoxCollider->m_points[2] - pBox->m_pos, projAxis);
//     dotValue[2] = math::Vector2::Dot(pBoxCollider->m_points[3] - pBox->m_pos, projAxis);
//     if (boxMax < dotValue[0]) boxMax = dotValue[0];
//     if (boxMax < dotValue[1]) boxMax = dotValue[1];
//     if (boxMax < dotValue[2]) boxMax = dotValue[2];
// 
//     if (boxToCircleLength > pCircleCollider->m_radius + boxMax)
//         return false;
// 
//     return true;
// }
// 
// bool Processor::CheckBoxToBoxCollision(Manifold* pManifold, const RigidBody* const pBox1, const RigidBody* const pBox2)
// {
//     const BoxCollider* const pBoxCollider1 = reinterpret_cast<const BoxCollider* const>(pBox1->m_pCollider);
//     const BoxCollider* const pBoxCollider2 = reinterpret_cast<const BoxCollider* const>(pBox2->m_pCollider);
// 
//     // math::Vector2 v = pBox2->m_pos - pBox1->m_pos;   // OBB �浹 ���� ������ ����
//     math::Vector2 projAxis;
//     real box1Min;
//     real box1Max;
//     real box2Min;
//     real box2Max;
//     real radian;
// 
//     // Right normal ��� (x, y) => (y, -x)
//     // 1. Box1 ��1
//     radian = math::DegreeToRadian(pBox1->m_orientation);
//     projAxis.x = std::cos(radian);
//     projAxis.y = std::sin(radian);
//     pBoxCollider1->GetWorldMinMax(projAxis, &box1Min, &box1Max);
//     pBoxCollider2->GetWorldMinMax(projAxis, &box2Min, &box2Max);
//     if (box1Max < box2Min || box2Max < box1Min)
//         return false;
// 
//     // 2. Box1 ��2
//     real temp = projAxis.x;
//     projAxis.x = -projAxis.y;
//     projAxis.y = temp;
//     pBoxCollider1->GetWorldMinMax(projAxis, &box1Min, &box1Max);
//     pBoxCollider2->GetWorldMinMax(projAxis, &box2Min, &box2Max);
//     if (box1Max < box2Min || box2Max < box1Min)
//         return false;
// 
//     // 3. Box2 ��1
//     radian = math::DegreeToRadian(pBox2->m_orientation);
//     projAxis.x = std::cos(radian);
//     projAxis.y = std::sin(radian);
//     pBoxCollider1->GetWorldMinMax(projAxis, &box1Min, &box1Max);
//     pBoxCollider2->GetWorldMinMax(projAxis, &box2Min, &box2Max);
//     if (box1Max < box2Min || box2Max < box1Min)
//         return false;
// 
//     // 4. Box2 ��2
//     temp = projAxis.x;
//     projAxis.x = -projAxis.y;
//     projAxis.y = temp;
//     pBoxCollider1->GetWorldMinMax(projAxis, &box1Min, &box1Max);
//     pBoxCollider2->GetWorldMinMax(projAxis, &box2Min, &box2Max);
//     if (box1Max < box2Min || box2Max < box1Min)
//         return false;
//     
//     return true;
// }

void Processor::WorldTransformPolygonVertices(PolygonCollider& polygonCollider)
{
    math::Matrix3x2R tMat(math::Matrix3x2R::Rotation(polygonCollider.m_orientation) * math::Matrix3x2R::Translation(polygonCollider.m_position));

    for (uint32_t index = 0; index < polygonCollider.m_vertexCount; index++)
    {
        polygonCollider.m_pWorldVertices[index] = polygonCollider.m_pVertices[index] * tMat;
    }
}

/*
const math::Vector2 Processor::MinkowskiSupport(const PolygonCollider& polygonColliderA, const PolygonCollider& polygonColliderB, const math::Vector2& direction)
{
    return polygonColliderA.SupportPoint(direction) - polygonColliderB.SupportPoint(-direction);
}
*/

void Processor::Step(class RigidBody& rigidBody)
{
    // rigidBody.AddForce(math::Vector2(real(0.0), real(-100.0)));

    rigidBody.m_linearVelocity += (rigidBody.m_force * rigidBody.m_invMass) * FIXED_DELTA_TIME;
    rigidBody.m_pos += rigidBody.m_linearVelocity * FIXED_DELTA_TIME;

    // ���ӵ� ����
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

    pManifold->pRigidBodyA = circleColliderA.m_pRigidBody;
    pManifold->pRigidBodyB = circleColliderB.m_pRigidBody;

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
    Simplex simplex(circleCollider, polygonCollider);
    Simplex::State state;

    while ((state = simplex.EvolveSimplex()) == Simplex::State::Evolving);

    if (state == Simplex::State::Intersecting)
    {
        printf("colliding!\n");
    }
    else
    {
        printf("Not colliding\n");
    }

    return static_cast<bool>(state);        // Intersecting = 1(true), Not Intersecting = 0(false)
}

bool Processor::CheckCollisionPolygonToPolygon(Manifold* pManifold, const PolygonCollider& polygonColliderA, const PolygonCollider& polygonColliderB)
{
    Simplex simplex(polygonColliderA, polygonColliderB);
    Simplex::State state;

    while ((state = simplex.EvolveSimplex()) == Simplex::State::Evolving);

    if (state == Simplex::State::Intersecting)
    {
        printf("colliding!\n");
    }
    else
    {
        printf("Not colliding\n");
    }

    return static_cast<bool>(state);        // Intersecting = 1(true), Not Intersecting = 0(false)
}
