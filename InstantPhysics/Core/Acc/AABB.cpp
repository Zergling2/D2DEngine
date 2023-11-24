#include "AABB.h"
#include "Common\IPParameter.h"
#include "Components\PhysicsObject.h"
#include <Windows.h>

using namespace ip;

AABBArray::AABBArray()
	: m_count(0)
{
	// SYSTEM_INFO si;
	// GetSystemInfo(&si);

	m_pBase = reinterpret_cast<AABBNode**>(VirtualAlloc(NULL, parameter::ACTIVE_SET_MAX_SIZE, MEM_RESERVE, PAGE_READWRITE));
	m_boundary = m_pBase;

	GrowUp();
}

AABBArray::~AABBArray()
{
}

void AABBArray::Add(const PhysicsObject* pPhysObj)
{
	Collider* pCol = pPhysObj->m_pCollider;
	if (&m_pBase[m_count] >= m_boundary)
		GrowUp();

	m_pBase[m_count] = pCol->m_AABB.pMinXNode;
	m_pBase[m_count + 1] = pCol->m_AABB.pMaxXNode;
	m_count += 2;
}

void AABBArray::Sort()
{
	for (int i = 1; i < m_count; i++)
	{
		AABBNode* keyPointer = m_pBase[i];
		Type keyType = m_pBase[i]->type;
		real key = m_pBase[i]->value;
		int j = i - 1;

		while (j >= 0)
		{
			real val = m_pBase[j]->value;

			if (val < key)
				break;

			if (val > key || (val == key && m_pBase[j]->type == MAX && keyType == MIN))
				m_pBase[j + 1] = m_pBase[j];
			else
				break;

			j--;
		}

		m_pBase[j + 1] = keyPointer;
	}
}

void AABBArray::Shrink()
{
	intptr_t allocatedBoundary = reinterpret_cast<intptr_t>(m_boundary);
	intptr_t usingPageBoundary = 
		(reinterpret_cast<intptr_t>(&m_pBase[m_count]) + parameter::ADDITIONAL_ALLOC_SIZE - 1) & parameter::ADDITIONAL_ALLOC_SIZE;

	// PAGE_NOACCESS 플래그로 다시 변경
	m_boundary = VirtualAlloc(reinterpret_cast<void*>(usingPageBoundary), allocatedBoundary - usingPageBoundary, MEM_RESERVE, PAGE_NOACCESS);
}

bool AABBArray::Destroy()
{
	BOOL ret = VirtualFree(m_pBase, 0, MEM_RELEASE);
	m_pBase = nullptr;
	m_boundary = nullptr;
	m_count = 0;
	m_commitSize = 0;

	return ret;
}

void AABBArray::GrowUp()
{
	if (reinterpret_cast<intptr_t>(m_boundary) >= reinterpret_cast<intptr_t>(m_pBase) + parameter::AABBSWEEPLIST_MAX_SIZE)
		*reinterpret_cast<int*>(0x00000000) = 0;

	LPCVOID result = VirtualAlloc(const_cast<void*>(m_boundary), parameter::ADDITIONAL_ALLOC_SIZE, MEM_COMMIT, PAGE_READWRITE);
	if (result == NULL)
		*reinterpret_cast<int*>(0x00000000) = 0;

	m_boundary = reinterpret_cast<void*>(reinterpret_cast<intptr_t>(m_boundary) + parameter::ADDITIONAL_ALLOC_SIZE);
	m_commitSize += parameter::ADDITIONAL_ALLOC_SIZE;
}
