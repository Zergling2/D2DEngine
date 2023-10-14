#pragma once

#include "Components\Derived\GraphicComponents\Derived\Geometry\Geometry.h"

namespace D2DEngine
{
	// Final class
	class RoundedRectangle : public Geometry
	{
	public:
		RoundedRectangle(GameObject* pGameObj, const D2D1_ROUNDED_RECT& shape);
		virtual ~RoundedRectangle();

		inline ID2D1RoundedRectangleGeometry* GetGeometry() { return m_pShape; }
		inline const ID2D1RoundedRectangleGeometry* GetGeometry() const { return m_pShape; }
	public:
		ID2D1RoundedRectangleGeometry* m_pShape;
	};
}
