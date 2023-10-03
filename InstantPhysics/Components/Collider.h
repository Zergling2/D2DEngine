#pragma once

namespace ip
{
	enum ColliderType
	{
		Circle,
		Box,
		Polygon
	};
	class Collider abstract
	{
	public:
		Collider(ColliderType type)
			: m_type(type)
		{
		}
		virtual ~Collider() = 0;
		inline ColliderType GetType() const { return m_type; }
		virtual void CalculateAABB(struct AABB* const pAABB, const class RigidBody* const pRigidBody) = 0;
		ColliderType m_type;
	};
}