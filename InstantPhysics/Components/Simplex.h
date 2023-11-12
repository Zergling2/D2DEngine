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
		inline Simplex()
			: m_pColliderA(nullptr)
			, m_pColliderB(nullptr)
		{
			m_vertices.reserve(size_t(16));
		}
		inline void Initialize(const class Collider* a, const class Collider* b)
		{
			m_pColliderA = a;
			m_pColliderB = b;
			m_vertices.clear();
		}
		State EvolveSimplex();
		// void ComputeManifold(struct Manifold& manifold);
	private:
		void ConvertVerticesToCCW();			// 초기 Simplex 반시계로 정렬 -> 이후 Simplex 확장 시 정점들 모두 자동으로 반시계
		void FindClosestEdge(math::Vector2* pClosestEdgeNormal, real* pClosestEdgeDistance, uint16_t* pClosestEdgeIndex);
	private:
		alignas(64)
		const class Collider* m_pColliderA;
		const class Collider* m_pColliderB;
		math::Vector2 m_direction;					// search direction vector
		std::vector<math::Vector2> m_vertices;		// simplex vertices

		// =====================================
		// closest edge caching 알고리즘 추가 필요
		// ...
		// ...
		// ...
		// =====================================
	};
}
