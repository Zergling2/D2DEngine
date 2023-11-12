#pragma once

#include "Headers\Essential.h"
#include "Core\Layer.h"
#include "Core\Camera.h"

namespace D2DEngine
{
	class Scene abstract
	{
		friend class Engine;
		friend class SceneManager;
	protected:
		Scene();
		virtual ~Scene() = 0;
	public:
		// ========================================================================================
		// INTERFACE
		// 
		// 씬이 시작될 때 최초에 한 번만 호출됨
		// 씬에서 사용될 오브젝트들 생성 등...
		virtual void OnStart() = 0;

		// 중요!
		// 반드시 이 함수가 호출되었을 때 렌더링 리소스들을 생성해야 합니다.
		virtual void CreateDeviceDependentResources() = 0;

		// 중요!
		// 내부적으로 ID2D1HwndRenderTarget 객체가 재생성되어야 하는 상황이 발생할 수 있습니다.
		// 이 경우 기존의 ID2D1HwndRenderTarget 객체로 생성한 렌더링 리소스들을 더 이상 사용할 수 없으므로
		// 렌더링 리소스들을 모두 해제해야 합니다.
		virtual void ReleaseDeviceDependentResources() = 0;
		
		// 씬이 제거될 때 한 번만 호출됨
		virtual void OnDestroy() = 0;

		// ========================================================================================

		void AddObject(GameObject* pObj, LayerType layer);

		// return old camera pointer
		Camera* SetRenderingCamera(Camera* pCamera);		// 추후 뷰포트 설정 및 복수 카메라 지원
		inline Camera& GetRenderingCamera() const { return *m_pRenderingCamera; }

		inline void SetZOrder(LayerType type, int zOrder) { m_layers[static_cast<UINT>(type)].SetZOrder(zOrder); }
		inline int GetZOrder(LayerType type) const { m_layers[static_cast<UINT>(type)].GetZOrder(); }
	private:
		void Initialize();				// SceneManager에서 호출
		void FixedUpdate();				// Engine에서 호출
		void Update();					// Engine에서 호출
		// void LateUpdate();			// Engine에서 호출
		void Render();					// Engine에서 호출
	private:		// Scene에서 사용할 멤버 함수

		// Destroy 호출 대기 오브젝트들
		void MoveToRFDL(std::list<GameObject*>* list, std::list<GameObject*>::iterator objIterator);
		void GetObjectCountToBeUpdate();
	private:
		Camera* m_pRenderingCamera;			// Current rendering camera
		Layer m_layers[static_cast<UINT>(LayerType::Count)];
		size_t m_updateCounter[static_cast<UINT>(LayerType::Count)];		// Number of objects required to be updated
		std::list<GameObject*> m_rfdl;			// Reserved for destruction list
	};
}
