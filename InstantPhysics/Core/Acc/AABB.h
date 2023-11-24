#pragma once

#include "Common\Type.h"

namespace ip
{
	struct AABB
	{
	public:
		struct AABBNode* pMinXNode;
		struct AABBNode* pMaxXNode;
		real minY;
		real maxY;
	};

	enum Type
	{
		MIN,
		MAX
	};
	struct AABBNode
	{
	public:
		AABBNode(class Collider* _pCollider, Type _type)
			: pCollider(_pCollider)
			, type(_type)
			, value(real(0.0))
		{
		}
		class Collider* pCollider;
		Type type;
		real value;
	};

	class AABBArray
	{
	public:
		AABBArray();
		~AABBArray();
		void Add(const class PhysicsObject* pPhysObj);
		inline AABBNode** Get() const { return m_pBase; }
		inline int GetCount() const { return m_count; }
		inline int GetCommitSize() const { return m_commitSize; }
		// void Remove(class PhysicsObject* pPhysObj);		// << SHIFT -> memory copy
		void Sort();
		void Shrink();
		bool Destroy();
	private:
		void GrowUp();
	private:
		AABBNode** m_pBase;
		const void* m_boundary;
		int m_count;			// pointer count
		int m_commitSize;
	};
}
