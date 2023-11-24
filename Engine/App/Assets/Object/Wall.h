#pragma once
#include "Object\Derived\GameObject\GameObject.h"
#include "Components\Derived\GraphicComponents\Derived\Geometry\Derived\Rectangle.h"

using namespace D2DEngine;

class Wall : public GameObject
{
public:
	Wall(const D2D_RECT_F& shape, float speed, float movement, const ip::math::Vector2& dir);
	virtual ~Wall() override;

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
	Graphic::Rectangle* m_pShape;
	float m_speed;
	float m_dist;
	float m_movement;
	ip::math::Vector2 m_dir;
};