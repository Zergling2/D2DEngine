#include "Rectangle.h"
#include "Core\Engine.h"

using namespace D2DEngine::Graphic;

Rectangle::Rectangle(GameObject* pGameObj, const D2D_RECT_F& shape)
	: Geometry(pGameObj)
	, m_pShape(nullptr)
{
	if (FAILED(Engine::GetInstance().GetFactory()->CreateRectangleGeometry(shape, &m_pShape)))
		OutputDebugString(_T("CreateRectangleGeometry() FAILED"));
}

Rectangle::~Rectangle()
{
	SafeRelease(m_pShape);
}
