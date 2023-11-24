#pragma once

#include "Headers\Essential.h"
#include "Core\TagManager.h"

namespace D2DEngine
{
	struct ActiveFlag
	{
		inline explicit ActiveFlag(bool flag) : value(flag) {}
		bool value;
	};

	// Base class for all objects D2DEngine can reference.
	class Object : public ip::PhysicsObject
	{
		friend class Scene;
	protected:
		Object(ActiveFlag flag, const wchar_t* name, ObjectTag tag = ObjectTag::Default);
		Object(ActiveFlag flag, const std::wstring& name, ObjectTag tag = ObjectTag::Default);
		virtual ~Object();


		// ===========================================================================================
		virtual void Start() = 0;

		virtual void Awake() = 0;

		// =============================
		// Fixed time
		virtual void FixedUpdate() = 0;
		// =============================

		// =============================
		// 매 프레임 호출
		// 컴포넌트 업데이트, ...
		virtual void Update() = 0;
		// =============================

		// =============================
		// Late update
		// 카메라 업데이트, else...
		// virtual void LateUpdate() = 0;
		// =============================

		// =============================
		// OnDestroy는 이전에 활성화되어있던 오브젝트에서만 호출됨!
		virtual void OnDestroy() = 0;
		// =============================

		// =============================
		// GraphicComponent 클래스를 상속한 객체들의 Render 함수를 이 함수를 오버로딩하여 내부에서 호출하면 됨.
		virtual void OnRender() = 0;
		// =============================

		// ===========================================================================================

		// inline void Destroy() { ====m_flag |= Object::DESTROY_FLAG;==== }		// 참조 카운트 구현 전까지 오브젝트 제거 구현 보류


		inline const wchar_t* ToString() const { return m_name.c_str(); }

		inline void SetActive(ActiveFlag flag) { m_isActive = flag; }
	public:
		std::wstring m_name;
		ObjectTag m_tag;
	private:
		ActiveFlag m_isActive;
		// UINT m_refCnt;		// GameObject에 포함되는 모든 컴포넌트들 수명까지 영향
	};
}
