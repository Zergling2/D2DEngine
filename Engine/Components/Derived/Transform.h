#pragma once

#include "Headers\Essential.h"
#include "Components\Component.h"

namespace D2DEngine
{
	// All objects have this component.
	class Transform : public Component
	{
	private:
		inline void Copy(const Transform& other)
		{
			m_position = other.m_position;
			m_scale = other.m_scale;
			m_rotation = other.m_rotation;
		}
	public:
		// Returns the Object Axis in World Space (오브젝트의 월드 좌표 기준 XY좌표축 구하기)
		static void GetAxis(const Transform& transform, D2D_VECTOR_2F* xAxis, D2D_VECTOR_2F* yAxis);
	public:
		inline Transform(class GameObject* pGameObj)
			: Component(pGameObj)
			, m_position{ 0.0f, 0.0f }
			, m_scale{ 1.0f, 1.0f }
			, m_rotation(0.0f)
		{
		}
		inline Transform(class GameObject* pGameObj, const Transform& ref)
			: Component(pGameObj)
		{
			Copy(ref);
		}
		inline Transform(class GameObject* pGameObj, float posX, float posY, float rot, float scaleX, float scaleY)
			: Component(pGameObj)
			, m_position{ posX, posY }
			, m_scale{ scaleX, scaleY }
			, m_rotation(rot)
		{
		}
		inline Transform& operator=(const Transform& ref)
		{
			Copy(ref);
		}
		inline void SetPosition(float x, float y)
		{
			m_position.x = x;
			m_position.y = y;
		}
		inline void SetPosition(const D2D_VECTOR_2F& pos)
		{
			m_position = pos;
		}
		inline void Move(float x, float y)
		{
			m_position.x += x;
			m_position.y += y;
		}
		inline void Move(const D2D_VECTOR_2F& displacement)
		{
			m_position.x += displacement.x;
			m_position.y += displacement.y;
		}

		// The rotation parameter is added to the current rotation value.
		inline void Rotate(float angle)
		{
			m_rotation += angle;
		}

		// Overwrites the current rotation value.
		inline void SetRotation(float newAngle)
		{
			m_rotation = newAngle;
		}

		// The scale parameter is multiplied by the current value.
		inline void Scale(float x, float y)
		{
			m_scale.x *= x;
			m_scale.y *= y;
		}

		// The scale parameter is multiplied by the current value.
		inline void Scale(const D2D_VECTOR_2F& scale)
		{
			m_scale.x *= scale.x;
			m_scale.y *= scale.y;
		}

		// Overwrites the current scale value.
		inline void SetScale(float newScaleX, float newScaleY)
		{
			m_scale.x = newScaleX;
			m_scale.y = newScaleY;
		}

		// Overwrites the current scale value.
		inline void SetScale(const D2D_VECTOR_2F& newScale)
		{
			m_scale = newScale;
		}

		inline const D2D1::Matrix3x2F GetWorldMatrix() const
		{
			return
				D2D1::Matrix3x2F::Scale(m_scale.x, m_scale.y) *
				D2D1::Matrix3x2F::Rotation(m_rotation) *
				D2D1::Matrix3x2F::Translation(m_position.x, m_position.y);
		}
	public:
		D2D_VECTOR_2F m_position;
		D2D_VECTOR_2F m_scale;
		float m_rotation;		// degree
	};
}
