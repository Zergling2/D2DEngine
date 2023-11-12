#pragma once
#include "Object\Derived\GameObject\GameObject.h"
#include "Components\Derived\GraphicComponents\Derived\Geometry\Derived\Polygon.h"

class Pentagon : public D2DEngine::GameObject
{
public:
	Pentagon(const D2D1_POINT_2F shape[5]);
	Pentagon(D2DEngine::ActiveFlag flag, const wchar_t* name, const D2D1_POINT_2F shape[5], D2DEngine::ObjectTag tag = D2DEngine::ObjectTag::Default);
	virtual ~Pentagon() override;

	ip::HullCollider* Collider() { return m_pCollider; }

	bool CreateCollider(const ip::math::Vector2 convexShapeVerticesCCW[5]);

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
	D2DEngine::Polygon* m_pShape;

	ip::HullCollider* m_pCollider;
};