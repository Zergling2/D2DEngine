#pragma once

#include "Components\Simplex.h"

namespace ip
{
	class MTJob abstract
	{
	public:
		virtual void Do() = 0;
	};

	class MTDoNothing : public MTJob
	{
	public:
		virtual void Do() override;
	};

	class MTUpdatePhysicsObject : public MTJob
	{
	public:
		virtual void Do() override;
		class PhysicsWorld* m_pPhysWorld;
		class PhysicsObject** m_pBeginAddr;
		size_t m_objCount;
	};

	class MTNPCollisionDetection : public MTJob
	{
	public:
		virtual void Do() override;
		class Collider* m_pColliderA;
		class Collider* m_pColliderB;
	};
}