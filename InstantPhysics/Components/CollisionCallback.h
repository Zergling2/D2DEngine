#pragma once

#include <Windows.h>

namespace ip
{
	// MultithreadCollisionDetection으로 변경 후 TLS 메모리풀에서 할당
	struct CollisionNode
	{
		struct CollisionNode* pNext;
		class PhysicsObject* pOther;	// 충돌 상대
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
