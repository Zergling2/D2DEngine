#pragma once

#include "Common\Math2D.h"

namespace ip
{
	struct Manifold
	{
		class RigidBody* pRigidBodyA;
		class RigidBody* pRigidBodyB;
		math::Vector2 collisionNormal;
		real penetrationDepth;
	};
}
