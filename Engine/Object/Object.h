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
		// �� ������ ȣ��
		// ������Ʈ ������Ʈ, ...
		virtual void Update() = 0;
		// =============================

		// =============================
		// Late update
		// ī�޶� ������Ʈ, else...
		// virtual void LateUpdate() = 0;
		// =============================

		// =============================
		// OnDestroy�� ������ Ȱ��ȭ�Ǿ��ִ� ������Ʈ������ ȣ���!
		virtual void OnDestroy() = 0;
		// =============================

		// =============================
		// GraphicComponent Ŭ������ ����� ��ü���� Render �Լ��� �� �Լ��� �����ε��Ͽ� ���ο��� ȣ���ϸ� ��.
		virtual void OnRender() = 0;
		// =============================

		// ===========================================================================================

		// inline void Destroy() { ====m_flag |= Object::DESTROY_FLAG;==== }		// ���� ī��Ʈ ���� ������ ������Ʈ ���� ���� ����


		inline const wchar_t* ToString() const { return m_name.c_str(); }

		inline void SetActive(ActiveFlag flag) { m_isActive = flag; }
	public:
		std::wstring m_name;
		ObjectTag m_tag;
	private:
		ActiveFlag m_isActive;
		// UINT m_refCnt;		// GameObject�� ���ԵǴ� ��� ������Ʈ�� ������� ����
	};
}
