#pragma once

#include "Components\Derived\GraphicComponents\Derived\Geometry\Geometry.h"

namespace D2DEngine
{
	class Ellipse : public Geometry
	{
	public:
		// Parameters
		// 1. Create with D2D1::Ellipse function.
		Ellipse(GameObject* pGameObj, const D2D1_ELLIPSE& shape);
		virtual ~Ellipse();

		inline ID2D1EllipseGeometry* GetGeometry() { return m_pShape; }
		inline const ID2D1EllipseGeometry* GetGeometry() const { return m_pShape; }
	public:
		ID2D1EllipseGeometry* m_pShape;
	};
}
