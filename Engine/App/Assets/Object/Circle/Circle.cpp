#include "Circle.h"
#include "Core\Engine.h"

Circle::Circle(float radius)
	: GameObject()
	, m_pShape(nullptr)
{
	m_pShape = new D2DEngine::Ellipse(this, D2D1_ELLIPSE{ D2D1_POINT_2F{0.0f, 0.0f}, radius, radius });
}

Circle::Circle(D2DEngine::ActiveFlag flag, const wchar_t* name, float radius, D2DEngine::ObjectTag tag)
	: GameObject(flag, name, tag)
	, m_pShape(nullptr)
{
	m_pShape = new D2DEngine::Ellipse(this, D2D1_ELLIPSE{ D2D1_POINT_2F{0.0f, 0.0f}, radius, radius });
}

Circle::~Circle()
{
	SafeDeleteScalar(m_pShape);

	if (m_pCollider)
	{
		D2DEngine::Engine::GetInstance().GetPhysicsProcessor().RemoveCollider(m_pCollider);
		m_pCollider = nullptr;
	}
}

bool Circle::CreateCollider(float radius)
{
	if (m_pCollider)
		return false;

	m_pCollider = D2DEngine::Engine::GetInstance().GetPhysicsProcessor().CreateCircleCollider(radius);
	return true;
}

bool Circle::RemoveCollider()
{
	return D2DEngine::Engine::GetInstance().GetPhysicsProcessor().RemoveCollider(m_pCollider);
}

void Circle::Start()
{
	D2DEngine::Log::Print(D2DEngine::Log::Type::LT_INFO, ToString());
#ifdef _DEBUG
	D2DEngine::Log::Print(D2DEngine::Log::Type::LT_SYSTEM, L"Circle::Start()");
#endif // _DEBUG
}

void Circle::Awake()
{
#ifdef _DEBUG
	D2DEngine::Log::Print(D2DEngine::Log::Type::LT_SYSTEM, L"Circle::Awake()");
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
	D2DEngine::Log::Print(D2DEngine::Log::Type::LT_SYSTEM, L"Circle::OnDestroy()");
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
		if (FAILED(D2DEngine::Engine::GetInstance().GetRenderTarget()->CreateSolidColorBrush(D2D1::ColorF(0.2f, 0.9f, 0.2f, 0.5f), &pGreenBrush)))
			OutputDebugString(_T("CreateSolidColorBrush() FAILED"));
		else
			c = true;
	}

	// Collider Transform 사용
	// GameObject::Transform 사용 X
	D2D1::Matrix3x2F wMat = D2D1::Matrix3x2F::Rotation(D2DEngine::Mathf::RadianToDegree(m_pCollider->GetRotation())) *
		D2D1::Matrix3x2F::Translation(m_pCollider->GetPosition().x, m_pCollider->GetPosition().y);
	D2DEngine::Engine::GetInstance().SetRenderingTransform(wMat);

	D2DEngine::Engine::GetInstance().GetRenderTarget()->FillGeometry(m_pShape->m_pShape, pGreenBrush, NULL);
	D2DEngine::Engine::GetInstance().GetRenderTarget()->DrawGeometry(m_pShape->m_pShape, pGreenBrush, 1.0f, NULL);
}
