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
		const D2D1::Matrix3x2F GetViewMatrix();
		const D2D1::Matrix3x2F GetProjectionMatrix();
		inline void Translate(float x, float y) { m_pos.x += x; m_pos.y += y; }
		inline void SetPosition(const D2D_VECTOR_2F& pos) { m_pos = pos; }
		inline void SetPosition(float x, float y) { m_pos.x = x; m_pos.y = y; }
		inline void SetScale(float x, float y) { m_scale.x = x; m_scale.y = y; }
		inline void SetOrientation(Degree degree) { m_orientation = degree; }
		void Rotate(Degree degree);
		inline float GetCurrentZoom() const { return m_zoom; }
		inline void Zoom(float zoom) { m_zoom = Mathf::Clamp(zoom, 0.1f, 5.0f); }
	private:
		D2D_VECTOR_2F m_pos;
		D2D_VECTOR_2F m_scale;
		Degree m_orientation;
		float m_fovWidth;
		float m_fovHeight;
		float m_zoom;
	};
}
