#pragma once

#include "Common\Math2D.h"
#include "Components\Collider.h"

namespace ip
{
	class RigidBody
	{
	public:
		inline RigidBody(
			const math::Vector2& pos,
			real orientation,
			real mass,
			real inertia,
			real restitution
		)
			: m_pos(pos)
			, m_orientation(orientation)
			, m_invMass(mass <= real(0.0) ? real(0.0) : (real(1.0) / mass))
			, m_linearVelocity(real(0.0), real(0.0))
			, m_force(real(0.0), real(0.0))
			, m_invInertia(inertia <= real(0.0) ? real(0.0) : (real(1.0) / inertia))
			, m_restitution(restitution)
		{
		}
		inline ~RigidBody()
		{
		}
		inline const math::Vector2 GlobalToLocalVector(const math::Vector2 globalVec) const
		{
			return globalVec * math::Matrix3x2R::Rotation(-m_orientation);
		}
		inline const math::Vector2 LocalToGlobalVector(const math::Vector2 localVec) const
		{
			return localVec * math::Matrix3x2R::Rotation(m_orientation);
		}
	public:
		math::Vector2 m_pos;
		real m_orientation;
		real m_invMass;				// 1 / Mass
		math::Vector2 m_linearVelocity;
		math::Vector2 m_force;
		real m_invInertia;			// 1 / Inertia
		real m_restitution;
	};
}
