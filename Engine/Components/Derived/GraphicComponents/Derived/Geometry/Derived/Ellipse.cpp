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

	// 형식: ID2D1Brush*
	// 기하 도형에 적용할 불투명 마스크이거나 불투명 마스크가 없는 경우 NULL 입니다.
	// 불투명 마스크(opacityBrush 매개 변수)를 지정한 경우 브러시는 x 및 y 확장 모드가 D2D1_EXTEND_MODE_CLAMP 설정된
	//  ID2D1BitmapBrush여야 합니다.자세한 내용은 주의 섹션을 참조하세요.

	Engine::GetInstance().GetRenderTarget()->FillGeometry(m_pShape, m_pBrush, NULL);
	Engine::GetInstance().GetRenderTarget()->DrawGeometry(m_pShape, m_pBrush, 1.0f, NULL);
}
