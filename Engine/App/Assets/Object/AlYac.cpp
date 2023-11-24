#include "AlYac.h"
#include "Core\Engine.h"
#include "App\Scene\SceneId.h"

AlYac::AlYac(real radius, real length)
	: GameObject()
	, m_pShape(nullptr)
{
	m_pShape = new Graphic::Capsule(this, radius, length);
}

AlYac::~AlYac()
{
	D2DESafeDelete(m_pShape);
}

void AlYac::Start()
{
	Log::Print(Log::Type::LT_INFO, ToString());
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"AlYac::Start()");
#endif // _DEBUG
}

void AlYac::Awake()
{
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"AlYac::Awake()");
#endif // _DEBUG
}

void AlYac::FixedUpdate()
{
	Rotate(Mathf::DegreeToRadian(1.0f));
}

void AlYac::Update()
{
}

void AlYac::OnDestroy()
{
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"AlYac::OnDestroy()");
#endif // _DEBUG
}

void AlYac::OnRender()
{
	if (!m_pShape->CheckEnable() || !m_pShape->m_pShape)
		return;

	static ID2D1SolidColorBrush* pPinkBrush = nullptr;
	static bool c = false;

	if (!c)
	{
		if (FAILED(Engine::GetInstance().GetRenderTarget()->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.2f, 0.2f, 0.5f), &pPinkBrush)))
			OutputDebugString(_T("CreateSolidColorBrush() FAILED"));
		else
			c = true;
	}

	// RigidBody Transform 사용
	// GameObject::Transform 사용 X

	D2D1::Matrix3x2F wMat = D2D1::Matrix3x2F::Rotation(Mathf::RadianToDegree(GetRotation())) *
		D2D1::Matrix3x2F::Translation(GetPosition().x, GetPosition().y);
	Engine::GetInstance().SetRenderingTransform(wMat);

	Engine::GetInstance().GetRenderTarget()->FillGeometry(m_pShape->m_pShape, pPinkBrush, NULL);
	Engine::GetInstance().GetRenderTarget()->DrawGeometry(m_pShape->m_pShape, pPinkBrush, 0.0f);
}
