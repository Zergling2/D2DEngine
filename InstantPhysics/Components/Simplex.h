#pragma once

#include "Common\Math2D.h"
#include <vector>

namespace ip
{
	class Simplex
	{
	public:
		enum class State
		{
			NotIntersecting,
			Intersecting,
			Evolving
		};
	public:
		inline Simplex(const class Collider& colliderA, const class Collider& colliderB)
			: m_colliderA(colliderA)
			, m_colliderB(colliderB)
		{
		}
		State EvolveSimplex();
	private:
		bool AddSupportPoint();
	private:
		const class Collider& m_colliderA;
		const class Collider& m_colliderB;
		math::Vector2 m_direction;					// search direction vector
		std::vector<math::Vector2> m_vertices;		// simplex vertices
	};
}
