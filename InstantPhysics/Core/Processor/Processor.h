#pragma once

#include "Type.h"
#include "Common\Math2D.h"

namespace ip
{
	struct Manifold
	{
		class RigidBody* pRigidBodyA;
		class RigidBody* pRigidBodyB;
		real penetrationDepth;
		math::Vector2 collisionNormal;
	};

	class Processor
	{
	public:
		Processor();
		~Processor();
	private:
		bool CheckAABBCollision(const class AABB* const pAABB1, const class AABB* const pAABB2);

		// �Ʒ� �Լ��� Collider���� ��� ȸ���� ����� ���� ��ǥ�� ���´ٰ� ����
		// VirtualAlloc Page allocation + SIMD ó���� ��ó�� �������� ���庯ȯ ��� ����
		bool CheckCircleToCircleCollision(Manifold* pManifold, const class RigidBody* const pCircle1, const class RigidBody* const pCircle2);
		bool CheckBoxToCircleCollision(const class RigidBody* const pBox, const class RigidBody* const pCircle);
		bool CheckBoxToBoxCollision(const class RigidBody* const pBox1, const class RigidBody* const pBox2);
	};
}
