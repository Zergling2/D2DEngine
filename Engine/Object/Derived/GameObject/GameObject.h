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
		// 매 프레임 호출
		// 컴포넌트 업데이트, ...
		virtual void Update() {}
		// =============================

		// =============================
		// Late update
		// 카메라 업데이트, else...
		// virtual void LateUpdate() {}
		// =============================

		// =============================
		// OnDestroy는 이전에 활성화되어있던 오브젝트에서만 호출됨!
		virtual void OnDestroy() {}
		// =============================

		// =============================
		// GraphicComponent 클래스를 상속한 객체들의 Render 함수를 이 함수를 오버로딩하여 내부에서 호출하면 됨.
		virtual void OnRender() {}
		// =============================

		// ===========================================================================================

		inline Transform& GetTransform() { return m_transform; }
	private:
		// components
		Transform m_transform;							// 기본 컴포넌트

		// 파생 클래스에서..
		// Sample
		// 
		// ip::Collider* m_pCollider;
	};
}
