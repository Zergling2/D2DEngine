#pragma once

#include "Common\Math2D.h"

// C:/Users/admin/Documents/컴퓨터공학/물리엔진/AABBForTransformedAABB.pdf

namespace ip
{
	struct AABB
	{
	public:
		inline AABB()
		{
		}
		inline AABB(real minX, real minY, real maxX, real maxY)
			: leftBottom(minX, minY)
			, rightTop(maxX, maxY)
		{
		}
		inline AABB(const math::Vector2& min, const math::Vector2& max)
			: leftBottom(min)
			, rightTop(max)
		{
		}
		math::Vector2 leftBottom;		// x_min, y_min
		math::Vector2 rightTop;			// x_max, y_max
	};
}
