#include "Scene.h"
#include "Core\Engine.h"

using namespace D2DEngine;

Scene::Scene()
	: m_pRenderingCamera(nullptr)
{
}

Scene::~Scene()
{
	for (UINT i = 0; i < static_cast<UINT>(LayerType::Count); i++)		// Update all layers in the scene. (Objects with scripts.)
	{
		auto end = m_layers[i].m_objectList.end();		// 오브젝트가 Update에서 추가되기 전 end iterator
		for (auto iter = m_layers[i].m_objectList.begin(); iter != end; ++iter)
		{
			(*iter)->OnDestroy();
			delete *iter;
		}
	}
}

void Scene::AddObject(GameObject* pObj, LayerType layer)
{
	if (static_cast<UINT>(layer) >= static_cast<UINT>(LayerType::Count))
		return;

	m_layers[static_cast<UINT>(layer)].m_objectList.push_back(pObj);
}

Camera* Scene::SetRenderingCamera(Camera* pCamera)		// 추후 뷰포트 설정 및 복수 카메라 지원
{
	Camera* pOld = m_pRenderingCamera;
	m_pRenderingCamera = pCamera;
	return pOld;
}

void Scene::Initialize()
{
	// 기본 zOrder는 enum class 선언과 동일
	
	for (UINT i = 0; i < static_cast<UINT>(LayerType::Count); i++)		// Update all layers in the scene. (Objects with scripts.)
		SetZOrder(static_cast<LayerType>(i), i);
}

void Scene::FixedUpdate()
{
	for (UINT i = 0; i < static_cast<UINT>(LayerType::Count); i++)		// Update all layers in the scene. (Objects with scripts.)
	{
		auto end = m_layers[i].m_objectList.end();		// 오브젝트가 Update에서 추가되기 전 end iterator
		for (auto iter = m_layers[i].m_objectList.begin(); iter != end; ++iter)
		{
			(*iter)->FixedUpdate();
		}
	}
}

void Scene::Update()
{
	// 1. 물리 API 호출 여기서 할건지?
	// RigidBody 컴포넌트가 추가되는 경우...
	// 물리 엔진 API 호출하는 식으로 작동 (Transform에 Directly/Indirectly 접근해야 할 듯)

	// 물리 API 여기서 호출 (240Hz?? or else...)




	for (UINT i = 0; i < static_cast<UINT>(LayerType::Count); i++)		// Update all layers in the scene. (Objects with scripts.)
	{
		auto end = m_layers[i].m_objectList.end();		// 오브젝트가 Update에서 추가되기 전 end iterator
		for (auto iter = m_layers[i].m_objectList.begin(); iter != end; ++iter)
		{
			(*iter)->Update();
		}
	}
}

void Scene::Render()
{
	if (m_pRenderingCamera == nullptr)		// 등록된 카메라 없으면 렌더링 생략
		return;

	D2D1::Matrix3x2F vpMatrix = m_pRenderingCamera->GetViewProjectionMatrix();
	D2DEngine::Engine::GetInstance().CacheVPMatrix(vpMatrix);

	for (UINT i = 0; i < static_cast<UINT>(LayerType::Count); i++)
	{
		auto end = m_layers[i].m_objectList.end();
		for (auto iter = m_layers[i].m_objectList.begin(); iter != end; ++iter)
		{
			// Frustum Culling Code..
			// ....

			// UI 처리
			// 1. 처음부터 스크린 좌표 사용?
			// 2. 항상 카메라 체이싱
			(*iter)->OnRender();
		}
	}
}

void Scene::GetObjectCountToBeUpdate()
{
	for (UINT i = 0; i < static_cast<UINT>(LayerType::Count); i++)
		m_updateCounter[i] = m_layers[i].m_objectList.size();
}

void Scene::MoveToRFDL(std::list<GameObject*>* list, std::list<GameObject*>::iterator objIterator)
{
	GameObject* pGameObject = *objIterator;

	list->erase(objIterator);		// 씬의 Object list에서 제거
	m_rfdl.push_back(pGameObject);	// RFDL에 추가

	// + LateUpdate 이후 RFDL 순회시 OnDestroy 호출해주고 메모리 해제까지
	// OnDestroy는 isActive 플래그 켜져있는 경우만 호출
}

// =================================================
// 현재 Object의 Component를 한군데에 몰아놓아서 호출 시 삼중 루프를 Update 함수에서도, Render 함수에서도 실행되고 있다.
// 나중에 최적화 필요
// =================================================