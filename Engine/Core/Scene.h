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
		// ���� ���۵� �� ���ʿ� �� ���� ȣ���
		// ������ ���� ������Ʈ�� ���� ��...
		virtual void OnStart() = 0;

		// �߿�!
		// �ݵ�� �� �Լ��� ȣ��Ǿ��� �� ������ ���ҽ����� �����ؾ� �մϴ�.
		virtual void CreateDeviceDependentResources() = 0;

		// �߿�!
		// ���������� ID2D1HwndRenderTarget ��ü�� ������Ǿ�� �ϴ� ��Ȳ�� �߻��� �� �ֽ��ϴ�.
		// �� ��� ������ ID2D1HwndRenderTarget ��ü�� ������ ������ ���ҽ����� �� �̻� ����� �� �����Ƿ�
		// ������ ���ҽ����� ��� �����ؾ� �մϴ�.
		virtual void ReleaseDeviceDependentResources() = 0;
		
		// ���� ���ŵ� �� �� ���� ȣ���
		virtual void OnDestroy() = 0;

		// ========================================================================================

		void AddObject(GameObject* pObj, LayerType layer);

		// return old camera pointer
		Camera* SetRenderingCamera(Camera* pCamera);		// ���� ����Ʈ ���� �� ���� ī�޶� ����
		inline Camera& GetRenderingCamera() const { return *m_pRenderingCamera; }

		inline void SetZOrder(LayerType type, int zOrder) { m_layers[static_cast<UINT>(type)].SetZOrder(zOrder); }
		inline int GetZOrder(LayerType type) const { m_layers[static_cast<UINT>(type)].GetZOrder(); }
	private:
		void Initialize();				// SceneManager���� ȣ��
		void FixedUpdate();				// Engine���� ȣ��
		void Update();					// Engine���� ȣ��
		// void LateUpdate();			// Engine���� ȣ��
		void Render();					// Engine���� ȣ��
	private:		// Scene���� ����� ��� �Լ�

		// Destroy ȣ�� ��� ������Ʈ��
		void MoveToRFDL(std::list<GameObject*>* list, std::list<GameObject*>::iterator objIterator);
		void GetObjectCountToBeUpdate();
	private:
		Camera* m_pRenderingCamera;			// Current rendering camera
		Layer m_layers[static_cast<UINT>(LayerType::Count)];
		size_t m_updateCounter[static_cast<UINT>(LayerType::Count)];		// Number of objects required to be updated
		std::list<GameObject*> m_rfdl;			// Reserved for destruction list
	};
}
