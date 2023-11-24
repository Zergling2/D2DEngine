#include "Circle.h"
#include "Core\Engine.h"

using namespace D2DEngine;

Circle::Circle(float radius)
	: GameObject()
	, m_pShape(nullptr)
{
	m_pShape = new Graphic::Ellipse(this, D2D1_ELLIPSE{ D2D1_POINT_2F{0.0f, 0.0f}, radius, radius });
}

Circle::Circle(ActiveFlag flag, const wchar_t* name, float radius, ObjectTag tag)
	: GameObject(flag, name, tag)
	, m_pShape(nullptr)
{
	m_pShape = new Graphic::Ellipse(this, D2D1_ELLIPSE{ D2D1_POINT_2F{0.0f, 0.0f}, radius, radius });
}

Circle::~Circle()
{
	D2DESafeDelete(m_pShape);
}

void Circle::Start()
{
	Log::Print(Log::Type::LT_INFO, ToString());
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"Circle::Start()");
#endif // _DEBUG
}

void Circle::Awake()
{
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"Circle::Awake()");
#endif // _DEBUG
}

void Circle::FixedUpdate()
{
}

void Circle::Update()
{
}

void Circle::OnDestroy()
{
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"Circle::OnDestroy()");
#endif // _DEBUG
}

void Circle::OnRender()
{
	if (!m_pShape->CheckEnable() || !m_pShape->m_pShape)
		return;

	static bool c = false;
	static ID2D1SolidColorBrush* pGreenBrush = nullptr;

	if (!c)
	{
		if (FAILED(Engine::GetInstance().GetRenderTarget()->CreateSolidColorBrush(D2D1::ColorF(0.2f, 0.9f, 0.2f, 0.5f), &pGreenBrush)))
			OutputDebugString(_T("CreateSolidColorBrush() FAILED"));
		else
			c = true;
	}

	// Collider Transform 사용
	// GameObject::Transform 사용 X
	D2D1::Matrix3x2F wMat = D2D1::Matrix3x2F::Rotation(Mathf::RadianToDegree(GetRotation())) *
		D2D1::Matrix3x2F::Translation(GetPosition().x, GetPosition().y);
	Engine::GetInstance().SetRenderingTransform(wMat);

	Engine::GetInstance().GetRenderTarget()->FillGeometry(m_pShape->m_pShape, pGreenBrush, NULL);
	Engine::GetInstance().GetRenderTarget()->DrawGeometry(m_pShape->m_pShape, pGreenBrush, 0.0f);
}
