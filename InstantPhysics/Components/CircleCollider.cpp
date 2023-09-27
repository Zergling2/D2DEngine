#include "CircleCollider.h"

using namespace ip;

void CircleCollider::GetMinMax(const math::Vector2& center, const math::Vector2& projVector, real* pMin, real* pMax) const
{
	real centerProj = math::Vector2::Dot(center, projVector);

	*pMin = centerProj - m_radius;
	*pMax = centerProj + m_radius;
}
