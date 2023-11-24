#include "ActiveSet.h"
#include "Common\IPParameter.h"
#include "Components\Collider.h"
#include <Windows.h>

using namespace ip;

ActiveSet::ActiveSet()
	: m_pBase(nullptr)
	, m_boundary(nullptr)
	, m_count(0)
	, m_commitSize(0)
{
	// SYSTEM_INFO si;
	// GetSystemInfo(&si);

	m_pBase = reinterpret_cast<Collider**>(VirtualAlloc(NULL, parameter::ACTIVE_SET_MAX_SIZE, MEM_RESERVE, PAGE_READWRITE));
	m_boundary = m_pBase;

	GrowUp();
}

ActiveSet::~ActiveSet()
{
	VirtualFree(m_pBase, 0, MEM_RELEASE);
}

void ActiveSet::Add(Collider* pCol)
{
	if (&m_pBase[m_count] >= m_boundary)
		GrowUp();

	m_pBase[m_count] = pCol;
	pCol->m_activeSetIndex = m_count;
	m_count++;
}

void ActiveSet::Remove(Collider* pCol)
{
#ifdef _DEBUG
	if ((signed int)m_count <= 0)
		*reinterpret_cast<int*>(0x0) = 0;
#endif // _DEBUG

	int lastIndex = m_count - 1;
	int removeIndex = pCol->m_activeSetIndex;
	pCol->m_activeSetIndex = -1;

	if (lastIndex != removeIndex)
	{
		m_pBase[lastIndex]->m_activeSetIndex = removeIndex;
		m_pBase[removeIndex] = m_pBase[lastIndex];
	}

	m_count--;
}

void ActiveSet::Clear()
{
	m_count = 0;
}

void ActiveSet::Shrink()
{
	intptr_t allocatedBoundary = reinterpret_cast<intptr_t>(m_boundary);
	intptr_t usingPageBoundary = 
		(reinterpret_cast<intptr_t>(&m_pBase[m_count]) + parameter::ADDITIONAL_ALLOC_SIZE - 1) & parameter::ADDITIONAL_ALLOC_SIZE;
	
	// PAGE_NOACCESS 플래그로 다시 변경
	m_boundary = VirtualAlloc(reinterpret_cast<void*>(usingPageBoundary), allocatedBoundary - usingPageBoundary, MEM_RESERVE, PAGE_NOACCESS);
}

bool ActiveSet::Destroy()
{
	BOOL ret = VirtualFree(m_pBase, 0, MEM_RELEASE);
	m_pBase = nullptr;
	m_boundary = nullptr;
	m_count = 0;
	m_commitSize = 0;

	return ret;
}

void ActiveSet::GrowUp()
{
	if (reinterpret_cast<intptr_t>(m_boundary) >= reinterpret_cast<intptr_t>(m_pBase) + parameter::AABBSWEEPLIST_MAX_SIZE)
		*reinterpret_cast<int*>(0x00000000) = 0;

	LPCVOID result = VirtualAlloc(const_cast<void*>(m_boundary), parameter::ADDITIONAL_ALLOC_SIZE, MEM_COMMIT, PAGE_READWRITE);
	if (result == NULL)
		*reinterpret_cast<int*>(0x00000000) = 0;

	m_boundary = reinterpret_cast<void*>(reinterpret_cast<intptr_t>(m_boundary) + parameter::ADDITIONAL_ALLOC_SIZE);
	m_commitSize += parameter::ADDITIONAL_ALLOC_SIZE;
}
