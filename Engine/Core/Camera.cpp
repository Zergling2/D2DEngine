#include "Camera.h"
#include "Core\Engine.h"

using namespace D2DEngine;

Camera::Camera(float fovWidth, float fovHeight, float zoom)
	: m_pos{0.0f, 0.0f}
	, m_scale{1.0f, 1.0f}
	, m_rotation(degree(0.0f))
	, m_fovWidth(fovWidth)
	, m_fovHeight(fovHeight)
	, m_zoom(zoom)
	, m_rotateCounter(0)
	, m_setRotCounter(0)
{
}

Camera::~Camera()
{
}

const D2D1::Matrix3x2F Camera::GetViewMatrix() const
{
	return
		D2D1::Matrix3x2F::Translation(-m_pos.x, -m_pos.y) *
		D2D1::Matrix3x2F::Scale(m_zoom, m_zoom) *
		D2D1::Matrix3x2F::Rotation(-m_rotation);
}

const D2D1::Matrix3x2F Camera::GetProjectionMatrix() const
{
	D2D1_SIZE_F rtSize = Engine::GetInstance().GetRenderTarget()->GetSize();

	// Scale에서 x축을 기준으로 뒤집기 행렬(스크린좌표 y 증가 방향이 world랑 반대이므로)
	return
		D2D1::Matrix3x2F::Scale(rtSize.width / m_fovWidth, -rtSize.height / m_fovHeight) *
		D2D1::Matrix3x2F::Translation(rtSize.width / 2.0f, rtSize.height / 2.0f);
}

void Camera::Rotate(degree d)
{
	m_rotateCounter++;
	m_rotation += d;
	if (m_rotateCounter & uint16_t(0x0200))
	{
		m_rotateCounter = 0;
		m_rotation = std::fmod(m_rotation, degree(360.0));
	}
}

void Camera::SetRotation(degree d)
{
	m_setRotCounter++;
	m_rotation += d;
	if (m_setRotCounter & uint16_t(0x0400))
	{
		m_setRotCounter = 0;
		m_rotation = std::fmod(m_rotation, degree(360.0));
	}
}
