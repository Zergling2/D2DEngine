#pragma once
#include "Object\Derived\GameObject\GameObject.h"
#include "Components\Derived\GraphicComponents\Derived\Geometry\Derived\Polygon.h"

using namespace D2DEngine;

class Car : public GameObject
{
public:
	Car();
	virtual ~Car() override;
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
	Graphic::Polygon* m_pShape;
};