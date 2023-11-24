#pragma once

#include "Common\Math2D.h"
#include "Components\Collider.h"
#include "Components\RigidBody.h"
#include "Components\CollisionCallback.h"
#include "Components\Collision.h"

namespace ip
{
	enum Space
	{
		World,
		Local
	};

	class PhysicsObject
	{
		friend class PhysicsWorld;
		friend class Collider;
		friend class CircleCollider;
		friend class CapsuleCollider;
		friend class HullCollider;
		friend class LineCollider;
		friend class AABBArray;
		friend class Simplex;
		friend class MTUpdatePhysicsObject;
	public:
		PhysicsObject();
		virtual ~PhysicsObject();

		inline class Collider& GetCollider() const { return *m_pCollider; }

		// ====================================================================================
		// Static RigidBody Object의 경우 시뮬레이션 도중 Transform 변경 후에는 반드시 이 함수를 한 번 호출해야 합니다.
		void StaticReady();
		// ====================================================================================

		// ====================================================================================
		bool CreateCircleCollider(CollisionLayer layer, real radius);
		bool CreateCapsuleCollider(CollisionLayer layer, real radius, real length);
		bool CreateBoxCollider(CollisionLayer layer, real width, real height);
		bool CreateHullCollider(CollisionLayer layer, math::Vector2 convexCCW[], uint16_t vertexCount);
		void RemoveCollider();
		bool CreateRigidBody(BodyType type,
			real mass, real inertia, real bounce,
			real staticFriction, real dynamicFriction, real gravityScale);
		void RemoveRigidBody();
		// ====================================================================================

		inline const math::Vector2 GetLocalDirection() const { return math::Vector2::Rotated(math::Vector2::right, m_rotation); }

		inline const math::Vector2 GetPosition() const { return m_position; }
		inline void SetPosition(const math::Vector2& position) { m_position = position; }
		inline void SetPosition(real x, real y) { m_position.x = x; m_position.y = y; }

		void Translate(const math::Vector2& translation, Space cs = Local);
		void Translate(real x, real y, Space cs = Local);

		inline void AddForce(const math::Vector2& force) { m_rigidBody.m_force += force; }
		inline void AddForce(real x, real y) { m_rigidBody.m_force.x += x; m_rigidBody.m_force.y += y; }
		void AddRelativeForce(const math::Vector2 force);
		void AddRelativeForce(real x, real y);

		inline const math::Vector2 GetVelocity() const { return m_rigidBody.m_linearVelocity; }
		inline void SetVelocity(const math::Vector2& velocity) { m_rigidBody.m_linearVelocity = velocity; }
		inline void SetVelocity(real x, real y) { m_rigidBody.m_linearVelocity.x = x; m_rigidBody.m_linearVelocity.y = y; }
		inline void AddVelocity(const math::Vector2 velocity) { m_rigidBody.m_linearVelocity += velocity; }
		inline void AddVelocity(real x, real y) { m_rigidBody.m_linearVelocity.x += x; m_rigidBody.m_linearVelocity.y += y; }

		inline real GetRotation() const { return m_rotation; }		// Radian
		void SetRotation(real angle);								// Radian
		void Rotate(real angle);									// Radian

		inline void AddTorque(real torque) { m_rigidBody.m_torque += torque; }
		inline real GetAngularVelocity() const { return m_rigidBody.m_angularVelocity; }
		inline void SetAngularVelocity(real w) { m_rigidBody.m_angularVelocity = w; }
		inline void AddAngularVelocity(real w) { m_rigidBody.m_angularVelocity += w; }

		// void FreezeRotation();
		
		virtual void OnTriggerEnter2D(Collider& other);
		virtual void OnTriggerStay2D(Collider& other);
		virtual void OnTriggerExit2D(Collider& other);
		virtual void OnCollisionEnter2D(Collision& other);
		virtual void OnCollisionStay2D(Collision& other);
		virtual void OnCollisionExit2D(Collision& other);

		/*
		OnTrigger_____ 호출 조건: 두 물체에 모두 Collider가 존재하며 두 물체 중 1개 이상의 컴포넌트에 isTrigger가 활성화 되어 있어야 합니다.
		OnCollision_____ 호출 조건: 두 물체에 모두 Collider가 존재하며 두 물체의 Collider에 isTrigger가 비활성화 되어 있어야 합니다.

		isTrigger 활성화 시 
		*/
	private:
		bool m_isDead;	// for the engine
		math::Vector2 m_position;
		math::Vector2 m_scale;
		real m_rotation;			// Unit: Radian
		RigidBody m_rigidBody;
		class Collider* m_pCollider;
		class PhysicsWorld* m_pPhysWorld;
	};
}
