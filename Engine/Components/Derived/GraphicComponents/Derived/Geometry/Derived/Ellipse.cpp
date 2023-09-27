#include "Ellipse.h"
#include "Core\Engine.h"

using namespace D2DEngine;

Ellipse::Ellipse(GameObject* pGameObj, const D2D1_ELLIPSE& shape)
	: Geometry(pGameObj)
	, m_bt(BrushType::SolidColorBrush)
	, m_pBrush(nullptr)
	, m_pShape(nullptr)
{
	// Create brush
	if (FAILED(Engine::GetInstance().GetRenderTarget()->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.75f, 0.5f, 1.0f), &m_pBrush)))
		OutputDebugString(_T("CreateSolidColorBrush() FAILED"));

	// Create ellipse geometry
	if (FAILED(Engine::GetInstance().GetFactory()->CreateEllipseGeometry(shape, &m_pShape)))
		OutputDebugString(_T("CreatePathGeometry() FAILED"));
}

Ellipse::~Ellipse()
{
	SafeRelease(m_pBrush);
	SafeRelease(m_pShape);
}

void Ellipse::Render()
{
	if (!this->CheckEnable() || !this->m_pShape)
		return;

	// ����: ID2D1Brush*
	// ���� ������ ������ ������ ����ũ�̰ų� ������ ����ũ�� ���� ��� NULL �Դϴ�.
	// ������ ����ũ(opacityBrush �Ű� ����)�� ������ ��� �귯�ô� x �� y Ȯ�� ��尡 D2D1_EXTEND_MODE_CLAMP ������
	//  ID2D1BitmapBrush���� �մϴ�.�ڼ��� ������ ���� ������ �����ϼ���.

	Engine::GetInstance().GetRenderTarget()->FillGeometry(m_pShape, m_pBrush, NULL);
	Engine::GetInstance().GetRenderTarget()->DrawGeometry(m_pShape, m_pBrush, 1.0f, NULL);
}
