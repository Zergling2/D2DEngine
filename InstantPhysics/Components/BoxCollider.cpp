#include "BoxCollider.h"

using namespace ip;

void BoxCollider::GetMinMax(const math::Vector2& projVector, real* pMin, real* pMax) const
{
	real values[4U];

	values[0] = math::Vector2::Dot(m_points[0], projVector);
	values[1] = math::Vector2::Dot(m_points[1], projVector);
	values[2] = math::Vector2::Dot(m_points[2], projVector);
	values[3] = math::Vector2::Dot(m_points[3], projVector);

	real min = values[0];
	real max = values[0];
	real val;
	
	val = values[1];
	if (min > val)
		min = val;
	if (max < val)
		max = val;
	val = values[2];
	if (min > val)
		min = val;
	if (max < val)
		max = val;
	val = values[3];
	if (min > val)
		min = val;
	if (max < val)
		max = val;

	*pMin = min;
	*pMax = max;
}
