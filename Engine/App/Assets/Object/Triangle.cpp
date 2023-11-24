#include "Triangle.h"
#include "Core\Engine.h"
#include "App\Scene\SceneId.h"

Triangle::Triangle(const D2D1_POINT_2F shape[3])
	: GameObject()
	, m_pShape(nullptr)
{
	m_pShape = new Graphic::Polygon(this, shape, 3U);
}

Triangle::Triangle(ActiveFlag flag, const wchar_t* name, const D2D1_POINT_2F shape[3], ObjectTag tag)
	: GameObject(flag, name, tag)
	, m_pShape(nullptr)
{
	m_pShape = new Graphic::Polygon(this, shape, 3U);
}

Triangle::~Triangle()
{
	D2DESafeDelete(m_pShape);
}

void Triangle::Start()
{
	Log::Print(Log::Type::LT_INFO, ToString());
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"Triangle::Start()");
#endif // _DEBUG
}

void Triangle::Awake()
{
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"Triangle::Awake()");
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
	Log::Print(Log::Type::LT_SYSTEM, L"Triangle::OnDestroy()");
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
		if (FAILED(Engine::GetInstance().GetRenderTarget()->CreateSolidColorBrush(D2D1::ColorF(100/255.0f, 0.0f, 150/255.0f, 0.5f), &pPurpleBrush)))
			OutputDebugString(_T("CreateSolidColorBrush() FAILED"));
		else
			c = true;
	}

	// RigidBody Transform 사용
	// GameObject::Transform 사용 X

	D2D1::Matrix3x2F wMat = D2D1::Matrix3x2F::Rotation(Mathf::RadianToDegree(GetRotation())) *
		D2D1::Matrix3x2F::Translation(GetPosition().x, GetPosition().y);
	Engine::GetInstance().SetRenderingTransform(wMat);

	Engine::GetInstance().GetRenderTarget()->FillGeometry(m_pShape->m_pShape, pPurpleBrush, NULL);
	Engine::GetInstance().GetRenderTarget()->DrawGeometry(m_pShape->m_pShape, pPurpleBrush, 0.0f);
}
