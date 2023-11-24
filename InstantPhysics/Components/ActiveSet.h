#pragma once

#include "Common\Type.h"

namespace ip
{
	class ActiveSet
	{
	public:
		class ActiveSetAxis
		{
		public:
			typedef int axis;
			static const axis X = 0;
			static const axis Y = 1;
		};
	public:
		// ActiveSet(ActiveSetAxis::axis _axis);
		ActiveSet();
		~ActiveSet();
		inline class Collider** Get() const { return m_pBase; }
		inline int GetCount() const { return m_count; }
		inline int GetCommitSize() const { return m_commitSize; }
		void Add(class Collider* pCol);
		void Remove(Collider* pCol);
		void Clear();
		void Shrink();
		bool Destroy();
	private:
		void GrowUp();
	private:
		Collider** m_pBase;
		const void* m_boundary;
		int m_count;			// pointer count
		int m_commitSize;
	};
}
