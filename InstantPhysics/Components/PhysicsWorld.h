#pragma once

#include "Common\Type.h"
#include "Common\IPParameter.h"
#include "Components\ActiveSet.h"
#include "Common\Math2D.h"
#include "Components\Collider.h"
#include "Components\RaycastHit2D.h"
#include "Components\Job.h"
#ifdef _DEBUG
#include <list>
#endif // _DEBUG
#include <vector>

namespace ip
{
	class PhysicsWorld
	{
		friend class MTUpdatePhysicsObject;
		friend class MTNPCollisionDetection;
	public:
		PhysicsWorld();
		~PhysicsWorld();

		bool Initialize();

		inline const math::Vector2 GetGravity() const { return m_gravity; }
		inline void SetGravity(const math::Vector2& g) { m_gravity = g; }
		inline void SetGravity(real x, real y) { m_gravity.x = x; m_gravity.y = y; }

		void Update();
		void AddPhysicsObject(class PhysicsObject* pPhysObj);
		void RemovePhysicsObject(class PhysicsObject* pPhysObj);
		RaycastHit2D Raycast(const math::Vector2& origin);
		RaycastHit2D Raycast(const math::Vector2& origin, const CollisionLayer mask);
		RaycastHit2D Raycast(const math::Vector2& origin, const math::Vector2& direction);
		RaycastHit2D Raycast(const math::Vector2& origin, const math::Vector2& direction, const CollisionLayer mask);
	private:
		void UpdatePhysicsObject();
		void Integrate(class PhysicsObject* pPhysObj);
		void KinematicUpdate(class PhysicsObject* pPhysObj);
		void BroadPhase();
		void NarrowPhase();
		void ResolveCollision(const struct Manifold& manifold);

		void UpdatePhysicsObjectMultiThread(PhysicsObject** ppBeginAddr, size_t size);
		void UpdatePhysicsObjectSingleThread(PhysicsObject** ppBeginAddr, size_t size);
	private:
		static bool CheckCirclevsCircleCollision(struct Manifold& manifold, const class Collider& circleColliderA, const class Collider& circleColliderB);
		static bool CheckCapsulevsCircleCollision(struct Manifold& manifold, const class Collider& capsuleCollider, const class Collider& circleCollider);
		static bool CheckCirclevsCapsuleCollision(struct Manifold& manifold, const class Collider& circleCollider, const class Collider& capsuleCollider);
		static bool CheckCapsulevsCapsuleCollision(struct Manifold& manifold, const class Collider& capsuleColliderA, const class Collider& capsuleColliderB);
		static bool CheckCapsulevsHullCollision(struct Manifold& manifold, const class Collider& capsuleCollider, const class Collider& hullCollider);
		static bool CheckHullvsCapsuleCollision(struct Manifold& manifold, const class Collider& hullCollider, const class Collider& capsuleCollider);
		static bool CheckHullvsHullCollision(struct Manifold& manifold, const class Collider& hullColliderA, const class Collider& hullColliderB);
		static bool CheckCirclevsHullCollision(struct Manifold& manifold, const class Collider& circleCollider, const class Collider& hullCollider);
		static bool CheckHullvsCircleCollision(struct Manifold& manifold, const class Collider& hullCollider, const class Collider& circleCollider);
		static const math::Vector2 GetClosestPointOnLineSegementFromOrigin(const math::Vector2& a, const math::Vector2& b);
		static void GetClosestVectorBetweenCircleCenterAndHull(const math::Vector2 point, const class HullCollider& hull, math::Vector2& v, real& dist);
		static void FaceQueryHullvsHull(struct Query& query, const class HullCollider& ref, const class HullCollider& inc);
		static void FaceQueryHullvsCircle(struct Query& query, const class HullCollider& ref, const class CircleCollider& inc);
		static void ComputeHullvsHullContactPoint(struct Manifold& manifold, uint16_t refFaceIndex, uint16_t incVertexIndex);
		// static bool CheckCirclevsBoxCollision(struct Manifold& manifold, const class Collider& circleCollider, const class Collider& boxCollider);
		// static bool CheckBoxvsCircleCollision(struct Manifold& manifold, const class Collider& boxCollider, const class Collider& circleCollider);
		// static bool CheckBoxvsBoxCollision(struct Manifold& manifold, const class Collider& boxColliderA, const class Collider& boxColliderB);
		// static bool CheckBoxvsHullCollision(struct Manifold& manifold, const class Collider& boxCollider, const class Collider& hullCollider);
		// static bool CheckHullvsBoxCollision(struct Manifold& manifold, const class Collider& hullCollider, const class Collider& boxCollider);
		// static void FaceQueryBoxvsHull(struct Query& query, const class BoxCollider& ref, const class HullCollider& inc);
		// static void FaceQueryHullvsBox(struct Query& query, const class HullCollider& ref, const class BoxCollider& inc);
		// static void FaceQueryBoxvsBox(struct Query& query, const class BoxCollider& ref, const class BoxCollider& inc);
		// static void ComputeBoxvsBoxContactPoint(struct Manifold& manifold, uint16_t refFaceIndex, uint16_t incVertexIndex);
		// static void ComputeBoxvsHullContactPoint(struct Manifold& manifold, uint16_t refFaceIndex, uint16_t incVertexIndex);
		// static void ComputeHullvsBoxContactPoint(struct Manifold& manifold, uint16_t refFaceIndex, uint16_t incVertexIndex);
		static unsigned int __stdcall WorkerThreadEntry(void* param);
	public:
#ifdef _DEBUG
		struct ContactPoint
		{
		public:
			ContactPoint(real _x, real _y) : x(_x), y(_y) {}
			real x, y;
		};
		static std::list<ContactPoint> m_cpList;
#endif // _DEBUG
	private:
		typedef bool (*CDF)(struct Manifold& manifold, const class Collider& colliderA, const class Collider& colliderB);
		static CDF cdft[COLLIDER_TYPE_COUNT][COLLIDER_TYPE_COUNT];
		math::Vector2 m_gravity;
		std::vector<class PhysicsObject*> m_physObjs;
		AABBArray m_AABBArray;
		ActiveSet m_activeSet;
		std::vector<std::pair<class Collider*, class Collider*>> m_collisionList;	// MemPool 대체
		std::vector<std::pair<class Collider*, class Collider*>> m_npList;			// MemPool 대체
		MTDoNothing m_MTDoNothingJob;		// thread safe
		MTUpdatePhysicsObject m_MTUpdatePhysObjJobArr[parameter::MAX_THREADS];
		MTNPCollisionDetection m_MTNPCDJobArr[parameter::MAX_THREADS];
		bool m_mtMode;
		void* m_MTUpdatePhysObjFinishEvent;
		void* m_threads[parameter::MAX_THREADS];
		long m_threadCount;
		volatile bool m_terminateMTFlag;
		/*alignas(64)*/
		volatile long m_fc;		// MTUpdatePhysicsObject job finish counter
	};
}
