#pragma once

#include <Windows.h>

namespace ip
{
	// MultithreadCollisionDetection���� ���� �� TLS �޸�Ǯ���� �Ҵ�
	struct CollisionNode
	{
		struct CollisionNode* pNext;
		class PhysicsObject* pOther;	// �浹 ���
	};

	class TLSCollisionNodePool
	{
	public:
		static bool Initialize();		// tls index allocation
		static __forceinline DWORD GetTlsIndex() { return TLSCollisionNodePool::s_tlsIndex; }
	private:
		static DWORD s_tlsIndex;
	};
}
