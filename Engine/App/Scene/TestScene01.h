#pragma once
#include "Core\Scene.h"

D2DEngine::Scene* CreateTestScene01();

class TestScene01 : public D2DEngine::Scene
{
public:
	TestScene01();
	virtual ~TestScene01() override;


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
	D2DEngine::Camera* m_pCamera;
};
