#include "Oscillation.h"
#include "Core\Engine.h"

using namespace D2DEngine;

Oscillation::Oscillation(float radius)
	: GameObject()
	, m_pShape(nullptr)
{
	m_pShape = new Graphic::Ellipse(this, D2D1_ELLIPSE{ D2D1_POINT_2F{0.0f, 0.0f}, radius, radius });
}

Oscillation::~Oscillation()
{
	D2DESafeDelete(m_pShape);
}

void Oscillation::Start()
{
	v2 = ip::math::Vector2::zero;
	Log::Print(Log::Type::LT_INFO, ToString());
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"Oscillation::Start()");
#endif // _DEBUG
}

void Oscillation::Awake()
{
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"Oscillation::Awake()");
#endif // _DEBUG
}

void Oscillation::FixedUpdate()
{
	ip::math::Vector2 v1 = GetVelocity();
	float h = Time::GetFixedDeltaTime();
	float omega = GetAngularVelocity();
	ip::math::Vector2 x1 = GetPosition();
	SetVelocity(v1 - h * (omega * omega) * x1);
	SetPosition(GetPosition() + h * v1);
}

void Oscillation::Update()
{
}

void Oscillation::OnDestroy()
{
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"Oscillation::OnDestroy()");
#endif // _DEBUG
}

void Oscillation::OnRender()
{
	if (!m_pShape->CheckEnable() || !m_pShape->m_pShape)
		return;

	static bool c = false;
	static ID2D1SolidColorBrush* pBrush = nullptr;

	if (!c)
	{
		if (FAILED(Engine::GetInstance().GetRenderTarget()->CreateSolidColorBrush(D2D1::ColorF(0.5f, 0.5f, 0.75f, 0.5f), &pBrush)))
			OutputDebugString(_T("CreateSolidColorBrush() FAILED"));
		else
			c = true;
	}

	// Collider Transform 사용
	// GameObject::Transform 사용 X
	D2D1::Matrix3x2F wMat = D2D1::Matrix3x2F::Rotation(Mathf::RadianToDegree(GetRotation())) *
		D2D1::Matrix3x2F::Translation(GetPosition().x, GetPosition().y);
	Engine::GetInstance().SetRenderingTransform(wMat);

	Engine::GetInstance().GetRenderTarget()->FillGeometry(m_pShape->m_pShape, pBrush, NULL);
	Engine::GetInstance().GetRenderTarget()->DrawGeometry(m_pShape->m_pShape, pBrush, 0.0f);
}
