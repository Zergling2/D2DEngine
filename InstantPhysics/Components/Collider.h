#pragma once

#include "Common\Math2D.h"
#include "Common\LayerCollisionMatrix.h"
#include "Core\Acc\AABB.h"

namespace ip
{
	typedef uint8_t ColliderType;
	static constexpr ColliderType CIRCLE = 0;
	static constexpr ColliderType CAPSULE = 1;
	static constexpr ColliderType HULL = 2;
	static constexpr ColliderType CHAIN = 3;
	static constexpr ColliderType COLLIDER_TYPE_COUNT = 4;

	class Collider abstract
	{
		friend class PhysicsWorld;
		friend class PhysicsObject;
		friend class MTUpdatePhysicsObject;
		friend class Simplex;
		friend class AABBArray;
		friend class ActiveSet;
	public:
		__forceinline class PhysicsObject& GetPhysicsObject() const { return *m_pPhysObj; }
		__forceinline void Enable() { m_isDead = false; }
		__forceinline void Disable() { m_isDead = true; }
		__forceinline void TriggerOn() { m_isTrigger = true; }
		__forceinline void TriggerOff() { m_isTrigger = false; }
		__forceinline bool isTrigger() const { return m_isTrigger; }
		__forceinline CollisionLayer GetLayer() const { return m_layer; }
	protected:
		Collider(class PhysicsObject* pPhysObj, ColliderType type, CollisionLayer layer)
			: m_pPhysObj(pPhysObj)
			, m_type(type)
			, m_layer(layer)
			, m_isDead(false)
			, m_isTrigger(false)
			, m_reserved(0)
			, m_activeSetIndex(-1)
		{
			m_AABB.pMinXNode = nullptr;
			m_AABB.pMaxXNode = nullptr;
		}
		virtual ~Collider();
		bool Initialize();		// Allocate AABB Node
		virtual void UpdateCollider() = 0;
	private:
		bool CreateAABBNode();
		void DeleteAABBNode();
		virtual const math::Vector2 SupportPoint(const math::Vector2& direction) const = 0;
	public:
		class PhysicsObject* m_pPhysObj;
		CollisionLayer m_layer;
		ColliderType m_type;
		bool m_isDead;
		bool m_isTrigger;
		uint8_t m_reserved;
		int m_activeSetIndex;
		AABB m_AABB;
	};
}