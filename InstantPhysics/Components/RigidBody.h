#pragma once

#include "Common\Math2D.h"

namespace ip
{
	enum BodyType
	{
		Unknown,
		Static,
		Kinematic,
		Dynamic
	};

	class RigidBody
	{
	public:
		inline void InitializeRigidBody() { m_type = BodyType::Unknown; }
		void InitializeRigidBody(
			BodyType type,
			real mass,
			real inertia,
			real bounce,
			real staticFriction,
			real dynamicFriction,
			real gravityScale
		);
	public:
		BodyType m_type;
		real m_mass;
		real m_invMass;

		math::Vector2 m_linearVelocity;
		real m_angularVelocity;

		real m_invInertia;
		math::Vector2 m_force;
		real m_torque;

		real m_bounce;
		real m_staticFriction;
		real m_dynamicFriction;
		real m_gravityScale;
	};
}
