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

		// 아래 함수의 Collider들은 모두 회전이 적용된 월드 좌표를 갖는다고 가정
		// VirtualAlloc Page allocation + SIMD 처리로 전처리 과정에서 월드변환 모두 적용
		bool CheckCircleToCircleCollision(Manifold* pManifold, const class RigidBody* const pCircle1, const class RigidBody* const pCircle2);
		bool CheckBoxToCircleCollision(const class RigidBody* const pBox, const class RigidBody* const pCircle);
		bool CheckBoxToBoxCollision(const class RigidBody* const pBox1, const class RigidBody* const pBox2);
	};
}
