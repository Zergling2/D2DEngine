#include "Capsule.h"
#include "Core\Engine.h"

using namespace D2DEngine::Graphic;

Capsule::Capsule(GameObject* pGameObj, float radius, float length)
	: Geometry(pGameObj)
	, m_pShape(nullptr)
{

	if (FAILED(Engine::GetInstance().GetFactory()->CreatePathGeometry(&m_pShape)))
		OutputDebugString(_T("CreatePathGeometry() FAILED"));

	ID2D1GeometrySink* pSink;
	if (FAILED(m_pShape->Open(&pSink)))
		OutputDebugString(_T("ID2D1PathGeometry::Open() FAILED"));

    float halfLength = length * 0.5f;

    pSink->BeginFigure(
        D2D1::Point2F(-halfLength, -radius),
        D2D1_FIGURE_BEGIN_FILLED
    );

    pSink->AddLine(D2D1::Point2F(halfLength, -radius));

    pSink->AddArc(
        D2D1::ArcSegment(
            D2D1::Point2F(halfLength, radius),
            D2D1::SizeF(radius, radius),
            0.0f,
            D2D1_SWEEP_DIRECTION_CLOCKWISE,
            D2D1_ARC_SIZE_SMALL
        )
    );

    pSink->AddLine(D2D1::Point2F(-halfLength, radius));

    pSink->AddArc(
        D2D1::ArcSegment(
            D2D1::Point2F(-halfLength, -radius),
            D2D1::SizeF(radius, radius),
            0.0f,
            D2D1_SWEEP_DIRECTION_CLOCKWISE,
            D2D1_ARC_SIZE_SMALL
        )
    );

    pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
    if (FAILED(pSink->Close()))
        OutputDebugString(_T("ID2D1GeometrySink::Close() FAILED"));

    pSink->Release();
}

Capsule::~Capsule()
{
	SafeRelease(m_pShape);
}
