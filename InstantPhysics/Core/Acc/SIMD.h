#pragma once

#include "Common\Math2D.h"

// C:/Users/admin/Documents/컴퓨터공학/물리엔진/AABBForTransformedAABB.pdf

namespace ip
{
	typedef real* ip_simd_aligned_addr;
	class SIMD
	{
	public:
		static void TransformVertices(math::Vector2 oldPos, real oldRad, math::Vector2 newPos, real newRad, ip_simd_aligned_addr xxxx, ip_simd_aligned_addr yyyy, uint16_t vertexCount);
	};
}
