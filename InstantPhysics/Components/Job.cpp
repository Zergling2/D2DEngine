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
		// 개별 스레드에서 리스트 합치고 나중에 병합?
		// SRWLock쓰고 그냥 벡터에 넣기?
		// 성능은 전자가 더 좋을듯
	}
}

void MTDoNothing::Do()
{
	// Do nothing
	return;
}
