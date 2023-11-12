#pragma once
#include "Object\Derived\GameObject\GameObject.h"
#include "Components\Derived\GraphicComponents\Derived\Geometry\Derived\Rectangle.h"

class ControllableRectangle : public D2DEngine::GameObject
{
public:
	ControllableRectangle(const D2D_RECT_F& shape);
	ControllableRectangle(D2DEngine::ActiveFlag flag, const wchar_t* name, const D2D_RECT_F& shape, D2DEngine::ObjectTag tag = D2DEngine::ObjectTag::Default);
	virtual ~ControllableRectangle() override;

	ip::HullCollider* Collider() { return m_pCollider; }

	bool CreateCollider(ip::math::Size2 size);

	bool RemoveCollider();

	// ===========================================================================================
	virtual void Start() override;

	virtual void Awake() override;

	// =============================
	// Fixed time
	virtual void FixedUpdate() override;
	// =============================

	// =============================
	// �� ������ ȣ��
	// ������Ʈ ������Ʈ, ...
	virtual void Update() override;
	// =============================

	// =============================
	// Late update
	// ī�޶� ������Ʈ, else...
	// virtual void LateUpdate() override;
	// =============================

	// =============================
	// OnDestroy�� ������ Ȱ��ȭ�Ǿ��ִ� ������Ʈ������ ȣ���!
	virtual void OnDestroy() override;
	// =============================

	// =============================
	// GraphicComponent Ŭ������ ����� ��ü���� Render �Լ��� �� �Լ��� �����ε��Ͽ� ���ο��� ȣ���ϸ� ��.
	virtual void OnRender() override;
	// =============================

	// ===========================================================================================
private:
	D2DEngine::Rectangle* m_pShape;

	ip::HullCollider* m_pCollider;
};