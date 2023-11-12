#include "Polygon.h"
#include "Core\Engine.h"

using namespace D2DEngine;

Polygon::Polygon(GameObject* pGameObj, const D2D1_POINT_2F vArr[], UINT count)
	: Geometry(pGameObj)
	, m_pShape(nullptr)
{
	if (count < 2U)
	{
		OutputDebugString(_T("Polygon must have at least 3 vertices."));
		return;
	}

	if (FAILED(Engine::GetInstance().GetFactory()->CreatePathGeometry(&m_pShape)))
		OutputDebugString(_T("CreatePathGeometry() FAILED"));

	ID2D1GeometrySink* pSink;
	if (FAILED(m_pShape->Open(&pSink)))
		OutputDebugString(_T("ID2D1PathGeometry::Open() FAILED"));

	pSink->SetFillMode(D2D1_FILL_MODE_WINDING);
	pSink->BeginFigure(vArr[count - 1], D2D1_FIGURE_BEGIN_FILLED);
	pSink->AddLines(vArr, count);
	pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

	if (FAILED(pSink->Close()))
		OutputDebugString(_T("ID2D1GeometrySink::Close() FAILED"));

	pSink->Release();
}

Polygon::~Polygon()
{
	SafeRelease(m_pShape);
}
