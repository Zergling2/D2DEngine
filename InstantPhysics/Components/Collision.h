#pragma once

#include "Common\Math2D.h"

namespace ip
{
	class Collision
	{
	public:
		math::Vector2 m_impulse;
		math::Point2 m_contactPoint[2];
		math::Vector2 m_normal;
		real m_separation;
	};
}
