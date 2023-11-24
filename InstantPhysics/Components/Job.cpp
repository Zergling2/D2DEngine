#include "Job.h"
#include "Components\PhysicsObject.h"
#include "Components\Manifold.h"
#include "Components\PhysicsWorld.h"
#include <Windows.h>

using namespace ip;

void MTUpdatePhysicsObject::Do()
{
	for (int i = 0; i < m_objCount; i++)
	{
		PhysicsObject* pPhysObj = *(m_pBeginAddr + i);
		switch (pPhysObj->m_rigidBody.m_type)
		{
		case Unknown:
		case Static:
			continue;
		case Kinematic:
			m_pPhysWorld->KinematicUpdate(pPhysObj);
			break;
		case Dynamic:
			m_pPhysWorld->Integrate(pPhysObj);
			break;
		}
		pPhysObj->m_pCollider->UpdateCollider();
	}

	if (m_pPhysWorld->m_threadCount == _InterlockedIncrement(&m_pPhysWorld->m_fc))
		SetEvent(m_pPhysWorld->m_MTUpdatePhysObjFinishEvent);
}

void MTNPCollisionDetection::Do()
{
	Manifold m;
	bool ret = PhysicsWorld::cdft[m_pColliderA->m_type][m_pColliderB->m_type](m, *m_pColliderA, *m_pColliderB);

	if (ret)
	{
		// ���� �����忡�� ����Ʈ ��ġ�� ���߿� ����?
		// SRWLock���� �׳� ���Ϳ� �ֱ�?
		// ������ ���ڰ� �� ������
	}
}

void MTDoNothing::Do()
{
	// Do nothing
	return;
}
