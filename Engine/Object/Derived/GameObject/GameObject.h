#pragma once

#include "Headers\Essential.h"
#include "Object\Object.h"
#include "Components\Derived\Transform.h"
#include "InstantPhysics.h"

namespace D2DEngine
{
	class GameObject abstract : public Object
	{
		friend class Scene;
	public:
		// ==================================================
		// Finds a GameObject by name and returns it.
		// static GameObject* Find(const wchar_t* name);
		// static GameObject* Find(const std::wstring& name);
		// ==================================================

		// ==================================================
		// Note: This method returns the first GameObject it finds with the specified tag.
		// If a scene contains multiple active GameObjects with the specified tag,
		// there is no guarantee this method will return a specific GameObject.
		// static GameObject* FindWithTag(const char* tag);
		// static GameObject* FindWithTag(const std::string& tag);
		// ==================================================
	public:
		GameObject();
		GameObject(ActiveFlag flag, const wchar_t* name, ObjectTag tag = ObjectTag::Default);
		GameObject(ActiveFlag flag, const std::wstring& name, ObjectTag tag = ObjectTag::Default);
		virtual ~GameObject() = 0;

		// ===========================================================================================
		virtual void Start() {}

		virtual void Awake() {}

		// =============================
		// Fixed time
		virtual void FixedUpdate() {}
		// =============================

		// =============================
		// �� ������ ȣ��
		// ������Ʈ ������Ʈ, ...
		virtual void Update() {}
		// =============================

		// =============================
		// Late update
		// ī�޶� ������Ʈ, else...
		// virtual void LateUpdate() {}
		// =============================

		// =============================
		// OnDestroy�� ������ Ȱ��ȭ�Ǿ��ִ� ������Ʈ������ ȣ���!
		virtual void OnDestroy() {}
		// =============================

		// =============================
		// GraphicComponent Ŭ������ ����� ��ü���� Render �Լ��� �� �Լ��� �����ε��Ͽ� ���ο��� ȣ���ϸ� ��.
		virtual void OnRender() {}
		// =============================

		// ===========================================================================================

		inline Transform& GetTransform() { return m_transform; }
	private:
		// components
		Transform m_transform;							// �⺻ ������Ʈ

		// �Ļ� Ŭ��������..
		// Sample
		// 
		// ip::Collider* m_pCollider;
	};
}
