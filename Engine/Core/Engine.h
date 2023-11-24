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

// 2 - 1. Fixed Update < PhysWorld API 호출부랑 관련 >
// 



// 2 - 2. Update, LateUpdate
// [1] Update
// [2] LateUpdate - 카메라 업데이트 (Chasing cam 등의 경우 Update에서 오브젝트가 이동해버릴수도 있으므로)

// LateUpdate에서 카메라 업데이트까지 끝난 후 렌더링 시작
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
		// 애플리케이션이 처음 시작될 때 최초에 한번 호출됩니다.
		virtual void Start() = 0;

		// 매 프레임마다 해당 함수가 호출됩니다.
		virtual void Update() = 0;

		// 정해진 시간당 호출 비율만큼 해당 함수가 호출됩니다.
		virtual void FixedUpdate() = 0;

		// 제거 예정 (엔진 내부에서 모든 오브젝트 렌더링 자동화) - 23.07.05 일단 다른 기능 구현을 위해 보류
		// 애플리케이션을 렌더링하세요.
		virtual void Render() = 0;

		// ===== Render 함수 자동화 이후 이것도 자동화 예정 =====
		// 애플리케이션에서 할당한 모든 리소스(렌더링 리소스 포함)를 해제하세요.
		virtual void Release() = 0;


		// ===== Render 함수 자동화 이후 이것도 자동화 예정 =====
		// 중요!
		// 반드시 이 함수가 호출되었을 때 렌더링 리소스들을 생성해야 합니다.
		virtual void CreateDeviceDependentResources() = 0;


		// ===== Render 함수 자동화 이후 이것도 자동화 예정 =====
		// 중요!
		// 내부적으로 ID2D1HwndRenderTarget 객체가 재생성되어야 하는 상황이 발생할 수 있습니다.
		// 이 경우 기존의 ID2D1HwndRenderTarget 객체로 생성한 렌더링 리소스들을 더 이상 사용할 수 없으므로
		// 렌더링 리소스들을 모두 해제해야 합니다.
		virtual void ReleaseDeviceDependentResources() = 0;
	};
	*/
}
