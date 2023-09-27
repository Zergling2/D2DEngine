#pragma once
#include "Core\Scene.h"

D2DEngine::Scene* CreateTestScene02();

class TestScene02 : public D2DEngine::Scene
{
public:
	TestScene02();
	virtual ~TestScene02() override;


	// ========================================================================================
	// INTERFACE
	// 
	// 씬이 시작될 때 최초에 한 번만 호출됨
	// 씬에서 사용될 오브젝트들 생성 등...
	virtual void OnStart() override;

	// 중요!
	// 반드시 이 함수가 호출되었을 때 렌더링 리소스들을 생성해야 합니다.
	virtual void CreateDeviceDependentResources() override;

	// 중요!
	// 내부적으로 ID2D1HwndRenderTarget 객체가 재생성되어야 하는 상황이 발생할 수 있습니다.
	// 이 경우 기존의 ID2D1HwndRenderTarget 객체로 생성한 렌더링 리소스들을 더 이상 사용할 수 없으므로
	// 렌더링 리소스들을 모두 해제해야 합니다.
	virtual void ReleaseDeviceDependentResources() override;

	// 씬이 제거될 때 한 번만 호출됨
	virtual void OnDestroy() override;

	// ========================================================================================
private:
	D2DEngine::Camera* pCamera;
};
