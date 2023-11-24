#include "Stone.h"
#include "Core\Engine.h"
#include "App\Scene\SceneId.h"

using namespace D2DEngine;

Stone::Stone(const D2D1_POINT_2F shape[5])
	: GameObject()
	, m_pShape(nullptr)
{
	m_pShape = new Graphic::Polygon(this, shape, 5U);
}

Stone::Stone(ActiveFlag flag, const wchar_t* name, const D2D1_POINT_2F shape[5], ObjectTag tag)
	: GameObject(flag, name, tag)
	, m_pShape(nullptr)
{
	m_pShape = new Graphic::Polygon(this, shape, 5U);
}

Stone::~Stone()
{
	D2DESafeDelete(m_pShape);
}

void Stone::Start()
{
	Log::Print(Log::Type::LT_INFO, ToString());
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"Stone::Start()");
#endif // _DEBUG
}

void Stone::Awake()
{
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"Stone::Awake()");
#endif // _DEBUG
}

void Stone::FixedUpdate()
{
}

void Stone::Update()
{
}

void Stone::OnDestroy()
{
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"Stone::OnDestroy()");
#endif // _DEBUG
}

void Stone::OnRender()
{
	if (!m_pShape->CheckEnable() || !m_pShape->m_pShape)
		return;

	static ID2D1SolidColorBrush* pGrayBrush = nullptr;
	static bool c = false;

	if (!c)
	{
		if (FAILED(Engine::GetInstance().GetRenderTarget()->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.5f), &pGrayBrush)))
			OutputDebugString(_T("CreateSolidColorBrush() FAILED"));
		else
			c = true;
	}

	// RigidBody Transform 사용
	// GameObject::Transform 사용 X

	D2D1::Matrix3x2F wMat = D2D1::Matrix3x2F::Rotation(Mathf::RadianToDegree(GetRotation())) *
		D2D1::Matrix3x2F::Translation(GetPosition().x, GetPosition().y);
	Engine::GetInstance().SetRenderingTransform(wMat);

	Engine::GetInstance().GetRenderTarget()->FillGeometry(m_pShape->m_pShape, pGrayBrush, NULL);
	Engine::GetInstance().GetRenderTarget()->DrawGeometry(m_pShape->m_pShape, pGrayBrush, 0.0f);
}
