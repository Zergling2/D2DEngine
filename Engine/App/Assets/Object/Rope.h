#pragma once
#include "Object\Derived\GameObject\GameObject.h"

using namespace D2DEngine;

class Rope : public GameObject
{
public:
	Rope(float segmentRadius, int segmentCount, float segmentLength);
	virtual ~Rope() override;

	void CreateRopeSegment();
	void UpdateSegment();
	void ApplyConstraint();

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
	class RopeSegment** m_ppRopeSegment;
	float m_segmentRadius;
	int m_segmentCount;
	float m_segmentLength;
};