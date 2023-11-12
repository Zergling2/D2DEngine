#include "Pentagon.h"
#include "Core\Engine.h"
#include "App\Scene\SceneId.h"

Pentagon::Pentagon(const D2D1_POINT_2F shape[5])
	: GameObject()
	, m_pShape(nullptr)
	, m_pCollider(nullptr)
{
	m_pShape = new D2DEngine::Polygon(this, shape, 5U);
}

Pentagon::Pentagon(D2DEngine::ActiveFlag flag, const wchar_t* name, const D2D1_POINT_2F shape[5], D2DEngine::ObjectTag tag)
	: GameObject(flag, name, tag)
	, m_pShape(nullptr)
	, m_pCollider(nullptr)
{
	m_pShape = new D2DEngine::Polygon(this, shape, 5U);
}

Pentagon::~Pentagon()
{
	SafeDeleteScalar(m_pShape);

	if (m_pCollider)
	{
		D2DEngine::Engine::GetInstance().GetPhysicsProcessor().RemoveCollider(m_pCollider);
		m_pCollider = nullptr;
	}
}

bool Pentagon::CreateCollider(const ip::math::Vector2 convexShapeVerticesCCW[5])
{
	if (m_pCollider)
		return false;
	m_pCollider = D2DEngine::Engine::GetInstance().GetPhysicsProcessor().CreateHullCollider(convexShapeVerticesCCW, 5);
	return true;
}

bool Pentagon::RemoveCollider()
{
	return D2DEngine::Engine::GetInstance().GetPhysicsProcessor().RemoveCollider(m_pCollider);
}

void Pentagon::Start()
{
	D2DEngine::Log::Print(D2DEngine::Log::Type::LT_INFO, ToString());
#ifdef _DEBUG
	D2DEngine::Log::Print(D2DEngine::Log::Type::LT_SYSTEM, L"Pentagon::Start()");
#endif // _DEBUG
}

void Pentagon::Awake()
{
#ifdef _DEBUG
	D2DEngine::Log::Print(D2DEngine::Log::Type::LT_SYSTEM, L"Pentagon::Awake()");
#endif // _DEBUG
}

void Pentagon::FixedUpdate()
{
}

void Pentagon::Update()
{
}

void Pentagon::OnDestroy()
{
#ifdef _DEBUG
	D2DEngine::Log::Print(D2DEngine::Log::Type::LT_SYSTEM, L"Pentagon::OnDestroy()");
#endif // _DEBUG
}

void Pentagon::OnRender()
{
	if (!m_pShape->CheckEnable() || !m_pShape->m_pShape)
		return;

	static ID2D1SolidColorBrush* pYellowBrush = nullptr;
	static bool c = false;

	if (!c)
	{
		if (FAILED(D2DEngine::Engine::GetInstance().GetRenderTarget()->CreateSolidColorBrush(D2D1::ColorF(0.9f, 0.9f, 0.1f, 0.5f), &pYellowBrush)))
			OutputDebugString(_T("CreateSolidColorBrush() FAILED"));
		else
			c = true;
	}

	// RigidBody Transform 사용
	// GameObject::Transform 사용 X

	D2D1::Matrix3x2F wMat = D2D1::Matrix3x2F::Rotation(D2DEngine::Mathf::RadianToDegree(m_pCollider->GetRotation())) *
		D2D1::Matrix3x2F::Translation(m_pCollider->GetPosition().x, m_pCollider->GetPosition().y);
	D2DEngine::Engine::GetInstance().SetRenderingTransform(wMat);

	D2DEngine::Engine::GetInstance().GetRenderTarget()->FillGeometry(m_pShape->m_pShape, pYellowBrush, NULL);
	D2DEngine::Engine::GetInstance().GetRenderTarget()->DrawGeometry(m_pShape->m_pShape, pYellowBrush, 1.0f, NULL);
}
