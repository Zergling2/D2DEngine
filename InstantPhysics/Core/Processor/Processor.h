#pragma once

#include "Type.h"
#include "Common\Math2D.h"
#include "Components\Manifold.h"
#include <list>

namespace ip
{
	class Processor
	{
	public:
		Processor();
		~Processor();

		void Update();
		class CircleCollider* CreateCircleCollider(real radius);
		class PolygonCollider* CreatePolygonCollider(const math::Vector2 convexShapeVerticesCCW[], uint32_t vertexCount);
		bool RemoveCollider(class Collider* pCollider);
	private:
		void UpdateRigidBodyTransform();
		bool CheckAABBCollision(const struct AABB* const pAABB1, const struct AABB* const pAABB2) const;
		void CollisionDetection();
		void ResolveCollision();

		// �Ʒ� �Լ��� Collider���� ��� ȸ���� ����� ���� ��ǥ�� ���´ٰ� ����
		// VirtualAlloc Page allocation + SIMD ó���� ��ó�� �������� ���庯ȯ ��� ����

		// bool CheckBoxToCircleCollision(struct Manifold* pManifold, const class RigidBody* const pBox, const class RigidBody* const pCircle);
		// __forceinline bool CheckCircleToBoxCollision(struct Manifold* pManifold, const class RigidBody* const pCircle, const class RigidBody* const pBox) { return CheckBoxToCircleCollision(pManifold, pBox, pCircle); }
		// 
		// bool CheckBoxToBoxCollision(struct Manifold* pManifold, const class RigidBody* const pBox1, const class RigidBody* const pBox2);

		// void BroadPhase();
		// void NarrowPhase();
	private:
		static void WorldTransformPolygonVertices(class PolygonCollider& polygonCollider);
		// static const math::Vector2 MinkowskiSupport(const class PolygonCollider& polygonColliderA, const class PolygonCollider& polygonColliderB, const math::Vector2& direction);
		static void Step(class RigidBody& rigidBody);
		static bool CheckCollisionCircleToCircle(Manifold* pManifold, const class CircleCollider& circleColliderA, class CircleCollider& circleColliderB);
		static bool CheckCollisionCircleToPolygon(Manifold* pManifold, const class CircleCollider& circleCollider, const class PolygonCollider& polygonCollider);
		static bool CheckCollisionPolygonToPolygon(Manifold* pManifold, const class PolygonCollider& polygonColliderA, const class PolygonCollider& polygonColliderB);
	private:
		std::list<class Collider*> m_colliders;
		std::list<Manifold> m_resolveList;
	};
}
