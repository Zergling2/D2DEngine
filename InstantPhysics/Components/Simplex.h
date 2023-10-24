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
		inline Simplex(const class Collider* pColliderA, const class Collider* pColliderB)
			: m_pColliderA(pColliderA)
			, m_pColliderB(pColliderB)
		{
			m_vertices.reserve(size_t(8));
		}
		State EvolveSimplex();
		void ComputeManifold(struct Manifold* pManifold);
	private:
		void ConvertVerticesToCCW();			// �ʱ� Simplex �ݽð�� ���� -> ���� Simplex Ȯ�� �� ������ ��� �ڵ����� �ݽð�
		void FindClosestEdge(math::Vector2* pClosestEdgeNormal, real* pClosestEdgeDistance, uint32_t* pClosestEdgeIndex);
	private:
		const class Collider* m_pColliderA;
		const class Collider* m_pColliderB;
		math::Vector2 m_direction;					// search direction vector
		std::vector<math::Vector2> m_vertices;		// simplex vertices

		// =====================================
		// closest edge caching �˰��� �߰� �ʿ�
		// ...
		// ...
		// ...
		// =====================================
	};
}
