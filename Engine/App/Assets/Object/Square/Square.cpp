#include "Square.h"
#include "Core\Engine.h"
#include "App\Scene\SceneId.h"

Square::Square(D2DEngine::ActiveFlag flag, const wchar_t* name, D2DEngine::ObjectTag tag)
	: GameObject(flag, name, tag)
	, m_pSquare(nullptr)
{
	m_pSquare = new D2DEngine::Rectangle(this, D2D_RECT_F{ -25.0f, 25.0f, 25.0f, -25.0f });
}

Square::~Square()
{
	SafeDeleteScalar(m_pSquare);
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
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		this->GetTransform().Translate(-2.0f, 0.0f);
	}

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		this->GetTransform().Translate(0.0f, 2.0f);
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		this->GetTransform().Translate(+2.0f, 0.0f);
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		this->GetTransform().Translate(0.0f, -2.0f);
	}

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		if (D2DEngine::SceneManager::GetInstance().GetCurrentSceneId() == static_cast<int>(SceneId::TestScene01))
		{
			D2DEngine::SceneManager::GetInstance().LoadScene(static_cast<int>(SceneId::TestScene02));
		}
		else if (D2DEngine::SceneManager::GetInstance().GetCurrentSceneId() == static_cast<int>(SceneId::TestScene02))
		{
			D2DEngine::SceneManager::GetInstance().LoadScene(static_cast<int>(SceneId::TestScene01));
		}
	}
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
	m_pSquare->Render();
}
