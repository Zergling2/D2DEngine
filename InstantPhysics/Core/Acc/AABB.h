#pragma once

#include "Common\Math2D.h"

// C:/Users/admin/Documents/��ǻ�Ͱ���/��������/AABBForTransformedAABB.pdf

namespace ip
{
	struct AABB
	{
	public:
		// Collider�κ��� AABB ���
		// Parameters
		// 1. const float vertices[]: Object Space Vertices!!
		// 2. int count: Number of vertices
		inline AABB(real minX, real minY, real maxX, real maxY)
			: m_min(minX, minY)
			, m_max(maxX, maxY)
		{
		}
		inline AABB(const math::Vector2& min, const math::Vector2& max)
			: m_min(min)
			, m_max(max)
		{
		}
		math::Vector2 m_min;
		math::Vector2 m_max;
	};
}
