#pragma once

#include "Headers\Essential.h"
#include "Core\Time.h"
#include "Core\TagManager.h"
#include "Core\Scene.h"
#include "Core\SceneManager.h"

/*
// ##################################################
// Engine flow

// 2. Update loop

// 2 - 1. Fixed Update < PhysWorld API ȣ��ζ� ���� >
// 



// 2 - 2. Update, LateUpdate
// [1] Update
// [2] LateUpdate - ī�޶� ������Ʈ (Chasing cam ���� ��� Update���� ������Ʈ�� �̵��ع������� �����Ƿ�)

// LateUpdate���� ī�޶� ������Ʈ���� ���� �� ������ ����
*/

namespace D2DEngine
{
	/*
	class Application;
	*/

	class Engine
	{
	private:
		Engine();
		~Engine();
	public:
		static __forceinline Engine& GetInstance() { return Engine::s_instance; }
		__forceinline BOOL GetClientRect(RECT* pRect) const { return ::GetClientRect(m_hwnd, pRect); }
		bool Initialize();
		int Run();
		void Stop();
		__forceinline ID2D1Factory* GetFactory() const { return m_pDirect2dFactory; }

		__forceinline void CacheVPMatrix(const D2D1::Matrix3x2F& vpMatrix) { m_vpMatrixCache = vpMatrix; }
		__forceinline void SetRenderingTransform(const D2D1::Matrix3x2F& wMat) { m_pRenderTarget->SetTransform(wMat * m_vpMatrixCache); }
		__forceinline ID2D1RenderTarget* GetRenderTarget() const { return m_pRenderTarget; }
		__forceinline ip::PhysicsWorld& PhysWorld() { return m_physWorld; }
	private:
		// Engine loop
		void Release();

		void Loop();

		HRESULT RenderScene(Scene& scene);

		// Initialize device-independent resources.
		HRESULT CreateDeviceIndependentResources();

		// Initialize device-dependent resources.
		// While your program is running, the graphics device that you are using might become unavailable.
		// For example, the device can be lost if the display resolution changes, or if the user removes the display adapter.
		// If the device is lost, the render target also becomes invalid, along with any device-
		// dependent resources that were associated with the device.
		// Direct2D signals a lost device by returning the error code D2DERR_RECREATE_TARGET from the EndDraw method.
		// If you receive this error code, you must re-create the render target and all device-dependent resources.
		// 
		// Creating a resource can be an expensive operation, so do not recreate your resources for every WM_PAINT message.
		// Create a resource once, and cache the resource pointer until the resource becomes invalid due to device loss,
		// or until you no longer need that resource.
		HRESULT CreateDeviceResources();

		// Release device-dependent resource.
		void DiscardDeviceResources();

		// Resize the render target.
		void OnResize(UINT width, UINT height);

		// The windows procedure.
		static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

		bool EngineRegisterClass();
		bool EngineCreateWindow();
		// bool CreateTagManager();
		// void ReleaseTagManager();

		// variables
		static Engine s_instance;
		bool m_initialized;
		bool m_loop;
		bool m_changeSceneFlag;
		HRESULT m_lastResult;
		float m_accumTime;
		LONGLONG m_lastPhysUpdateTime;		// physics update
		HWND m_hwnd;
		// Application* m_pApp;
		ID2D1Factory* m_pDirect2dFactory;
		ID2D1HwndRenderTarget* m_pRenderTarget;
		// class TagManager* m_pTagManager;
		UINT m_width;		// Window width
		UINT m_height;		// Window height
		D2D1::Matrix3x2F m_vpMatrixCache;
		ip::PhysicsWorld m_physWorld;
	};


	/*
	class Application
	{
		friend class Engine;
	public:
		Application();
		virtual ~Application() = 0;
	private:
		// ���ø����̼��� ó�� ���۵� �� ���ʿ� �ѹ� ȣ��˴ϴ�.
		virtual void Start() = 0;

		// �� �����Ӹ��� �ش� �Լ��� ȣ��˴ϴ�.
		virtual void Update() = 0;

		// ������ �ð��� ȣ�� ������ŭ �ش� �Լ��� ȣ��˴ϴ�.
		virtual void FixedUpdate() = 0;

		// ���� ���� (���� ���ο��� ��� ������Ʈ ������ �ڵ�ȭ) - 23.07.05 �ϴ� �ٸ� ��� ������ ���� ����
		// ���ø����̼��� �������ϼ���.
		virtual void Render() = 0;

		// ===== Render �Լ� �ڵ�ȭ ���� �̰͵� �ڵ�ȭ ���� =====
		// ���ø����̼ǿ��� �Ҵ��� ��� ���ҽ�(������ ���ҽ� ����)�� �����ϼ���.
		virtual void Release() = 0;


		// ===== Render �Լ� �ڵ�ȭ ���� �̰͵� �ڵ�ȭ ���� =====
		// �߿�!
		// �ݵ�� �� �Լ��� ȣ��Ǿ��� �� ������ ���ҽ����� �����ؾ� �մϴ�.
		virtual void CreateDeviceDependentResources() = 0;


		// ===== Render �Լ� �ڵ�ȭ ���� �̰͵� �ڵ�ȭ ���� =====
		// �߿�!
		// ���������� ID2D1HwndRenderTarget ��ü�� ������Ǿ�� �ϴ� ��Ȳ�� �߻��� �� �ֽ��ϴ�.
		// �� ��� ������ ID2D1HwndRenderTarget ��ü�� ������ ������ ���ҽ����� �� �̻� ����� �� �����Ƿ�
		// ������ ���ҽ����� ��� �����ؾ� �մϴ�.
		virtual void ReleaseDeviceDependentResources() = 0;
	};
	*/
}
