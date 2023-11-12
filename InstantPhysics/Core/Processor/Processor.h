#pragma once

#include "Type.h"
#include "Common\Math2D.h"
#include "Components\Collider.h"
#include "Components\Simplex.h"
#include <list>

namespace ip
{
	class Processor
	{
	public:
		Processor();
		~Processor();

		// bool Initialize();

		void Update();
		class CircleCollider* CreateCircleCollider(real radius);
		class HullCollider* CreateBoxCollider(math::Size2 size);
		class HullCollider* CreateHullCollider(const math::Vector2 convexShapeVerticesCCW[], uint16_t vertexCount);
		bool RemoveCollider(class Collider* pCollider);
	private:
		void UpdateRigidBodyTransform();
		void CollisionDetection();
		void ResolveCollision(const struct Manifold& manifold);

		// 아래 함수의 Collider들은 모두 회전이 적용된 월드 좌표를 갖는다고 가정
		// VirtualAlloc Page allocation + SIMD 처리로 전처리 과정에서 월드변환 모두 적용
		// 
		// void BroadPhase();
		// void NarrowPhase();
	private:
		static bool CheckCollisionAABBToAABB(const struct AABB& a, const struct AABB& b);
		static void ApplyWorldTransform(class HullCollider& hull);
		static void Integrate(class RigidBody& rigidBody);
		static bool CheckCirclevsCircleCollision(struct Manifold& manifold, const class Collider& circleColliderA, const class Collider& circleColliderB);
		// static bool CheckCirclevsBoxCollision(struct Manifold& manifold, const class Collider& circleCollider, const class Collider& boxCollider);
		// static bool CheckBoxvsCircleCollision(struct Manifold& manifold, const class Collider& boxCollider, const class Collider& circleCollider);
		// static bool CheckBoxvsBoxCollision(struct Manifold& manifold, const class Collider& boxColliderA, const class Collider& boxColliderB);
		// static bool CheckBoxvsHullCollision(struct Manifold& manifold, const class Collider& boxCollider, const class Collider& hullCollider);
		// static bool CheckHullvsBoxCollision(struct Manifold& manifold, const class Collider& hullCollider, const class Collider& boxCollider);
		static bool CheckHullvsHullCollision(struct Manifold& manifold, const class Collider& hullColliderA, const class Collider& hullColliderB);
		static bool CheckCirclevsHullCollision(struct Manifold& manifold, const class Collider& circleCollider, const class Collider& hullCollider);
		static bool CheckHullvsCircleCollision(struct Manifold& manifold, const class Collider& hullCollider, const class Collider& circleCollider);
		static const math::Vector2 GetClosestPointOnLineSegementFromOrigin(const math::Vector2& a, const math::Vector2& b);
		static void GetClosestVectorBetweenCircleCenterAndHull(const math::Vector2 point, const class HullCollider& hull, math::Vector2& v, real& dist);
		// static void FaceQueryBoxvsHull(struct Query& query, const class BoxCollider& ref, const class HullCollider& inc);
		// static void FaceQueryHullvsBox(struct Query& query, const class HullCollider& ref, const class BoxCollider& inc);
		static void FaceQueryHullvsHull(struct Query& query, const class HullCollider& ref, const class HullCollider& inc);
		static void FaceQueryHullvsCircle(struct Query& query, const class HullCollider& ref, const class CircleCollider& inc);
		// static void FaceQueryBoxvsBox(struct Query& query, const class BoxCollider& ref, const class BoxCollider& inc);
		// static void ComputeBoxvsBoxContactPoint(struct Manifold& manifold, uint16_t refFaceIndex, uint16_t incVertexIndex);
		// static void ComputeBoxvsHullContactPoint(struct Manifold& manifold, uint16_t refFaceIndex, uint16_t incVertexIndex);
		// static void ComputeHullvsBoxContactPoint(struct Manifold& manifold, uint16_t refFaceIndex, uint16_t incVertexIndex);
		static void ComputeHullvsHullContactPoint(struct Manifold& manifold, uint16_t refFaceIndex, uint16_t incVertexIndex);
		static inline real FrictionSolver(real friction1, real friction2)
		{
			return (friction1 + friction2) * real(0.5);
			// return std::sqrt(friction1 * friction1 + friction2 * friction2);
		}
	private:
		typedef bool (*CDF)(struct Manifold& manifold, const class Collider& colliderA, const class Collider& colliderB);
		static CDF cdft[static_cast<size_t>(ColliderType::COLLIDER_TYPE_COUNT)][static_cast<size_t>(ColliderType::COLLIDER_TYPE_COUNT)];
		static Simplex s_simplex[1];
		std::list<class Collider*> m_colliders;
#ifdef _DEBUG
	public:
		struct ContactPoint
		{
		public:
			ContactPoint(real _x, real _y) : x(_x), y(_y) {}
			real x, y;
		};
		static std::list<ContactPoint> m_cpList;
#endif // _DEBUG

	};
}
