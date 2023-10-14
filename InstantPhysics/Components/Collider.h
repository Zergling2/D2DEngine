#pragma once

#include "Common\Math2D.h"
#include "Core\Acc\AABB.h"

namespace ip
{
	/*
	class ColliderType
	{
	public:
		static const unsigned int CIRCLE = 0x00000001U;		// 00000000 00000000 00000000 00000001
		static const unsigned int BOX = 0x00000002U;		// 00000000 00000000 00000000 00000010
		static const unsigned int POLYGON = 0x00000004U;	// 00000000 00000000 00000000 00000100
	};
	*/

	enum class ColliderType
	{
		Circle,
		Polygon
	};

	class Collider abstract
	{
	public:
		Collider(ColliderType type)
			: m_type(type)
			, m_position(real(0.0), real(0.0))
			, m_orientation(real(0.0))
			, m_pRigidBody(nullptr)
		{
		}
		Collider(ColliderType type, const math::Vector2& position, real orientation)
			: m_type(type)
			, m_position(position)
			, m_orientation(orientation)
			, m_pRigidBody(nullptr)
		{
		}
		virtual ~Collider() = 0 {}
		inline ColliderType GetType() const { return m_type; }
		inline class RigidBody* Body() const { return m_pRigidBody; }
		virtual const math::Vector2 SupportPoint(const math::Vector2& direction) const = 0;
		virtual void ComputeAABB() = 0;
	public:
		ColliderType m_type;
		AABB m_AABB;
		math::Vector2 m_position;
		real m_orientation;
		class RigidBody* m_pRigidBody;
	};
}