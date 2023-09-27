#pragma once

#include "Common\Math2D.h"

namespace ip
{
	class RigidBody
	{
	public:
		inline RigidBody(
			const math::Vector2& pos,
			real orientation,
			real mass,
			const math::Vector2& velocity,
			real inertia,
			class Collider* pCollider
		)
			: m_pos(pos)
			, m_orientation(orientation)
			, m_massInverse(mass <= real(0.0) ? real(0.0) : (real(1.0) / mass))
			, m_velocity(velocity)
			, m_inertiaInverse(inertia <= real(0.0) ? real(0.0) : (real(1.0) / inertia))
			, m_pCollider(pCollider)
		{
		}
		inline ~RigidBody() {}		// Collider pointer Ã³¸®??
	public:
		math::Vector2 m_pos;
		real m_orientation;
		real m_massInverse;				// 1 / Mass
		math::Vector2 m_velocity;
		real m_inertiaInverse;			// 1 / Inertia
		class Collider* m_pCollider;
	};
}
