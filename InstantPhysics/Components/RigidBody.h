#pragma once

#include "Common\Math2D.h"

namespace ip
{
	class RigidBody
	{
	public:
		inline RigidBody(
			class Collider* pCollider,
			real mass,
			real inertia,
			real restitution,
			bool isStatic
		)
			: m_pCollider(pCollider)
			, m_mass(mass)
			, m_invMass(mass <= real(0.0) ? real(0.0) : (real(1.0) / mass))
			, m_linearVelocity(real(0.0), real(0.0))
			, m_force(real(0.0), real(0.0))
			, m_invInertia(inertia <= real(0.0) ? real(0.0) : (real(1.0) / inertia))
			, m_restitution(restitution)
			, m_isStatic(isStatic)
		{
			if (isStatic)
				m_invMass = real(0.0);
		}
		inline ~RigidBody()
		{
		}
	public:
		class Collider* m_pCollider;
		real m_mass;
		real m_invMass;				// 1 / Mass
		math::Vector2 m_linearVelocity;
		math::Vector2 m_force;
		real m_invInertia;			// 1 / Inertia
		real m_restitution;
		bool m_isStatic;
	};
}
