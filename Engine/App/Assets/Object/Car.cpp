#include "Car.h"
#include "Core\Engine.h"
#include "App\Scene\SceneId.h"
#include "App\Assets\Object\Circle.h"

using namespace D2DEngine;

Car::Car()
	: GameObject()
	, m_pShape(nullptr)
{
	D2D1_POINT_2F shape[6];
	shape[0] = { -0.5f, -0.25f };
	shape[1] = { 0.5f, -0.25f };
	shape[2] = { 0.5f, 0.0f };
	shape[3] = { 0.2f, 0.0f };
	shape[4] = { 0.15f, 0.25f };
	shape[5] = { -0.5f, 0.25f };
	m_pShape = new Graphic::Polygon(this, shape, 6);
}

Car::~Car()
{
	D2DESafeDelete(m_pShape);
}

void Car::Start()
{
	Log::Print(Log::Type::LT_INFO, ToString());
}

void Car::Awake()
{
}

void Car::FixedUpdate()
{
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		Circle* pCircle = new Circle(0.1f);
		pCircle->CreateCircleCollider(ip::Default, 0.1f);
		pCircle->CreateRigidBody(ip::Dynamic, real(0.05), real(0.002), real(0.3), real(0.0), real(0.0), real(1.0));
		pCircle->SetPosition(this->GetPosition().x + GetLocalDirection().x, this->GetPosition().y + GetLocalDirection().y);
		pCircle->SetRotation(this->GetRotation());
		pCircle->AddRelativeForce(100.0, 0.0);
		Engine::GetInstance().PhysWorld().AddPhysicsObject(pCircle);
		SceneManager::GetInstance().GetCurrentScene().AddObject(pCircle, LayerType::Interactable);
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		Translate(-0.05, 0, ip::World);
		// Translate(-0.02, 0, ip::Space::World);
	}

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		Translate(0, 0.05, ip::World);
		// Translate(0, 0.02, ip::Space::World);
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		Translate(0.05, 0, ip::World);
		// Translate(0.02, 0, ip::Space::World);
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		Translate(0, -0.05, ip::World);
		// Translate(0, -0.02, ip::Space::World);
	}

	if (GetAsyncKeyState('Q') & 0x8000)
	{
		Rotate(0.02);
	}

	if (GetAsyncKeyState('E') & 0x8000)
	{
		Rotate(-0.02);
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

void Car::Update()
{
}

void Car::OnDestroy()
{
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"Player::OnDestroy()");
#endif // _DEBUG
}

void Car::OnRender()
{
	if (!m_pShape->CheckEnable() || !m_pShape->m_pShape)
		return;

	static bool c = false;
	static ID2D1SolidColorBrush* pBlueBrush = nullptr;

	if (!c)
	{
		if (FAILED(Engine::GetInstance().GetRenderTarget()->CreateSolidColorBrush(D2D1::ColorF(0.1f, 0.2f, 0.9f, 0.5f), &pBlueBrush)))
			OutputDebugString(_T("CreateSolidColorBrush() FAILED"));
		else
			c = true;
	}

	// RigidBody Transform 사용
	// GameObject::Transform 사용 X

	D2D1::Matrix3x2F wMat = D2D1::Matrix3x2F::Rotation(Mathf::RadianToDegree(GetRotation())) *
		D2D1::Matrix3x2F::Translation(GetPosition().x, GetPosition().y);
	Engine::GetInstance().SetRenderingTransform(wMat);

	Engine::GetInstance().GetRenderTarget()->FillGeometry(m_pShape->m_pShape, pBlueBrush, NULL);
	Engine::GetInstance().GetRenderTarget()->DrawGeometry(m_pShape->m_pShape, pBlueBrush, 0.0f);
}
