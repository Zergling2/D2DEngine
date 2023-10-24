#pragma once

#include "Common\Math2D.h"

namespace ip
{
	struct Manifold
	{
		class Collider* pColliderA;
		class Collider* pColliderB;
		math::Vector2 collisionNormal;
		real penetrationDepth;
	};
}
