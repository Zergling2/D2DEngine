#include "Circle.h"
#include "Core\Engine.h"

Circle::Circle(D2DEngine::ActiveFlag flag, const wchar_t* name, D2DEngine::ObjectTag tag)
	: GameObject(flag, name, tag)
	, m_pCircle(nullptr)
{
	m_pCircle = new D2DEngine::Ellipse(this, D2D1_ELLIPSE{ D2D1_POINT_2F{0.0f, 0.0f}, 25.0f, 25.0f });
}

Circle::~Circle()
{
	SafeDeleteScalar(m_pCircle);
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
	m_pCircle->Render();
}
