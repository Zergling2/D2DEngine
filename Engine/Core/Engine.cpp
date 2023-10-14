#include "Engine.h"
#include "Core\SceneManager.h"

using namespace D2DEngine;

Engine Engine::s_instance;

Engine::Engine()
	: m_initialized(false)
	, m_loop(true)
	, m_changeSceneFlag(false)
	, m_lastResult(S_OK)
	, m_accumTime(0.0f)
	, m_lastPhysUpdateTime(0LL)
	, m_hwnd(nullptr)
	, m_pDirect2dFactory(nullptr)
	, m_pRenderTarget(nullptr)
	// , m_pTagManager(nullptr)
	, m_width(RES_WIDTH)
	, m_height(RES_HEIGHT)
{
}

Engine::~Engine()
{
	Release();
}

bool Engine::Initialize()
{
	m_loop = true;

	// 타이머 초기화
	if (!Time::Initialize())
		return false;

	/*
	// 태그 관리자 초기화
	if (!TagManager::GetInstance().Initialize())
		return false;
	*/


	// Initialize device-indpendent resources, such as the Direct2D factory.
	m_lastResult = CreateDeviceIndependentResources();
	if (FAILED(m_lastResult))
		return false;

	// RegisterClass API 호출
	if (!Engine::EngineRegisterClass())
		return false;

	// CreateWindow API 호출
	if (!Engine::EngineCreateWindow())
		return false;

	// ID2D1HwndRenderTarget 객체 생성
	m_lastResult = CreateDeviceResources();
	if (FAILED(m_lastResult))
		return false;

	m_initialized = true;

	return true;
}

int Engine::Run()
{
	if (!m_initialized)
		return -1;

	MSG msg = { 0, };

	Time::Tick();

	while (m_loop)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Loop();
		}
	}

	return static_cast<int>(msg.wParam);
}

void Engine::Stop()
{
	m_loop = false;
}

void Engine::Release()
{
	SafeRelease(m_pRenderTarget);
	SafeRelease(m_pDirect2dFactory);
	// TagManager::GetInstance().Release();

	m_initialized = false;
}

LRESULT CALLBACK Engine::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		return 0;
	case WM_DESTROY:
		Engine::GetInstance().Release();
		DestroyWindow(hwnd);
		return 0;
	case WM_SIZE:
		Engine::GetInstance().OnResize(LOWORD(lParam), HIWORD(lParam));
		return 0;
	case WM_PAINT:
		ValidateRect(hwnd, NULL);
		return 0;
	case WM_CLOSE:
		Engine::GetInstance().Stop();
		PostQuitMessage(0);
		return 0;
	case WM_DISPLAYCHANGE:
		InvalidateRect(hwnd, NULL, FALSE);
		return 0;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
}

void Engine::Loop()
{
	if (SceneManager::GetInstance().m_sceneChangeFlag)
	{
		bool result = SceneManager::GetInstance().CreateNextScene();
		Time::Tick();			// update timer
		m_accumTime = 0.0f;
		if (!result)
			return;
	}

	Scene* pCurrentScene = SceneManager::GetInstance().GetCurrentScene();
	Time::Tick();			// update timer
	m_accumTime += Time::GetRealDeltaTime();

	// ProcessInput();


	// Avoid 'spiral of death' and clamp dt, thus clamping 
	// how many times the physics update can be called in 
	// a single game loop. 
	// if (accumulator > 0.2f)
	// 	accumulator = 0.2f

	while (m_accumTime >= FIXED_DELTA_TIME)
	{
		pCurrentScene->FixedUpdate();
		m_physicsProcessor.Update();
		m_accumTime -= FIXED_DELTA_TIME;
	}

	pCurrentScene->Update();
	// pCurrentScene->LateUpdate();
	RenderScene(pCurrentScene);			// Render App
}

HRESULT Engine::RenderScene(Scene* pScene)
{
	m_pRenderTarget->BeginDraw();
	Engine::GetInstance().GetRenderTarget()->Clear(D2D1::ColorF(D2D1::ColorF::White));

	pScene->Render();

	m_lastResult = m_pRenderTarget->EndDraw();

	if (m_lastResult == D2DERR_RECREATE_TARGET)
	{
		pScene->ReleaseDeviceDependentResources();
		DiscardDeviceResources();
		CreateDeviceResources();
		pScene->CreateDeviceDependentResources();
	}

	return m_lastResult;
}

HRESULT Engine::CreateDeviceIndependentResources()
{
	m_lastResult = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);
	return m_lastResult;
}

HRESULT Engine::CreateDeviceResources()
{
	if (m_pRenderTarget)		// 중복 생성 방지
		return E_FAIL;

	RECT rc;
	::GetClientRect(m_hwnd, &rc);
	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

	// Create a Direct2D render target.
	return m_pDirect2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), 
		D2D1::HwndRenderTargetProperties(m_hwnd, size, D2D1_PRESENT_OPTIONS_IMMEDIATELY), &m_pRenderTarget);
}

void Engine::DiscardDeviceResources()
{
	SafeRelease(m_pRenderTarget);
}

void Engine::OnResize(UINT width, UINT height)
{
	if (m_pRenderTarget)
	{
		// Note: This method can fail, but it's okay to ignore the
		// error here, because the error will be returned again
		// the next time EndDraw is called.
		m_pRenderTarget->Resize(D2D1::SizeU(width, height));
	}
}

bool Engine::EngineRegisterClass()
{
	// Register the window class.
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Engine::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = HINST_THISCOMPONENT;
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
	wcex.lpszClassName = WNDCLASS_NAME;

	return RegisterClassEx(&wcex) != 0 ? true : false;
}

bool Engine::EngineCreateWindow()
{
	/*
	In terms of using the correct DPI, to create a window at a specific size
	like this, the procedure is to first create the window hidden. Then we get
	the actual DPI from the HWND (which will be assigned by whichever monitor
	the window is created on). Then we use SetWindowPos to resize it to the
	correct DPI-scaled size, then we use ShowWindow to show it.
	*/

	HWND result = CreateWindowW(
		WNDCLASS_NAME,
		WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		0,
		0,
		NULL,
		NULL,
		HINST_THISCOMPONENT,
		&Engine::GetInstance());

	if (result == NULL)
		return false;

	Engine::GetInstance().m_hwnd = result;
	// Because the SetWindowPos function takes its size in pixels, we
	// obtain the window's DPI, and use it to scale the window size.

	RECT rc = { 0, 0, (LONG)m_width, (LONG)m_height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	SetWindowPos(Engine::GetInstance().m_hwnd, HWND_TOP, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE);
	ShowWindow(Engine::GetInstance().m_hwnd, SW_SHOWNORMAL);
	UpdateWindow(Engine::GetInstance().m_hwnd);

	return true;
}

/*
Application::Application()
{
}

Application::~Application()
{
}
*/