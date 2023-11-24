#include "Camera.h"
#include "Core\Engine.h"

using namespace D2DEngine;

Camera::Camera(float fovWidth, float fovHeight)
	: m_pos{0.0f, 0.0f}
	, m_scale{1.0f, 1.0f}
	, m_rotation(degree(0.0f))
	, m_fovWidth(fovWidth)
	, m_fovHeight(fovHeight)
{
}

Camera::~Camera()
{
}

const ip::math::Vector2 Camera::ScreenToWorldPoint(const D2D_VECTOR_2F mousePosition)
{
	return ip::math::Vector2::zero;
}

const D2D1::Matrix3x2F Camera::GetViewMatrix() const
{
	return
		D2D1::Matrix3x2F::Translation(-m_pos.x, -m_pos.y) *
		D2D1::Matrix3x2F::Scale(m_scale.x, m_scale.y) *
		D2D1::Matrix3x2F::Rotation(-m_rotation);
}

const D2D1::Matrix3x2F Camera::GetProjectionMatrix() const
{
	D2D1_SIZE_F rtSize = Engine::GetInstance().GetRenderTarget()->GetSize();

	// Scale에서 x축을 기준으로 뒤집기 행렬(스크린좌표 y 증가 방향이 world랑 반대이므로)
	return
		D2D1::Matrix3x2F::Scale(rtSize.width / m_fovWidth, -rtSize.height / m_fovHeight) *
		D2D1::Matrix3x2F::Translation(rtSize.width * 0.5f, rtSize.height * 0.5f);
}

void Camera::Rotate(degree d)
{
	real r = std::fmod(m_rotation + d, 360.0f);
	if (r < real(0.0))
		r += 360.0f;

	m_rotation = r;
}

void Camera::SetRotation(degree d)
{
	real r = std::fmod(d, 360.0f);
	if (r < real(0.0))
		r += 360.0f;

	m_rotation = r;
}
