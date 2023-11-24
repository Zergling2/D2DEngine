#pragma once

#include "Common\IPParameter.h"
#include "Common\Type.h"
#include <Windows.h>

namespace ip
{
	template<typename T>
	class IPLargeArray
	{
	public:
		IPLargeArray(size_t maxSize, size_t addiAllocSize);
		size_t Shrink_to_fit();		// Return shrinked
		virtual bool Add(T& val) = 0;
		virtual bool Erase(T& val) = 0;
	protected:
		bool GrowUp();
	protected:
		T* m_pBase;
		void* m_boundary;
		size_t m_count;
		size_t m_addiAllocSize;
		size_t m_commitSize;
		size_t m_maxSize;
	};

	template<typename T>
	IPLargeArray<T>::IPLargeArray(size_t maxSize, size_t addiAllocSize)
	{

	}

	template<typename T>
	size_t IPLargeArray<T>::Shrink_to_fit()
	{
		intptr_t allocatedBoundary = reinterpret_cast<intptr_t>(m_boundary);
		intptr_t usingPageBoundary =
			(reinterpret_cast<intptr_t>(&m_pBase[m_count]) + m_addiAllocSize - 1) & m_addiAllocSize;

		// PAGE_NOACCESS 플래그로 다시 변경
		m_boundary = VirtualAlloc(reinterpret_cast<void*>(usingPageBoundary), allocatedBoundary - usingPageBoundary, MEM_RESERVE, PAGE_NOACCESS);
	}

	template<typename T>
	bool IPLargeArray<T>::GrowUp()
	{
		if (reinterpret_cast<size_t>(m_boundary) >= reinterpret_cast<size_t>(m_pBase) + m_maxSize)
			*reinterpret_cast<int*>(0x00000000) = 0;

		LPCVOID result = VirtualAlloc(const_cast<void*>(m_boundary), m_addiAllocSize, MEM_COMMIT, PAGE_READWRITE);
		if (result == nullptr)
			*reinterpret_cast<int*>(0x00000000) = 0;

		m_boundary = reinterpret_cast<void*>(reinterpret_cast<size_t>(m_boundary) + m_addiAllocSize);
		m_commitSize += m_addiAllocSize;
	}
}
