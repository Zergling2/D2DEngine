#include "RoundedRectangle.h"
#include "Core\Engine.h"

using namespace D2DEngine;

RoundedRectangle::RoundedRectangle(GameObject* pGameObj, const D2D1_ROUNDED_RECT& shape)
	: Geometry(pGameObj)
	, m_pShape(nullptr)
{
	if (FAILED(Engine::GetInstance().GetFactory()->CreateRoundedRectangleGeometry(shape, &m_pShape)))
		OutputDebugString(_T("CreateRoundedRectangleGeometry() FAILED"));
}

RoundedRectangle::~RoundedRectangle()
{
	SafeRelease(m_pShape);
}

