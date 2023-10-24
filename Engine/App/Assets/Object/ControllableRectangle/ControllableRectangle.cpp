#include "ControllableRectangle.h"
#include "Core\Engine.h"
#include "App\Scene\SceneId.h"

ControllableRectangle::ControllableRectangle(const D2D_RECT_F& shape)
	: GameObject()
	, m_pShape(nullptr)
	, m_pCollider(nullptr)
{
	m_pShape = new D2DEngine::Rectangle(this, shape);
}

ControllableRectangle::ControllableRectangle(D2DEngine::ActiveFlag flag, const wchar_t* name, const D2D_RECT_F& shape, D2DEngine::ObjectTag tag)
	: GameObject(flag, name, tag)
	, m_pShape(nullptr)
	, m_pCollider(nullptr)
{
	m_pShape = new D2DEngine::Rectangle(this, shape);
}


ControllableRectangle::~ControllableRectangle()
{
	SafeDeleteScalar(m_pShape);

	if (m_pCollider)
	{
		D2DEngine::Engine::GetInstance().GetPhysicsProcessor().RemoveCollider(m_pCollider);
		m_pCollider = nullptr;
	}
}

bool ControllableRectangle::CreateCollider(const ip::math::Vector2 convexShapeVerticesCCW[], uint32_t vertexCount)
{
	if (m_pCollider)
		return false;
	m_pCollider = D2DEngine::Engine::GetInstance().GetPhysicsProcessor().CreatePolygonCollider(convexShapeVerticesCCW, vertexCount);
	return true;
}

bool ControllableRectangle::RemoveCollider()
{
	return D2DEngine::Engine::GetInstance().GetPhysicsProcessor().RemoveCollider(m_pCollider);
}

void ControllableRectangle::Start()
{
	D2DEngine::Log::Print(D2DEngine::Log::Type::LT_INFO, ToString());
#ifdef _DEBUG
	D2DEngine::Log::Print(D2DEngine::Log::Type::LT_SYSTEM, L"Square::Start()");
#endif // _DEBUG
}

void ControllableRectangle::Awake()
{
#ifdef _DEBUG
	D2DEngine::Log::Print(D2DEngine::Log::Type::LT_SYSTEM, L"Square::Awake()");
#endif // _DEBUG
}

void ControllableRectangle::FixedUpdate()
{
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		m_pCollider->GetRigidBody()->m_force.x -= 200.0;
		// m_pCollider->m_position.x -= 2.0f;
	}

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		m_pCollider->GetRigidBody()->m_force.y += 200.0;
		// m_pCollider->m_position.y += 2.0f;
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		m_pCollider->GetRigidBody()->m_force.x += 200.0;
		// m_pCollider->m_position.x += 2.0f;
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		m_pCollider->GetRigidBody()->m_force.y -= 200.0;
		// m_pCollider->m_position.y -= 2.0f;
	}

	if (GetAsyncKeyState('Q') & 0x8000)
	{
		m_pCollider->m_orientation += 2.0f;
	}

	if (GetAsyncKeyState('E') & 0x8000)
	{
		m_pCollider->m_orientation -= 2.0f;
	}

	// if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	// {
	// 	if (D2DEngine::SceneManager::GetInstance().GetCurrentSceneId() == static_cast<int>(SceneId::TestScene01))
	// 	{
	// 		D2DEngine::SceneManager::GetInstance().LoadScene(static_cast<int>(SceneId::TestScene02));
	// 	}
	// 	else if (D2DEngine::SceneManager::GetInstance().GetCurrentSceneId() == static_cast<int>(SceneId::TestScene02))
	// 	{
	// 		D2DEngine::SceneManager::GetInstance().LoadScene(static_cast<int>(SceneId::TestScene01));
	// 	}
	// }
}

void ControllableRectangle::Update()
{
}

void ControllableRectangle::OnDestroy()
{
#ifdef _DEBUG
	D2DEngine::Log::Print(D2DEngine::Log::Type::LT_SYSTEM, L"Player::OnDestroy()");
#endif // _DEBUG
}

void ControllableRectangle::OnRender()
{
	if (!m_pShape->CheckEnable() || !m_pShape->m_pShape)
		return;

	static bool c = false;
	static ID2D1SolidColorBrush* pBlueBrush = nullptr;

	if (!c)
	{
		if (FAILED(D2DEngine::Engine::GetInstance().GetRenderTarget()->CreateSolidColorBrush(D2D1::ColorF(0.1f, 0.2f, 0.9f, 0.5f), &pBlueBrush)))
			OutputDebugString(_T("CreateSolidColorBrush() FAILED"));
		else
			c = true;
	}

	// RigidBody Transform 사용
	// GameObject::Transform 사용 X

	D2D1::Matrix3x2F wMat = D2D1::Matrix3x2F::Rotation(m_pCollider->m_orientation) *
		D2D1::Matrix3x2F::Translation(m_pCollider->m_position.x, m_pCollider->m_position.y);
	D2DEngine::Engine::GetInstance().SetRenderingTransform(wMat);

	D2DEngine::Engine::GetInstance().GetRenderTarget()->FillGeometry(m_pShape->m_pShape, pBlueBrush, NULL);
	D2DEngine::Engine::GetInstance().GetRenderTarget()->DrawGeometry(m_pShape->m_pShape, pBlueBrush, 1.0f, NULL);
}
