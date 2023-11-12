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
	// 매 프레임 호출
	// 컴포넌트 업데이트, ...
	virtual void Update() override;
	// =============================

	// =============================
	// Late update
	// 카메라 업데이트, else...
	// virtual void LateUpdate() override;
	// =============================

	// =============================
	// OnDestroy는 이전에 활성화되어있던 오브젝트에서만 호출됨!
	virtual void OnDestroy() override;
	// =============================

	// =============================
	// GraphicComponent 클래스를 상속한 객체들의 Render 함수를 이 함수를 오버로딩하여 내부에서 호출하면 됨.
	virtual void OnRender() override;
	// =============================

	// ===========================================================================================
private:
	D2DEngine::Polygon* m_pShape;

	ip::HullCollider* m_pCollider;
};