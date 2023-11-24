#include "RopeSegment.h"
#include "Core\Engine.h"

using namespace D2DEngine;

RopeSegment::RopeSegment(float radius)
	: GameObject()
	, m_pShape(nullptr)
{
	m_pShape = new Graphic::Ellipse(this, D2D1_ELLIPSE{ D2D1_POINT_2F{0.0f, 0.0f}, radius, radius });
}

RopeSegment::~RopeSegment()
{
	D2DESafeDelete(m_pShape);
}

void RopeSegment::Start()
{
	Log::Print(Log::Type::LT_INFO, ToString());
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"RopeSegment::Start()");
#endif // _DEBUG
}

void RopeSegment::Awake()
{
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"RopeSegment::Awake()");
#endif // _DEBUG
}

void RopeSegment::FixedUpdate()
{
}

void RopeSegment::Update()
{
}

void RopeSegment::OnDestroy()
{
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"RopeSegment::OnDestroy()");
#endif // _DEBUG
}

void RopeSegment::OnRender()
{
	if (!m_pShape->CheckEnable() || !m_pShape->m_pShape)
		return;

	static bool c = false;
	static ID2D1SolidColorBrush* pBlueBrush = nullptr;

	if (!c)
	{
		if (FAILED(Engine::GetInstance().GetRenderTarget()->CreateSolidColorBrush(D2D1::ColorF(0.1f, 0.1f, 0.8f, 0.8f), &pBlueBrush)))
			OutputDebugString(_T("CreateSolidColorBrush() FAILED"));
		else
			c = true;
	}

	// Collider Transform 사용
	// GameObject::Transform 사용 X
	D2D1::Matrix3x2F wMat = D2D1::Matrix3x2F::Rotation(Mathf::RadianToDegree(GetRotation())) *
		D2D1::Matrix3x2F::Translation(GetPosition().x, GetPosition().y);
	Engine::GetInstance().SetRenderingTransform(wMat);

	Engine::GetInstance().GetRenderTarget()->FillGeometry(m_pShape->m_pShape, pBlueBrush, NULL);
	Engine::GetInstance().GetRenderTarget()->DrawGeometry(m_pShape->m_pShape, pBlueBrush, 0.0f);
}
