#pragma once
#include "Object\Derived\GameObject\GameObject.h"
#include "Components\Derived\GraphicComponents\Derived\Geometry\Derived\Polygon.h"

using namespace D2DEngine;

class Triangle : public GameObject
{
public:
	Triangle(const D2D1_POINT_2F shape[3]);
	Triangle(ActiveFlag flag, const wchar_t* name, const D2D1_POINT_2F shape[3], ObjectTag tag = ObjectTag::Default);
	virtual ~Triangle() override;

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