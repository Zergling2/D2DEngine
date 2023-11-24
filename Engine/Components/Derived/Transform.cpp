#include "Transform.h"

using namespace D2DEngine;

void Transform::GetAxis(const Transform& transform, D2D_VECTOR_2F* xAxis, D2D_VECTOR_2F* yAxis)
{
	float rad = Mathf::DegreeToRadian(transform.m_rotation);
	xAxis->x = std::cos(rad);
	xAxis->y = std::sin(rad);
	yAxis->y = xAxis->x;		// sin(x + 90') = cos(x);
	yAxis->x = -xAxis->y;		// cos(x + 90') = -sin(x);
}

void Transform::Rotate(float degree)
{
	float r = std::fmod(m_rotation + degree, 360.0f);
	if (r < real(0.0))
		r += 360.0f;
	m_rotation = r;
}

void Transform::SetRotation(float degree)
{
	float r = std::fmod(degree, 360.0f);
	if (r < real(0.0))
		r += 360.0f;
	m_rotation = r;
}
