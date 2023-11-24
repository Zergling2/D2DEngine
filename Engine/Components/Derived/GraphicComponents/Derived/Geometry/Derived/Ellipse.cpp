#include "Ellipse.h"
#include "Core\Engine.h"

using namespace D2DEngine::Graphic;

Ellipse::Ellipse(GameObject* pGameObj, const D2D1_ELLIPSE& shape)
	: Geometry(pGameObj)
	, m_pShape(nullptr)
{
	// Create ellipse geometry
	if (FAILED(Engine::GetInstance().GetFactory()->CreateEllipseGeometry(shape, &m_pShape)))
		OutputDebugString(_T("CreatePathGeometry() FAILED"));
}

Ellipse::~Ellipse()
{
	SafeRelease(m_pShape);
}
