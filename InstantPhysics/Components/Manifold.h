#pragma once

#include "Common\Math2D.h"

namespace ip
{
	struct Manifold
	{
		class Collider* pReference;
		class Collider* pIncident;
		math::Vector2 collisionNormal;
		real penetrationDepth;
		math::Vector2 contact[2];
		uint32_t contactCount;
	};
}
