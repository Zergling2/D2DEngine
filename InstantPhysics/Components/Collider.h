#pragma once

#include "Common\Math2D.h"
#include "Core\Acc\AABB.h"

namespace ip
{
	class ColliderType
	{
	public:
		static constexpr uint16_t CIRCLE = 0;
		static constexpr uint16_t CAPSULE = 1;
		static constexpr uint16_t HULL = 2;
		static constexpr uint16_t LINE = 3;
		static constexpr uint16_t COLLIDER_TYPE_COUNT = 4;
	};

	class Collider abstract
	{
		friend class Processor;
		friend class Simplex;
	public:
		Collider(uint16_t type)
			: m_type(type)
			, m_rotateCounter(0)
			, m_position(real(0.0), real(0.0))
			, m_radian(real(0.0))
			, m_lastWTPosition(real(0.0), real(0.0))
			, m_lastWTRadian(real(0.0))
			, m_pRigidBody(nullptr)
		{
		}
		Collider(uint16_t type, const math::Vector2& position, real rotation)
			: m_type(type)
			, m_rotateCounter(0)
			, m_position(position)
			, m_radian(rotation)
			, m_lastWTPosition(real(0.0), real(0.0))
			, m_lastWTRadian(real(0.0))
			, m_pRigidBody(nullptr)
		{
		}
		virtual ~Collider();
		bool CreateRigidBody(real mass, real inertia, real restitution, real sf, real df, bool isStatic);

		__forceinline class RigidBody& GetRigidBody() const { return *m_pRigidBody; }

		__forceinline const math::Vector2& GetPosition() const { return m_position; }

		__forceinline void SetPosition(const math::Vector2& pos) { m_position = pos; }

		__forceinline void SetPosition(real x, real y) { m_position.x = x; m_position.y = y; }

		__forceinline void SetPositionX(real x) { m_position.x = x; }

		__forceinline void SetPositionY(real y) { m_position.y = y; }

		__forceinline void Move(real dx, real dy) { m_position.x += dx; m_position.y += dy; }

		__forceinline void MoveX(real dx) { m_position.x += dx; }

		__forceinline void MoveY(real dy) { m_position.y += dy; }

		// 라디안 단위로 회전 값 전달
		__forceinline real GetRotation() const { return m_radian; }

		// 전달된 radian 값 만큼 회전된 상태로 설정
		void SetRotation(real radian);

		// 전달된 radian 값 만큼 회전
		void Rotate(real radian);
	private:
		virtual const math::Vector2 SupportPoint(const math::Vector2& direction) const = 0;
		virtual void ComputeAABB() = 0;
	protected:
		uint16_t m_type;
		uint16_t m_rotateCounter;
		AABB m_AABB;
		math::Vector2 m_position;
		real m_radian;						// orientation
		math::Vector2 m_lastWTPosition;		// for simd world transform		World 변환 함수에서만 갱신
		real m_lastWTRadian;				// for simd world transform		World 변환 함수에서만 갱신
		class RigidBody* m_pRigidBody;
	};
}