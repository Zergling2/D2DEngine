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
	// ���� ���۵� �� ���ʿ� �� ���� ȣ���
	// ������ ���� ������Ʈ�� ���� ��...
	virtual void OnStart() override;

	// �߿�!
	// �ݵ�� �� �Լ��� ȣ��Ǿ��� �� ������ ���ҽ����� �����ؾ� �մϴ�.
	virtual void CreateDeviceDependentResources() override;

	// �߿�!
	// ���������� ID2D1HwndRenderTarget ��ü�� ������Ǿ�� �ϴ� ��Ȳ�� �߻��� �� �ֽ��ϴ�.
	// �� ��� ������ ID2D1HwndRenderTarget ��ü�� ������ ������ ���ҽ����� �� �̻� ����� �� �����Ƿ�
	// ������ ���ҽ����� ��� �����ؾ� �մϴ�.
	virtual void ReleaseDeviceDependentResources() override;

	// ���� ���ŵ� �� �� ���� ȣ���
	virtual void OnDestroy() override;

	// ========================================================================================
private:
	D2DEngine::Camera* pCamera;
};
