#include "Square.h"
#include "Core\Engine.h"
#include "App\Scene\SceneId.h"

Square::Square(const D2D_RECT_F& shape)
	: GameObject()
	, m_pShape(nullptr)
	, m_pCollider(nullptr)
{
	m_pShape = new D2DEngine::Rectangle(this, shape);
}

Square::Square(D2DEngine::ActiveFlag flag, const wchar_t* name, const D2D_RECT_F& shape, D2DEngine::ObjectTag tag)
	: GameObject(flag, name, tag)
	, m_pShape(nullptr)
	, m_pCollider(nullptr)
{
	m_pShape = new D2DEngine::Rectangle(this, shape);
}

Square::~Square()
{
	SafeDeleteScalar(m_pShape);
	
	if (m_pCollider)
	{
		D2DEngine::Engine::GetInstance().GetPhysicsProcessor().RemoveCollider(m_pCollider);
		m_pCollider = nullptr;
	}
}

bool Square::CreateCollider(const ip::math::Vector2 convexShapeVerticesCCW[], uint32_t vertexCount)
{
	if (m_pCollider)
		return false;
	m_pCollider = D2DEngine::Engine::GetInstance().GetPhysicsProcessor().CreatePolygonCollider(convexShapeVerticesCCW, vertexCount);
	return true;
}

bool Square::RemoveCollider()
{
	return D2DEngine::Engine::GetInstance().GetPhysicsProcessor().RemoveCollider(m_pCollider);
}

void Square::Start()
{
	D2DEngine::Log::Print(D2DEngine::Log::Type::LT_INFO, ToString());
#ifdef _DEBUG
	D2DEngine::Log::Print(D2DEngine::Log::Type::LT_SYSTEM, L"Square::Start()");
#endif // _DEBUG
}

void Square::Awake()
{
#ifdef _DEBUG
	D2DEngine::Log::Print(D2DEngine::Log::Type::LT_SYSTEM, L"Square::Awake()");
#endif // _DEBUG
}

void Square::FixedUpdate()
{
}

void Square::Update()
{
}

void Square::OnDestroy()
{
#ifdef _DEBUG
	D2DEngine::Log::Print(D2DEngine::Log::Type::LT_SYSTEM, L"Square::OnDestroy()");
#endif // _DEBUG
}

void Square::OnRender()
{
	if (!m_pShape->CheckEnable() || !m_pShape->m_pShape)
		return;

	static ID2D1SolidColorBrush* pRedBrush = nullptr;
	static bool c = false;

	if (!c)
	{
		if (FAILED(D2DEngine::Engine::GetInstance().GetRenderTarget()->CreateSolidColorBrush(D2D1::ColorF(0.9f, 0.1f, 0.1f, 0.5f), &pRedBrush)))
			OutputDebugString(_T("CreateSolidColorBrush() FAILED"));
		else
			c = true;
	}

	// RigidBody Transform 사용
	// GameObject::Transform 사용 X

	D2D1::Matrix3x2F wMat = D2D1::Matrix3x2F::Rotation(m_pCollider->m_orientation) *
		D2D1::Matrix3x2F::Translation(m_pCollider->m_position.x, m_pCollider->m_position.y);
	D2DEngine::Engine::GetInstance().SetRenderingTransform(wMat);

	D2DEngine::Engine::GetInstance().GetRenderTarget()->FillGeometry(m_pShape->m_pShape, pRedBrush, NULL);
	D2DEngine::Engine::GetInstance().GetRenderTarget()->DrawGeometry(m_pShape->m_pShape, pRedBrush, 1.0f, NULL);
}
