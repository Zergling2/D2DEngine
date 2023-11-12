#include "Triangle.h"
#include "Core\Engine.h"
#include "App\Scene\SceneId.h"

Triangle::Triangle(const D2D1_POINT_2F shape[3])
	: GameObject()
	, m_pShape(nullptr)
	, m_pCollider(nullptr)
{
	m_pShape = new D2DEngine::Polygon(this, shape, 3U);
}

Triangle::Triangle(D2DEngine::ActiveFlag flag, const wchar_t* name, const D2D1_POINT_2F shape[3], D2DEngine::ObjectTag tag)
	: GameObject(flag, name, tag)
	, m_pShape(nullptr)
	, m_pCollider(nullptr)
{
	m_pShape = new D2DEngine::Polygon(this, shape, 3U);
}

Triangle::~Triangle()
{
	SafeDeleteScalar(m_pShape);

	if (m_pCollider)
	{
		D2DEngine::Engine::GetInstance().GetPhysicsProcessor().RemoveCollider(m_pCollider);
		m_pCollider = nullptr;
	}
}

bool Triangle::CreateCollider(const ip::math::Vector2 convexShapeVerticesCCW[3])
{
	if (m_pCollider)
		return false;
	m_pCollider = D2DEngine::Engine::GetInstance().GetPhysicsProcessor().CreateHullCollider(convexShapeVerticesCCW, 3);
	return true;
}

bool Triangle::RemoveCollider()
{
	return D2DEngine::Engine::GetInstance().GetPhysicsProcessor().RemoveCollider(m_pCollider);
}

void Triangle::Start()
{
	D2DEngine::Log::Print(D2DEngine::Log::Type::LT_INFO, ToString());
#ifdef _DEBUG
	D2DEngine::Log::Print(D2DEngine::Log::Type::LT_SYSTEM, L"Triangle::Start()");
#endif // _DEBUG
}

void Triangle::Awake()
{
#ifdef _DEBUG
	D2DEngine::Log::Print(D2DEngine::Log::Type::LT_SYSTEM, L"Triangle::Awake()");
#endif // _DEBUG
}

void Triangle::FixedUpdate()
{
}

void Triangle::Update()
{
}

void Triangle::OnDestroy()
{
#ifdef _DEBUG
	D2DEngine::Log::Print(D2DEngine::Log::Type::LT_SYSTEM, L"Triangle::OnDestroy()");
#endif // _DEBUG
}

void Triangle::OnRender()
{
	if (!m_pShape->CheckEnable() || !m_pShape->m_pShape)
		return;

	static ID2D1SolidColorBrush* pPurpleBrush = nullptr;
	static bool c = false;

	if (!c)
	{
		if (FAILED(D2DEngine::Engine::GetInstance().GetRenderTarget()->CreateSolidColorBrush(D2D1::ColorF(100/255.0f, 0.0f, 150/255.0f, 0.5f), &pPurpleBrush)))
			OutputDebugString(_T("CreateSolidColorBrush() FAILED"));
		else
			c = true;
	}

	// RigidBody Transform 사용
	// GameObject::Transform 사용 X

	D2D1::Matrix3x2F wMat = D2D1::Matrix3x2F::Rotation(D2DEngine::Mathf::RadianToDegree(m_pCollider->GetRotation())) *
		D2D1::Matrix3x2F::Translation(m_pCollider->GetPosition().x, m_pCollider->GetPosition().y);
	D2DEngine::Engine::GetInstance().SetRenderingTransform(wMat);

	D2DEngine::Engine::GetInstance().GetRenderTarget()->FillGeometry(m_pShape->m_pShape, pPurpleBrush, NULL);
	D2DEngine::Engine::GetInstance().GetRenderTarget()->DrawGeometry(m_pShape->m_pShape, pPurpleBrush, 1.0f, NULL);
}
