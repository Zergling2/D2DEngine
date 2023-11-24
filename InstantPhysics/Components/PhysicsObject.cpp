#include "PhysicsObject.h"
#include "Common\IPMacro.h"
#include "Components\PhysicsWorld.h"
#include "Components\CircleCollider.h"
#include "Components\CapsuleCollider.h"
#include "Components\HullCollider.h"
#include "Components\RigidBody.h"

using namespace ip;

PhysicsObject::PhysicsObject()
    : m_isDead(false)
    , m_pCollider(nullptr)
    , m_pPhysWorld(nullptr)
    , m_position(real(0.0), real(0.0))
    , m_scale(real(1.0), real(1.0))
    , m_rotation(real(0.0))
{
    m_rigidBody.InitializeRigidBody();
}

PhysicsObject::~PhysicsObject()
{
    RemoveCollider();
    RemoveRigidBody();
}

void PhysicsObject::StaticReady()
{
    m_pCollider->UpdateCollider();
}

bool PhysicsObject::CreateCircleCollider(CollisionLayer layer, real radius)
{
    RemoveCollider();

    CircleCollider* pNewCollider = new (std::nothrow)CircleCollider(this, layer);
    if (!pNewCollider)
        return false;

    if (pNewCollider->Initialize(radius) == false)
    {
        delete pNewCollider;
        return false;
    }

    m_pCollider = pNewCollider;
    return true;
}

bool PhysicsObject::CreateCapsuleCollider(CollisionLayer layer, real radius, real length)
{
    RemoveCollider();

    CapsuleCollider* pNewCollider = new (std::nothrow)CapsuleCollider(this, layer);
    if (!pNewCollider)
        return false;

    if (pNewCollider->Initialize(radius, length) == false)
    {
        delete pNewCollider;
        return false;
    }

    m_pCollider = pNewCollider;
    return true;
}

bool PhysicsObject::CreateBoxCollider(CollisionLayer layer, real width, real height)
{
    RemoveCollider();

    math::Vector2 buffer[4];        // LB, RB, RT, LT ¼ø¼­

    real halfWidth = width * real(0.5);
    real halfHeight = height * real(0.5);
    buffer[0].x = -halfWidth;
    buffer[0].y = -halfHeight;
    buffer[1].x = +halfWidth;
    buffer[1].y = -halfHeight;
    buffer[2].x = +halfWidth;
    buffer[2].y = +halfHeight;
    buffer[3].x = -halfWidth;
    buffer[3].y = +halfHeight;

    HullCollider* pNewCollider = new (std::nothrow)HullCollider(this, layer);
    if (!pNewCollider)
        return false;

    if (pNewCollider->Initialize(buffer, 4) == false)
    {
        delete pNewCollider;
        return false;
    }

    m_pCollider = pNewCollider;
    return true;
}

bool PhysicsObject::CreateHullCollider(CollisionLayer layer, math::Vector2 convexCCW[], uint16_t vertexCount)
{
    RemoveCollider();

    HullCollider* pNewCollider = new (std::nothrow)HullCollider(this, layer);
    if (!pNewCollider)
        return false;

    if (pNewCollider->Initialize(convexCCW, vertexCount) == false)
    {
        delete pNewCollider;
        return false;
    }

    m_pCollider = pNewCollider;
    return true;
}

void PhysicsObject::RemoveCollider()
{
    if (!m_pCollider)
        return;

    if (m_pPhysWorld)
        m_pPhysWorld->RemovePhysicsObject(this);

    IPSafeDelete(m_pCollider);
}

bool PhysicsObject::CreateRigidBody(BodyType type, real mass, real inertia, real bounce, real staticFriction, real dynamicFriction, real gravityScale)
{
    m_rigidBody.InitializeRigidBody(type, mass, inertia, bounce, staticFriction, dynamicFriction, gravityScale);
    return true;
}

void PhysicsObject::RemoveRigidBody()
{
    m_rigidBody.m_type = Unknown;
}

void PhysicsObject::Translate(const math::Vector2& translation, Space cs)
{
    real cosine;
    real sine;
    math::Vector2 rt;

    switch (cs)
    {
    case Space::World:
        m_position += translation;
        break;
    case Space::Local:
        cosine = std::cos(m_rotation);
        sine = std::sin(m_rotation);
        rt = math::Vector2(translation.x * cosine + translation.y * sine, translation.y * cosine - translation.x * sine);
        m_position += rt;
        break;
    default:
        break;
    }
}

void PhysicsObject::Translate(real x, real y, Space cs)
{
    real cosine;
    real sine;
    math::Vector2 rt;

    switch (cs)
    {
    case Space::World:
        m_position.x += x;
        m_position.y += y;
        break;
    case Space::Local:
        cosine = std::cos(m_rotation);
        sine = std::sin(m_rotation);
        rt = math::Vector2(x * cosine + y * sine, y * cosine - x * sine);
        m_position += rt;
        break;
    default:
        break;
    }
}

void PhysicsObject::AddRelativeForce(const math::Vector2 force)
{
    real cosine = std::cos(m_rotation);
    real sine = std::sin(m_rotation);
    math::Vector2 rf(force.x * cosine - force.y * sine, force.y * cosine + force.x * sine);
    m_rigidBody.m_force += rf;
}

void PhysicsObject::AddRelativeForce(real x, real y)
{
    real cosine = std::cos(m_rotation);
    real sine = std::sin(m_rotation);
    math::Vector2 rf(x * cosine - y * sine, y * cosine + x * sine);
    m_rigidBody.m_force += rf;
}

void PhysicsObject::SetRotation(real angle)
{
    real r = std::fmod(angle, math::PIx2);
    if (r < real(0.0))
        r += math::PIx2;

    m_rotation = r;
}

void PhysicsObject::Rotate(real angle)
{
    real r = std::fmod(m_rotation + angle, math::PIx2);
    if (r < real(0.0))
        r += math::PIx2;

    m_rotation = r;
}

void PhysicsObject::OnTriggerEnter2D(Collider& other)
{
    return;
}

void PhysicsObject::OnTriggerStay2D(Collider& other)
{
    return;
}

void PhysicsObject::OnTriggerExit2D(Collider& other)
{
    return;
}

void PhysicsObject::OnCollisionEnter2D(Collision& other)
{
    return;
}

void PhysicsObject::OnCollisionStay2D(Collision& other)
{
    return;
}

void PhysicsObject::OnCollisionExit2D(Collision& other)
{
    return;
}
