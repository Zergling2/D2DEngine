#include "Processor.h"
#include "Components\RigidBody.h"
#include "Core\Acc\AABB.h"
#include "Components\CircleCollider.h"
#include "Components\BoxCollider.h"

using namespace ip;

Processor::Processor()
{
}

Processor::~Processor()
{
}

bool Processor::CheckAABBCollision(const AABB* const pAABB1, const AABB* const pAABB2)
{
    if (pAABB1->m_max.x < pAABB2->m_min.x || pAABB1->m_max.y < pAABB2->m_min.y ||
        pAABB1->m_min.x > pAABB2->m_max.x || pAABB1->m_min.y > pAABB2->m_max.y)
    {
        return false;
    }
    return true;
}

bool Processor::CheckCircleToCircleCollision(Manifold* pManifold, const class RigidBody* const pCircle1, const class RigidBody* const pCircle2)
{
    math::Vector2 v = pCircle2->m_pos - pCircle1->m_pos;
    real distancePower2 = v.x * v.x + v.y * v.y;
    real radiusSum = reinterpret_cast<const CircleCollider* const>(pCircle1->m_pCollider)->m_radius +
        reinterpret_cast<const CircleCollider* const>(pCircle2->m_pCollider)->m_radius;
    
    if (distancePower2 > radiusSum * radiusSum)
        return false;

    pManifold->pRigidBodyA = const_cast<RigidBody*>(pCircle1);
    pManifold->pRigidBodyB = const_cast<RigidBody*>(pCircle2);

    if (distancePower2 != real(0.0))
    {
        real distance = std::sqrt(distancePower2);
        pManifold->penetrationDepth = radiusSum - distance;
        pManifold->collisionNormal = v / distance;
    }
    else
    {
        pManifold->penetrationDepth = reinterpret_cast<const CircleCollider* const>(pCircle1->m_pCollider)->m_radius;
        pManifold->collisionNormal = math::Vector2(real(1.0), real(0.0));
    }
    return true;
}

bool Processor::CheckBoxToCircleCollision(const class RigidBody* const pBox, const class RigidBody* const pCircle)
{
    const BoxCollider* const pBoxCollider = reinterpret_cast<const BoxCollider* const>(pBox->m_pCollider);
    const CircleCollider* const pCircleCollider = reinterpret_cast<const CircleCollider* const>(pCircle->m_pCollider);

    math::Vector2 projAxis;
    real boxMin;
    real boxMax;
    real circleMin;
    real circleMax;

    projAxis = { pBoxCollider->m_points[1].y - pBoxCollider->m_points[0].y, -(pBoxCollider->m_points[1].x - pBoxCollider->m_points[0].x) };
    pBoxCollider->GetMinMax(projAxis, &boxMin, &boxMax);
    pCircleCollider->GetMinMax(pCircle->m_pos, projAxis, &circleMin, &circleMax);
    if (boxMin > circleMax || circleMin > boxMax)
        return false;

    projAxis = { pBoxCollider->m_points[2].y - pBoxCollider->m_points[1].y, -(pBoxCollider->m_points[2].x - pBoxCollider->m_points[1].x) };
    pBoxCollider->GetMinMax(projAxis, &boxMin, &boxMax);
    pCircleCollider->GetMinMax(pCircle->m_pos, projAxis, &circleMin, &circleMax);
    if (boxMin > circleMax || circleMin > boxMax)
        return false;

    return true;
}

bool Processor::CheckBoxToBoxCollision(Manifold* pManifold, const class RigidBody* const pBox1, const class RigidBody* const pBox2)
{
    const BoxCollider* const pBoxCollider1 = reinterpret_cast<const BoxCollider* const>(pBox1->m_pCollider);
    const BoxCollider* const pBoxCollider2 = reinterpret_cast<const BoxCollider* const>(pBox2->m_pCollider);

    math::Vector2 v = pBox2->m_pos - pBox1->m_pos;
    math::Vector2 projAxis;
    real box1Min;
    real box1Max;
    real box2Min;
    real box2Max;

    // 1
    projAxis = { pBoxCollider1->m_points[1].y - pBoxCollider1->m_points[0].y, -(pBoxCollider1->m_points[1].x - pBoxCollider1->m_points[0].x) };
    pBoxCollider1->GetMinMax(projAxis, &box1Min, &box1Max);
    pBoxCollider2->GetMinMax(projAxis, &box2Min, &box2Max);
    if (box1Min > box2Max || box2Min > box1Max)
        return false;

    // 2
    projAxis = { pBoxCollider1->m_points[2].y - pBoxCollider1->m_points[1].y, -(pBoxCollider1->m_points[2].x - pBoxCollider1->m_points[1].x) };
    pBoxCollider1->GetMinMax(projAxis, &box1Min, &box1Max);
    pBoxCollider2->GetMinMax(projAxis, &box2Min, &box2Max);
    if (box1Min > box2Max || box2Min > box1Max)
        return false;

    // 3
    projAxis = { pBoxCollider2->m_points[1].y - pBoxCollider2->m_points[0].y, -(pBoxCollider2->m_points[1].x - pBoxCollider2->m_points[0].x) };
    pBoxCollider1->GetMinMax(projAxis, &box1Min, &box1Max);
    pBoxCollider2->GetMinMax(projAxis, &box2Min, &box2Max);
    if (box1Min > box2Max || box2Min > box1Max)
        return false;

    // 4
    projAxis = { pBoxCollider2->m_points[2].y - pBoxCollider2->m_points[1].y, -(pBoxCollider2->m_points[2].x - pBoxCollider2->m_points[1].x) };
    pBoxCollider1->GetMinMax(projAxis, &box1Min, &box1Max);
    pBoxCollider2->GetMinMax(projAxis, &box2Min, &box2Max);
    if (box1Min > box2Max || box2Min > box1Max)
        return false;

    return true;
}
