#include "RigidBody.h"

using namespace ip;

void RigidBody::InitializeRigidBody(BodyType type, real mass, real inertia, real bounce, real staticFriction, real dynamicFriction, real gravityScale)
{
	m_type = type;
	m_force = math::Vector2::zero;
	m_linearVelocity = math::Vector2::zero;
	m_torque = real(0.0);
	m_angularVelocity = real(0.0);
	m_bounce = bounce;
	m_staticFriction = staticFriction;
	m_dynamicFriction = dynamicFriction;
	m_gravityScale = gravityScale;

	switch (type)
	{
	case BodyType::Unknown:
	case BodyType::Static:
	case BodyType::Kinematic:
		m_invMass = real(0.0);
		m_mass = real(0.0);
		m_invInertia = real(0.0);
		break;
	case BodyType::Dynamic:
		m_invMass = mass <= real(0.0) ? real(1.0) : (real(1.0) / mass);
		m_mass = mass <= real(0.0) ? real(1.0) : mass;
		m_invInertia = inertia <= real(0.0) ? real(0.0) : (real(1.0) / inertia);
		break;
	}
}