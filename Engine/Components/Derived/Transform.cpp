#include "Transform.h"

using namespace D2DEngine;

void Transform::GetAxis(const Transform& transform, D2D_VECTOR_2F* xAxis, D2D_VECTOR_2F* yAxis)
{
	float rad = Mathf::DegreeToRadian(transform.m_orientation);
	xAxis->x = std::cos(rad);
	xAxis->y = std::sin(rad);
	yAxis->y = xAxis->x;		// sin(x + 90') = cos(x);
	yAxis->x = -xAxis->y;		// cos(x + 90') = -sin(x);
}
