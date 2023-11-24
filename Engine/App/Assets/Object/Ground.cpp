#include "Ground.h"
#include "Core\Engine.h"
#include "App\Scene\SceneId.h"

using namespace D2DEngine;

Ground::Ground(const D2D1_POINT_2F shape[4])
	: GameObject()
	, m_pShape(nullptr)
{
	m_pShape = new Graphic::Polygon(this, shape, 4);
}

Ground::~Ground()
{
	D2DESafeDelete(m_pShape);
}

void Ground::Start()
{
	Log::Print(Log::Type::LT_INFO, ToString());
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"Ground::Start()");
#endif // _DEBUG
}

void Ground::Awake()
{
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"Ground::Awake()");
#endif // _DEBUG
}

void Ground::FixedUpdate()
{
}

void Ground::Update()
{
}

void Ground::OnDestroy()
{
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"Ground::OnDestroy()");
#endif // _DEBUG
}

void Ground::OnRender()
{
	if (!m_pShape->CheckEnable() || !m_pShape->m_pShape)
		return;

	static ID2D1SolidColorBrush* pBrownBrush = nullptr;
	static bool c = false;

	if (!c)
	{
		if (FAILED(Engine::GetInstance().GetRenderTarget()->CreateSolidColorBrush(D2D1::ColorF(101.0f / 255.0f, 67.0f / 255.0f, 33.0f / 255.0f, 1.0f), &pBrownBrush)))
			OutputDebugString(_T("CreateSolidColorBrush() FAILED"));
		else
			c = true;
	}

	D2D1::Matrix3x2F wMat = D2D1::Matrix3x2F::Rotation(Mathf::RadianToDegree(GetRotation())) *
		D2D1::Matrix3x2F::Translation(GetPosition().x, GetPosition().y);
	Engine::GetInstance().SetRenderingTransform(wMat);

	Engine::GetInstance().GetRenderTarget()->FillGeometry(m_pShape->m_pShape, pBrownBrush, NULL);
	Engine::GetInstance().GetRenderTarget()->DrawGeometry(m_pShape->m_pShape, pBrownBrush, 0.0f);
}
