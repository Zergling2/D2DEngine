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
		auto end = m_layers[i].m_objectList.end();		// ������Ʈ�� Update���� �߰��Ǳ� �� end iterator
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

Camera* Scene::SetRenderingCamera(Camera* pCamera)		// ���� ����Ʈ ���� �� ���� ī�޶� ����
{
	Camera* pOld = m_pRenderingCamera;
	m_pRenderingCamera = pCamera;
	return pOld;
}

void Scene::Initialize()
{
	// �⺻ zOrder�� enum class ����� ����
	
	for (UINT i = 0; i < static_cast<UINT>(LayerType::Count); i++)		// Update all layers in the scene. (Objects with scripts.)
		SetZOrder(static_cast<LayerType>(i), i);
}

void Scene::FixedUpdate()
{
	for (UINT i = 0; i < static_cast<UINT>(LayerType::Count); i++)		// Update all layers in the scene. (Objects with scripts.)
	{
		auto end = m_layers[i].m_objectList.end();		// ������Ʈ�� Update���� �߰��Ǳ� �� end iterator
		for (auto iter = m_layers[i].m_objectList.begin(); iter != end; ++iter)
		{
			(*iter)->FixedUpdate();
		}
	}
}

void Scene::Update()
{
	// 1. ���� API ȣ�� ���⼭ �Ұ���?
	// RigidBody ������Ʈ�� �߰��Ǵ� ���...
	// ���� ���� API ȣ���ϴ� ������ �۵� (Transform�� Directly/Indirectly �����ؾ� �� ��)

	// ���� API ���⼭ ȣ�� (240Hz?? or else...)




	for (UINT i = 0; i < static_cast<UINT>(LayerType::Count); i++)		// Update all layers in the scene. (Objects with scripts.)
	{
		auto end = m_layers[i].m_objectList.end();		// ������Ʈ�� Update���� �߰��Ǳ� �� end iterator
		for (auto iter = m_layers[i].m_objectList.begin(); iter != end; ++iter)
		{
			(*iter)->Update();
		}
	}
}

void Scene::Render()
{
	if (m_pRenderingCamera == nullptr)		// ��ϵ� ī�޶� ������ ������ ����
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

			// UI ó��
			// 1. ó������ ��ũ�� ��ǥ ���?
			// 2. �׻� ī�޶� ü�̽�
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

	list->erase(objIterator);		// ���� Object list���� ����
	m_rfdl.push_back(pGameObject);	// RFDL�� �߰�

	// + LateUpdate ���� RFDL ��ȸ�� OnDestroy ȣ�����ְ� �޸� ��������
	// OnDestroy�� isActive �÷��� �����ִ� ��츸 ȣ��
}

// =================================================
// ���� Object�� Component�� �ѱ����� ���Ƴ��Ƽ� ȣ�� �� ���� ������ Update �Լ�������, Render �Լ������� ����ǰ� �ִ�.
// ���߿� ����ȭ �ʿ�
// =================================================