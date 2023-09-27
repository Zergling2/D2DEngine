#include "Rectangle.h"
#include "Core\Engine.h"

using namespace D2DEngine;

Rectangle::Rectangle(GameObject* pGameObj, const D2D_RECT_F& shape)
	: Geometry(pGameObj)
	, m_bt(BrushType::SolidColorBrush)
	, m_pBrush(nullptr)
	, m_pShape(nullptr)
{
	// Create brush
	if (FAILED(Engine::GetInstance().GetRenderTarget()->CreateSolidColorBrush(D2D1::ColorF(0.75f, 0.5f, 1.0f, 1.0f), &m_pBrush)))
		OutputDebugString(_T("CreateSolidColorBrush() FAILED"));

	if (FAILED(Engine::GetInstance().GetFactory()->CreateRectangleGeometry(shape, &m_pShape)))
		OutputDebugString(_T("CreateRectangleGeometry() FAILED"));
}

Rectangle::~Rectangle()
{
	SafeRelease(m_pBrush);
	SafeRelease(m_pShape);
}

void Rectangle::Render()
{
	if (!this->CheckEnable() || !this->m_pShape)
		return;

	Engine::GetInstance().GetRenderTarget()->FillGeometry(m_pShape, m_pBrush, NULL);
	Engine::GetInstance().GetRenderTarget()->DrawGeometry(m_pShape, m_pBrush, 1.0f, NULL);
}
