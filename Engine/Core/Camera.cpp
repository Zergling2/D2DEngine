#include "Camera.h"
#include "Core\Engine.h"

using namespace D2DEngine;

Camera::Camera(float fovWidth, float fovHeight, float zoom)
	: m_pos{0.0f, 0.0f}
	, m_scale{1.0f, 1.0f}
	, m_orientation(Degree(0.0f))
	, m_fovWidth(fovWidth)
	, m_fovHeight(fovHeight)
	, m_zoom(zoom)
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
		D2D1::Matrix3x2F::Rotation(-m_orientation);
}

const D2D1::Matrix3x2F Camera::GetProjectionMatrix() const
{
	D2D1_SIZE_F rtSize = Engine::GetInstance().GetRenderTarget()->GetSize();

	// Scale���� x���� �������� ������ ���(��ũ����ǥ y ���� ������ world�� �ݴ��̹Ƿ�)
	return
		D2D1::Matrix3x2F::Scale(rtSize.width / m_fovWidth, -rtSize.height / m_fovHeight) *
		D2D1::Matrix3x2F::Translation(rtSize.width / 2.0f, rtSize.height / 2.0f);
}

void Camera::Rotate(Degree degree)
{
	if (degree >= 360.0f || degree <= -360.0f)
		degree /= 360.0f;
	m_orientation += degree;
	if (m_orientation >= 360.0f) m_orientation -= 360.0f;
	else if (m_orientation <= -360.0f) m_orientation += 360.0f;
}
