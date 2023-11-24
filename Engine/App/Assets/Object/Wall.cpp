#include "Wall.h"
#include "Core\Engine.h"
#include "App\Scene\SceneId.h"

Wall::Wall(const D2D_RECT_F& shape, float speed, float movement, const ip::math::Vector2& dir)
	: GameObject()
	, m_pShape(nullptr)
	, m_speed(speed)
	, m_movement(movement)
	, m_dist(0.0f)
	, m_dir(dir)
{
	m_pShape = new Graphic::Rectangle(this, shape);
}

Wall::~Wall()
{
	D2DESafeDelete(m_pShape);
}

void Wall::Start()
{
	Log::Print(Log::Type::LT_INFO, ToString());
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"Square::Start()");
#endif // _DEBUG
}

void Wall::Awake()
{
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"Square::Awake()");
#endif // _DEBUG
}

void Wall::FixedUpdate()
{
	
}

void Wall::Update()
{
	m_dist += Time::GetDeltaTime() * m_speed;
	if (m_dist >= m_movement)
	{
		m_dist -= m_movement;
		m_dir = m_dir * -1.0f;
	}
	SetPosition(GetPosition() + m_dir * Time::GetDeltaTime());
}

void Wall::OnDestroy()
{
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"Square::OnDestroy()");
#endif // _DEBUG
}

void Wall::OnRender()
{
	if (!m_pShape->CheckEnable() || !m_pShape->m_pShape)
		return;

	static ID2D1SolidColorBrush* pBlackBrush = nullptr;
	static bool c = false;

	if (!c)
	{
		if (FAILED(Engine::GetInstance().GetRenderTarget()->CreateSolidColorBrush(D2D1::ColorF(0.1f, 0.1f, 0.1f, 1.0f), &pBlackBrush)))
			OutputDebugString(_T("CreateSolidColorBrush() FAILED"));
		else
			c = true;
	}

	// RigidBody Transform 사용
	// GameObject::Transform 사용 X
	
	D2D1::Matrix3x2F wMat = D2D1::Matrix3x2F::Rotation(Mathf::RadianToDegree(GetRotation())) *
		D2D1::Matrix3x2F::Translation(GetPosition().x, GetPosition().y);
	Engine::GetInstance().SetRenderingTransform(wMat);

	Engine::GetInstance().GetRenderTarget()->FillGeometry(m_pShape->m_pShape, pBlackBrush, NULL);
	Engine::GetInstance().GetRenderTarget()->DrawGeometry(m_pShape->m_pShape, pBlackBrush, 0.0f);
}
