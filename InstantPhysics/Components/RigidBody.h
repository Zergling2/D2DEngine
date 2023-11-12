#pragma once

#include "Common\Math2D.h"

namespace ip
{
	class RigidBody
	{
	public:
		inline RigidBody(
			class Collider& collider,
			real mass,
			real inertia,
			real restitution,
			real staticFriction,
			real dynamicFriction,
			bool isStatic
		)
			: m_collider(collider)
			, m_mass(mass)
			, m_invMass(mass <= real(0.0) ? real(0.0) : (real(1.0) / mass))
			, m_force(real(0.0), real(0.0))
			, m_linearVelocity(real(0.0), real(0.0))
			, m_invInertia(inertia <= real(0.0) ? real(0.0) : (real(1.0) / inertia))
			, m_torque(real(0.0))
			, m_angularVelocity(real(0.0))
			, m_bounce(restitution)
			, m_staticFriction(staticFriction)
			, m_dynamicFriction(dynamicFriction)
			, m_isStatic(isStatic)
		{
			if (isStatic)
			{
				m_invMass = real(0.0);
				m_invInertia = real(0.0);
			}
		}
		inline ~RigidBody()
		{
		}
	public:
		class Collider& m_collider;
		real m_mass;
		// ======================
		real m_invMass;				// 1 / Mass
		math::Vector2 m_force;
		math::Vector2 m_linearVelocity;
		// ======================
		real m_invInertia;			// 1 / Inertia
		real m_torque;
		real m_angularVelocity;
		// ======================
		real m_bounce;
		real m_staticFriction;
		real m_dynamicFriction;
		bool m_isStatic;
	};
}
