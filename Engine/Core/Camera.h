#pragma once

#include "Headers\Essential.h"

namespace D2DEngine
{
	class Camera
	{
	public:
		Camera(float fovWidth, float fovHeight, float zoom = 1.0f);
		virtual ~Camera();
	public:
		const D2D1::Matrix3x2F GetViewMatrix() const;
		const D2D1::Matrix3x2F GetProjectionMatrix() const;
		inline const D2D1::Matrix3x2F GetViewProjectionMatrix() const { return GetViewMatrix() * GetProjectionMatrix(); }
		__forceinline void Move(float dx, float dy) { m_pos.x += dx; m_pos.y += dy; }
		__forceinline void MoveX(float dx) { m_pos.x += dx; }
		__forceinline void MoveY(float dy) { m_pos.y += dy; }
		__forceinline void SetPosition(const D2D_VECTOR_2F& pos) { m_pos = pos; }
		__forceinline void SetPosition(float x, float y) { m_pos.x = x; m_pos.y = y; }
		__forceinline void SetScale(float x, float y) { m_scale.x = x; m_scale.y = y; }
		void Rotate(degree d);
		void SetRotation(degree d);
		inline float GetCurrentZoom() const { return m_zoom; }
		inline void Zoom(float zoom) { m_zoom = Mathf::Clamp(zoom, 0.1f, 5.0f); }
	private:
		D2D_VECTOR_2F m_pos;
		D2D_VECTOR_2F m_scale;
		degree m_rotation;
		float m_fovWidth;
		float m_fovHeight;
		float m_zoom;
		uint16_t m_rotateCounter;
		uint16_t m_setRotCounter;
	};
}
